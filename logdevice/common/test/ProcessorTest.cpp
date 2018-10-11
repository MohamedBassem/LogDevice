/**
 * Copyright (c) 2017-present, Facebook, Inc. and its affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <sys/types.h>
#include <thread>
#include <vector>

#include <folly/Memory.h>
#include "logdevice/common/debug.h"
#include "logdevice/common/EventLoopHandle.h"
#include "logdevice/common/LibeventTimer.h"
#include "logdevice/common/ExponentialBackoffTimer.h"
#include "logdevice/common/NoopTraceLogger.h"
#include "logdevice/common/Processor.h"
#include "logdevice/common/Request.h"
#include "logdevice/common/test/TestUtil.h"
#include "logdevice/common/types_internal.h"
#include "logdevice/common/Worker.h"

using namespace facebook::logdevice;

static std::map<pthread_t, int> requests_per_thread;
static std::mutex requests_per_thread_map_lock;

/**
 * A test request whose execute() method bumps a per-thread counter.
 */
struct ThreadCountingRequest : public Request {
  explicit ThreadCountingRequest(int thread_affinity)
      : Request(RequestType::TEST_PROCESSOR_THREAD_COUNTING_REQUEST),
        thread_affinity_(thread_affinity) {}

  Request::Execution execute() override {
    std::lock_guard<std::mutex> guard(requests_per_thread_map_lock);
    pthread_t id = EventLoop::onThisThread()->getThread();
    ++requests_per_thread[id];
    return Execution::COMPLETE;
  }

  int getThreadAffinity(int /*nthreads*/) override {
    return thread_affinity_;
  }

  int thread_affinity_;
};

class ProcessorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    dbg::assertOnData = true;

    // In order for writes to closed pipes to return EPIPE (instead of bringing
    // down the process), which we rely on to detect shutdown, ignore SIGPIPE.
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPIPE, &sa, &oldact);
  }
  void TearDown() override {
    sigaction(SIGPIPE, &oldact, nullptr);
  }

 private:
  struct sigaction oldact {};
};

/**
 * Tests that a processor with a bunch of threads shuts down correctly.  Try
 * it with valgrind!
 */
TEST_F(ProcessorTest, ShutdownTest) {
  Settings settings = create_default_settings<Settings>();
  settings.num_workers = 16;
  make_test_processor(settings);
}

TEST_F(ProcessorTest, ThreadTargetingTest) {
  Settings settings = create_default_settings<Settings>();
  settings.num_workers = 3;
  auto processor = make_test_processor(settings);

  // Send 100 requests to the first thread and one to the other two.
  for (int t = 0; t < 3; ++t) {
    int n = t == 0 ? 100 : 1;
    for (int i = 0; i < n; ++i) {
      std::unique_ptr<Request> req = std::make_unique<ThreadCountingRequest>(t);
      int rv = processor->postRequest(req);
      ld_check(rv == 0);
    }
  }

  // Wait for the work to finish
  processor.reset();

  std::vector<int> counts;
  for (const auto& it : requests_per_thread) {
    counts.push_back(it.second);
  }
  sort(counts.begin(), counts.end());
  std::vector<int> expected{1, 1, 100};
  EXPECT_EQ(expected, counts);
}

struct TargetedNoopRequest : public Request {
  explicit TargetedNoopRequest(worker_id_t target)
      : Request(RequestType::TEST_PROCESSOR_TARGETED_NOOP_REQUEST),
        target_(target) {}
  int getThreadAffinity(int /*nthreads*/) override {
    return target_.val_;
  }
  Request::Execution execute() override {
    return Execution::COMPLETE;
  }
  worker_id_t target_;
};

struct PostToOtherWorkerRequest : public Request {
  PostToOtherWorkerRequest(worker_id_t us, worker_id_t them)
      : Request(RequestType::TEST_PROCESSOR_POST_TO_OTHER_WORKER_REQUEST),
        us_(us),
        them_(them) {}

  int getThreadAffinity(int /*nthreads*/) override {
    return us_.val_;
  }
  Request::Execution execute() override {
    for (int i = 0; i < 100; ++i) {
      std::unique_ptr<Request> req =
          std::make_unique<TargetedNoopRequest>(them_);
      int rv = Worker::onThisThread()->processor_->postRequest(req);
      ld_info("us_ = %d, rv = %d", us_.val_, rv);
      /* sleep override */
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return Execution::COMPLETE;
  }
  worker_id_t us_, them_;
};

/**
 * Processor should shut down cleanly even if there are still workers trying
 * to post Requests to each other.
 */
TEST_F(ProcessorTest, ShutdownPingPongTest) {
  Settings settings = create_default_settings<Settings>();
  settings.num_workers = 2;
  auto processor = make_test_processor(settings);

  for (int w = 0; w < 2; ++w) {
    std::unique_ptr<Request> req = std::make_unique<PostToOtherWorkerRequest>(
        worker_id_t(w), worker_id_t(w ^ 1));
    int rv = processor->postRequest(req);
    ld_check(rv == 0);
  }

  /* sleep override */
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  processor.reset();
}

// Check that EventLoop and EventLoopHandle can function without being wrapped
// in Worker and Processor. Request, LibeventTimer and ExponentialBackoffTimer
// should all work. This is used by some tools.
TEST_F(ProcessorTest, UseEventLoopDirectly) {
  Alarm alarm(std::chrono::seconds(10));

  EventLoopHandle handle(
      new EventLoop(), /* capacity */ 2, /* requests per iteration */ 1);
  handle.start();

  struct Req : public Request {
    std::function<void()> f;

    explicit Req(std::function<void()> _f) : f(_f) {}

    Execution execute() override {
      f();
      return Execution::COMPLETE;
    }
  };

  Semaphore sem0, sem1, sem2;
  // Post a slow request, then 2 fast requests.
  {
    std::unique_ptr<Request> rq = std::make_unique<Req>([&] {
      sem0.post();
      sem1.wait();
    });
    ASSERT_EQ(0, handle.postRequest(rq));
  }
  // Post a fast request.
  {
    std::unique_ptr<Request> rq = std::make_unique<Req>([&] { sem2.post(); });
    ASSERT_EQ(0, handle.postRequest(rq));
  }
  // Wait for the slow request to start.
  sem0.wait();
  // Post another fast request.
  {
    std::unique_ptr<Request> rq = std::make_unique<Req>([&] { sem2.post(); });
    ASSERT_EQ(0, handle.postRequest(rq));
  }
  // Now the queue should be full, and the next request should fail to post.
  {
    std::unique_ptr<Request> rq = std::make_unique<Req>([&] { ADD_FAILURE(); });
    int rv = handle.postRequest(rq);
    EXPECT_EQ(-1, rv);
    EXPECT_EQ(E::NOBUFS, err);
  }

  EXPECT_EQ(0, sem2.value());
  sem1.post();
  sem2.wait();
  sem2.wait();
  EXPECT_EQ(0, sem2.value());

  LibeventTimer timer;
  {
    std::unique_ptr<Request> rq = std::make_unique<Req>([&] {
      timer.assign(handle.get()->getEventBase(), [&] { sem2.post(); });
      timer.activate(std::chrono::milliseconds(1));
    });
    ASSERT_EQ(0, handle.blockingRequest(rq));
  }
  sem2.wait();
  EXPECT_EQ(0, sem2.value());
}

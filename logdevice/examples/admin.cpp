#include <iostream>

#include <folly/init/Init.h>
#include <folly/io/async/EventBase.h>
#include <thrift/lib/cpp/async/TAsyncSocket.h>
#include <thrift/lib/cpp2/async/HeaderClientChannel.h>

#include "logdevice/admin/if/gen-cpp2/AdminAPI.h"
#include "logdevice/admin/if/gen-cpp2/AdminAPIAsyncClient.h"

using apache::thrift::async::TAsyncSocket;

int main(int argc, char* argv[]) {
  folly::init(&argc, &argv);

  folly::EventBase eventBase;

  // Create a Thrift client.
  auto socket = TAsyncSocket::newSocket(
     &eventBase, folly::SocketAddress("::1", 48327), 10000);
  auto channel =
            apache::thrift::HeaderClientChannel::newChannel(std::move(socket));
  auto client =
      std::make_unique<facebook::logdevice::thrift::AdminAPIAsyncClient>(std::move(channel));

  auto pid = client->sync_getPid();

  std::cout << pid << std::endl;

  return 0;
}

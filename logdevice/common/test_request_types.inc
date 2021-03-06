/**
 * Copyright (c) 2017-present, Facebook, Inc. and its affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
/* can be included multiple times */

#ifndef REQUEST_TYPE
#error REQUEST_TYPE() macro not defined
#define REQUEST_TYPE(...)
#endif

REQUEST_TYPE(TEST_REQUEST)
REQUEST_TYPE(TEST_ADVANCE_EPOCH_REQUEST)
REQUEST_TYPE(TEST_APPENDER_REQUEST)
REQUEST_TYPE(TEST_DUMP_METADATA_GET_ALL_METADATA_REQUEST)
REQUEST_TYPE(TEST_UPDATE_EPOCH_BEGIN_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_ADD_CLIENT_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_COUNTING_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_ON_CLIENT_CLOSE_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_SEND_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_SEND_VARLEN_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_SEND_STORED_WITH_TIMEOUT_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_SENDER_BASIC_SEND_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_SLEEPING_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_SLOW_REQUEST)
REQUEST_TYPE(TEST_MESSAGING_SOCKET_CONNECT_REQUEST)
REQUEST_TYPE(TEST_NC_SEND_MESSAGE_REQUEST)
REQUEST_TYPE(TEST_PROCESSOR_POST_TO_OTHER_WORKER_REQUEST)
REQUEST_TYPE(TEST_PROCESSOR_TARGETED_NOOP_REQUEST)
REQUEST_TYPE(TEST_PROCESSOR_THREAD_COUNTING_REQUEST)
REQUEST_TYPE(TEST_REQUEST_QUEUE_CONDITION_REQUEST)
REQUEST_TYPE(TEST_REQUEST_QUEUE_COUNTING_REQUEST)
REQUEST_TYPE(TEST_REQUEST_QUEUE_SIMPLE_REQUEST)
REQUEST_TYPE(TEST_SENDMESSAGE_EXPECT_BADPROTO_REQUEST)
REQUEST_TYPE(TEST_SENDMESSAGE_EXPECT_TWO_MESSAGES_SENT)
REQUEST_TYPE(TEST_SENDMESSAGE_ON_CLOSE_INVALID_CLUSTER_REQUEST)
REQUEST_TYPE(TEST_SENDMESSAGE_ON_CLOSE_PROTONOSUPPORT_REQUEST)
REQUEST_TYPE(TEST_STORAGE_THREADPOOL_MAKE_TASK_REQUEST)
REQUEST_TYPE(TEST_STORAGE_THREADPOOL_PUT_STORAGE_TASK_REQUEST)
REQUEST_TYPE(TEST_WRITE_STORAGE_TASK_REQUEST)
REQUEST_TYPE(TEST_ZK_CONVERT_LEGACY_METADATA_REQUEST)
REQUEST_TYPE(TEST_ZK_LCE_REQUEST)
REQUEST_TYPE(TEST_ZK_NEXT_EPOCH_REQUEST)
REQUEST_TYPE(TEST_ZK_UPDATE_METADATA_REQUEST)

#undef REQUEST_TYPE

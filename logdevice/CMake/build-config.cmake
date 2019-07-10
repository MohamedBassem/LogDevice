# Copyright (c) 2017-present, Facebook, Inc. and its affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-new-ttp-matching")
set(LOGDEVICE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(LOGDEVICE_ADMIN_DIR "${LOGDEVICE_DIR}/admin")
set(LOGDEVICE_ADMIN_SERVER_DIR "${LOGDEVICE_DIR}/ops/admin_server")
set(LOGDEVICE_CLIENT_HEADER_DIR "${LOGDEVICE_DIR}/include")
set(LOGDEVICE_CLIENT_SRC_DIR "${LOGDEVICE_DIR}/lib")
set(LOGDEVICE_COMMON_DIR "${LOGDEVICE_DIR}/common")
set(LOGDEVICE_EXAMPLES_DIR "${LOGDEVICE_DIR}/examples")
set(LOGDEVICE_SERVER_DIR "${LOGDEVICE_DIR}/server")
set(LOGDEVICE_TEST_DIR "${LOGDEVICE_DIR}/test")

set(LOGDEVICE_STAGING_DIR "${CMAKE_BINARY_DIR}/staging")
set(LOGDEVICE_PYTHON_CLIENT_DIR "${LOGDEVICE_DIR}/clients/python")


# Setting Output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(UNIT_TEST_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)

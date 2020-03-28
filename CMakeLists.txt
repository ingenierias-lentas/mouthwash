cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(mouthwash VERSION 1.0)

include(cmake/CPM.cmake)

find_package(PkgConfig REQUIRED)

# Midifile setup
include(ExternalProject)
ExternalProject_Add(midifile
    BUILD_COMMAND "cd midifile && make library"
    INSTALL_COMMAND
        "cp midifile/lib ${CMAKE_CURRENT_BINARY_DIR} && cp midifile/include/*.h ${CMAKE_CURRENT_BINARY_DIR}"
)
set(MIDIFILE_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/lib/libmidifile.a")

# Boost setup
find_package(Boost 1.72 COMPONENTS thread date_time REQUIRED)
include_directories( ${Boost_INCLUDE_DIR} )

# Conan/Poco setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# LV2 setup
pkg_check_modules(LV2 REQUIRED IMPORTED_TARGET /l/l/lib/pkgconfig/lv2.pc)
pkg_check_modules(LILV REQUIRED IMPORTED_TARGET /l/l/lib/pkgconfig/lilv-0.pc)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
set(_LV2 PkgConfig::LV2)
set(_LILV PkgConfig::LILV)
set(_SNDFILE PkgConfig::SNDFILE)
#set(_LV2_BASE_DIR /l/l)
#include_directories("${_LV2_BASE_DIR}/include")
#set(_LILV_LIBLILV "${_LV2_BASE_DIR}/lib/liblilv-0.so")

# Protobuf setup
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC setup
#find_package(gRPC REQUIRED)
pkg_check_modules(gRPC IMPORTED_TARGET grpc++_unsecure)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP_UNSECURE PkgConfig::gRPC)
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
find_program(_GRPC_PYTHON_PLUGIN_EXECUTABLE grpc_python_plugin)
find_program(_GRPC_WEB_PLUGIN_EXECUTABLE protoc-gen-grpc-web)
#set(_REFLECTION gRPC::grpc++_reflection)
#set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
#set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)


# Proto files
get_filename_component(bottle_proto "protos/bottle.proto" ABSOLUTE)
get_filename_component(bottle_proto_path "${bottle_proto}" PATH)
get_filename_component(mix_proto "protos/mix.proto" ABSOLUTE)
get_filename_component(mix_proto_path "${mix_proto}" PATH)
get_filename_component(health_proto "protos/health.proto" ABSOLUTE)
get_filename_component(health_proto_path "${health_proto}" PATH)

# Generated sources
set(bottle_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/bottle.pb.cc")
set(bottle_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/bottle.pb.h")
set(bottle_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/bottle.grpc.pb.cc")
set(bottle_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/bottle.grpc.pb.h")

set(bottle_proto_py "${CMAKE_CURRENT_SOURCE_DIR}/python/bottle_pb.py")
set(bottle_grpc_py "${CMAKE_CURRENT_SOURCE_DIR}/python/bottle_grpc_pb.py")

add_custom_command(
    OUTPUT "${bottle_proto_srcs}" "${bottle_proto_hdrs}" "${bottle_grpc_srcs}"
    "${bottle_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${bottle_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${bottle_proto}"
    DEPENDS "${bottle_proto}")

set(mix_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/mix.pb.cc")
set(mix_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/mix.pb.h")
set(mix_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/mix.grpc.pb.cc")
set(mix_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/mix.grpc.pb.h")

set(mix_proto_py "${CMAKE_CURRENT_SOURCE_DIR}/python/mix_pb.py")
set(mix_grpc_py "${CMAKE_CURRENT_SOURCE_DIR}/python/mix_grpc_pb.py")

add_custom_command(
    OUTPUT "${mix_proto_srcs}" "${mix_proto_hdrs}" "${mix_grpc_srcs}"
    "${mix_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${mix_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${mix_proto}"
    DEPENDS "${mix_proto}")

set(health_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/health.pb.cc")
set(health_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/health.pb.h")
set(health_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/health.grpc.pb.cc")
set(health_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/health.grpc.pb.h")

set(health_proto_py "${CMAKE_CURRENT_SOURCE_DIR}/python/health_pb.py")
set(health_grpc_py "${CMAKE_CURRENT_SOURCE_DIR}/python/health_grpc_pb.py")

add_custom_command(
    OUTPUT "${health_proto_srcs}" "${health_proto_hdrs}" "${health_grpc_srcs}"
    "${health_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${health_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${health_proto}"
    DEPENDS "${health_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(mouthwash src/main.cc
    src/bar_helper.cc
    src/ingredient.cc
    src/recipe.cc
    src/bar.cc
    src/bartender.cc
    src/bottle_server.cc
    src/mixer_client.cc
    ${bottle_proto_srcs}
    ${bottle_grpc_srcs}
    ${mix_proto_srcs}
    ${mix_grpc_srcs}
    ${health_proto_srcs}
    ${health_grpc_srcs})

target_link_libraries(mouthwash
    #${_REFLECTION}
    ${MIDIFILE_LIBRARIES}
    ${Boost_LIBRARIES}
    ${_GRPC_GRPCPP_UNSECURE}
    ${_PROTOBUF_LIBPROTOBUF}
    ${_LV2}
    ${_LILV}
    ${_SNDFILE}
    ${CONAN_LIBS})

set_property(TARGET mouthwash PROPERTY CXX_STANDARD 17)

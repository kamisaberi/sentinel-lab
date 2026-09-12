cmake_minimum_required(VERSION 3.20)
project(sentinel-lab VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Research Hardware Toggles (Default: OpenVINO ON, TensorRT optional)
option(ENABLE_OPENVINO "Enable Intel OpenVINO Backend (CPU/NPU)" ON)
option(ENABLE_TENSORRT "Enable NVIDIA TensorRT Backend (GPU)"     OFF)
option(BUILD_TESTS     "Build unit tests and benchmarks"          ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    /usr/local/include
)

# Core System Dependencies
find_package(Threads REQUIRED)
find_library(BPF_LIB bpf REQUIRED)
find_library(ELF_LIB elf REQUIRED)
find_library(XINFER_LIB xinfer REQUIRED PATHS /usr/local/lib /usr/lib)
find_library(BLACKBOX_LIB blackbox REQUIRED PATHS /usr/local/lib /usr/lib)

set(LAB_SOURCES
    src/main.cpp
    src/engine.cpp
    src/ebpf_filter.cpp
    src/benchmarker.cpp
    src/network_ingest.cpp
    src/rest_api.cpp
)

if(ENABLE_OPENVINO)
    add_compile_definitions(SENTINEL_LAB_OPENVINO)
    find_package(OpenVINO REQUIRED)
endif()

if(ENABLE_TENSORRT)
    enable_language(CUDA)
    add_compile_definitions(SENTINEL_LAB_TENSORRT)
    find_package(CUDA REQUIRED)
    find_library(TENSORRT_NVINFER nvinfer REQUIRED)
    include_directories(${CUDA_INCLUDE_DIRS})
endif()

add_executable(sentinel_lab ${LAB_SOURCES})

target_link_libraries(sentinel_lab PRIVATE
    Threads::Threads
    ${BPF_LIB}
    ${ELF_LIB}
    z
    ${XINFER_LIB}
    ${BLACKBOX_LIB}
)

if(ENABLE_OPENVINO)
    target_link_libraries(sentinel_lab PRIVATE openvino::runtime)
endif()

if(ENABLE_TENSORRT)
    target_link_libraries(sentinel_lab PRIVATE ${CUDA_LIBRARIES} ${TENSORRT_NVINFER})
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
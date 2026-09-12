# Sentinel-Lab Unit Test Suite

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    /usr/local/include
)

# 1. Inference Unit Test
add_executable(test_inference test_inference.cpp ../src/engine.cpp)
target_link_libraries(test_inference PRIVATE ${XINFER_LIB} ${BLACKBOX_LIB} Threads::Threads)
if(ENABLE_OPENVINO)
    target_link_libraries(test_inference PRIVATE openvino::runtime)
endif()
add_test(NAME TestInference COMMAND test_inference)

# 2. eBPF Filter Harness Test
add_executable(test_ebpf test_ebpf.cpp ../src/ebpf_filter.cpp)
target_link_libraries(test_ebpf PRIVATE ${BPF_LIB} ${ELF_LIB} z Threads::Threads)
add_test(NAME TestEBPF COMMAND test_ebpf)

# 3. Benchmarker Statistical Precision Test
add_executable(test_benchmarker test_benchmarker.cpp ../src/benchmarker.cpp)
target_link_libraries(test_benchmarker PRIVATE Threads::Threads)
add_test(NAME TestBenchmarker COMMAND test_benchmarker)
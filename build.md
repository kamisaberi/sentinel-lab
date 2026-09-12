cd /home/kami/sentinel-lab

# 1. Compile kernel eBPF bytecode
./bpf/build_bpf.sh

# 2. Build C++ project and test suite
mkdir -p build && cd build
cmake .. -DENABLE_OPENVINO=ON -DBUILD_TESTS=ON
make -j$(nproc)

# 3. Run unit tests
ctest --output-on-failure

# 4. Run main benchmark daemon
sudo ./sentinel_lab
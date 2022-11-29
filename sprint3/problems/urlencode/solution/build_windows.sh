rm -r build

mkdir build && cd build

conan install .. -s build_type=Debug --build=missing

cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build . -j6
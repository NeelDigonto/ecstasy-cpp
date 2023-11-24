cmake.exe \
    -S . \
    -B build \
    -G "Visual Studio 17 2022"\
    -T v142 \
    -A x64 \
    -DCMAKE_INSTALL_PREFIX=D:/Program/\
    -DEIGEN_TEST_CXX11=ON \
    -DEIGEN_TEST_CUSTOM_CXX_FLAGS="/std:cxx11" \
&& cmake.exe \
    --build build \
    --target install \
    --config Release
bash scripts/cube.sh
bash scripts/qm.sh

mkdir -p build
/usr/sbin/cmake --no-warn-unused-cli                           \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE                  \
    -DCMAKE_BUILD_TYPE:STRING=Debug                            \
    -DCMAKE_C_COMPILER:FILEPATH=/bin/arm-none-eabi-gcc         \
    -DCMAKE_CXX_COMPILER:FILEPATH=/bin/arm-none-eabi-g++       \
    -S${PWD}                                                   \
    -B${PWD}/build                                             \
    -GNinja

/usr/sbin/cmake --build build --config Debug --target all -j 8

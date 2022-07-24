nodemon \
    -w wrec/meson.build -w wrec-test/meson.build \
    -w wrec-cli/meson.build \
    -w meson.build \
    -w "wrec/wrec.c" -w wrec/wrec.h \
    -w wrec-test/wrec-test.c -w wrec-test/wrec-test.h \
    -w wrec-cli/wrec-cli.c -w wrec-cli/wrec-cli.h \
    -x sh -- -c "clear && make build && clear && ./build/wrec-test/wrec-test||true"

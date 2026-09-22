to build
```cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake -G Ninja && cmake --build build```

to flash:
1. install https://www.st.com/en/development-tools/stm32cubeprog.html
2. find / -iname "STM32_Programmer_CLI*" to find the cli bin
3. run ```STM32_Programmer_CLI -c port=SWD -w build/LoRaX.elf -v -rst```

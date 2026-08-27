# PulseLink

![CI](https://github.com/Joaovsky/pulselink/actions/workflows/ci.yml/badge.svg)

Distributed condition-monitoring edge system. A Zephyr RTOS firmware node runs a
fault-classification FSM, reads a custom devicetree sensor driver, and streams
CRC-checked telemetry over CAN to a multithreaded C++17 Linux gateway that
classifies machine health and writes it to a Linux kernel char device.
Simulation-first: QEMU / native_sim / SocketCAN vcan0.

## Architecture
Zephyr firmware (C++, FSM, custom driver) --CAN--> C++ gateway (threads, CRC,
classification) --> /dev/telemetry0 (Linux char driver)

## Build & run (gateway)
    cd gateway && cmake -S . -B build && cmake --build build
    ctest --test-dir build --output-on-failure
    sudo ./build/gateway vcan0      # + sudo ./build/injector vcan0

## Build & run (firmware)
    cd $ZEPHYR_BASE && west build -p always -b native_sim ~/pulselink/firmware
    sudo ./build/zephyr/zephyr.exe --can-if=vcan0

## Tech
C++17, Zephyr RTOS, CAN (SocketCAN), Linux kernel driver, threads,
Catch2, Ztest, clang-tidy, CMake, GitHub Actions CI/CD.
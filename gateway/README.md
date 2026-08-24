# PulseLink

Distributed condition-monitoring edge system. A Zephyr RTOS firmware node
streams CRC-checked machine-health telemetry over CAN to a multithreaded
C++17 Linux gateway. Simulation-first (QEMU / native_sim / SocketCAN vcan0).

## Status
- [x] Gateway CAN protocol + CRC (unit-tested)
- [ ] Gateway CAN reader (threads)
- [ ] Zephyr firmware node
- [ ] CI/CD pipeline

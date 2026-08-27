#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include "control_fsm.hpp"
#include "sim_sensor.h"

LOG_MODULE_REGISTER(pulselink, LOG_LEVEL_INF);

static uint16_t crc16(const uint8_t* d, size_t n) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < n; ++i) {
        crc ^= static_cast<uint16_t>(d[i]) << 8;
        for (int b = 0; b < 8; ++b)
            crc = (crc & 0x8000) ? static_cast<uint16_t>((crc << 1) ^ 0x1021)
                                 : static_cast<uint16_t>(crc << 1);
    }
    return crc;
}
static void pack(uint8_t o[8], uint8_t node, int16_t t, uint16_t v, uint8_t h) {
    o[0]=node; o[1]=t>>8; o[2]=t&0xFF; o[3]=v>>8; o[4]=v&0xFF; o[5]=h;
    const uint16_t c=crc16(o,6); o[6]=c>>8; o[7]=c&0xFF;
}
static void rx_cb(const struct device*, struct can_frame* f, void*) {
    LOG_INF("CAN RX loopback: node=%u health=%u", f->data[0], f->data[5]);
}

int main(void) {
    const struct device* can = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
    const struct device* sensor = DEVICE_DT_GET(DT_NODELABEL(simsensor0));
    if (!device_is_ready(can))    { LOG_ERR("CAN not ready");    return 0; }
    if (!device_is_ready(sensor)) { LOG_ERR("sensor not ready"); return 0; }
    can_start(can);

    struct can_filter filter = { .id = 0x123, .mask = CAN_STD_ID_MASK };
    can_add_rx_filter(can, rx_cb, nullptr, &filter);

    ControlFsm fsm;
    while (true) {
        int16_t temp = 0;
        sim_sensor_sample(sensor, &temp);              /* <-- vem do driver */
        const uint16_t vib = static_cast<uint16_t>(temp / 4);
        const Health h = fsm.update(temp, vib);

        uint8_t buf[8];
        pack(buf, 1, temp, vib, static_cast<uint8_t>(h));
        struct can_frame frame{};
        frame.id = 0x123; frame.dlc = 8;
        memcpy(frame.data, buf, 8);
        can_send(can, &frame, K_FOREVER, nullptr, nullptr);

        LOG_INF("CAN TX: temp=%d.%02dC vib=%u health=%d",
                temp/100, (temp%100+100)%100, vib, static_cast<int>(h));
        k_sleep(K_MSEC(500));
    }
    return 0;
}
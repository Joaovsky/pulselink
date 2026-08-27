#define DT_DRV_COMPAT pulselink_simsensor
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include "sim_sensor.h"
LOG_MODULE_REGISTER(simsensor, LOG_LEVEL_INF);

// Estado do driver: apenas um valor de temperatura simulado.
struct simsensor_data { int16_t temp; };

// Implementação de `sample`: atualiza o valor simulado e retorna-o via *out.
// Incremento fixo (137 representa 1.37°C) e wrap-around para manter faixa.
static int simsensor_sample(const struct device *dev, int16_t *out) {
    struct simsensor_data *d = (struct simsensor_data *)dev->data;
    d->temp = (int16_t)(d->temp + 137);      /* passo distintivo: 1.37C */
    if (d->temp > 9000) d->temp = 2000;
    *out = d->temp;
    return 0; // 0 = sucesso
}

// Tabela de funções exposta pelo driver (ligada a `dev->api`).
static const struct simsensor_api api = { .sample = simsensor_sample };

// Inicialização do dispositivo: configura estado inicial e registra mensagem.
static int simsensor_init(const struct device *dev) {
    ((struct simsensor_data *)dev->data)->temp = 2000;
    LOG_INF("=== SIMSENSOR DRIVER ONLINE (custom devicetree driver) ===");
    return 0;
}

// Macro que instancia o dispositivo para cada nó do devicetree com status "okay".
#define SIMSENSOR_INIT(inst)                                          \
    static struct simsensor_data simsensor_data_##inst;               \
    DEVICE_DT_INST_DEFINE(inst, simsensor_init, NULL,                 \
        &simsensor_data_##inst, NULL, POST_KERNEL,                    \
        CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &api);

DT_INST_FOREACH_STATUS_OKAY(SIMSENSOR_INIT)
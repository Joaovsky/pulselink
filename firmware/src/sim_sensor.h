#pragma once
#include <zephyr/device.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
// API (tabela de funções) que um driver de sensor simulado deve expor.
// Contém apenas a operação necessária: obter uma amostra.
struct simsensor_api {
    // Preenche *out com a amostra; retorna 0 em sucesso, <0 em erro.
    int (*sample)(const struct device *dev, int16_t *out);
};

// Helper inline: obtém a tabela `simsensor_api` de `dev->api` e chama `sample`.
// Use esta função para ler uma amostra sem fazer cast manual.
static inline int sim_sensor_sample(const struct device *dev, int16_t *out) {
    const struct simsensor_api *api = (const struct simsensor_api *)dev->api;
    return api->sample(dev, out);
}

#ifdef __cplusplus
}
#endif
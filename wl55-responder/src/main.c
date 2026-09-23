#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <string.h>

LOG_MODULE_REGISTER(wl55_responder, LOG_LEVEL_INF);

static const struct device *radio = DEVICE_DT_GET(DT_ALIAS(lora0));
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static struct lora_modem_config cfg = {
    .frequency = 868000000,
    .bandwidth = BW_250_KHZ,
    .datarate = SF_8,
    .coding_rate = CR_4_5,
    .preamble_len = 12,
    .tx_power = 14,
    .tx = false,
    .iq_inverted = false,
    .public_network = false,
    .packet_crc_disable = false,
};

int main(void)
{
    uint8_t rx[255];
    uint8_t pong[] = {'P', 'O', 'N', 'G'};
    uint32_t ping_count = 0;
    uint32_t pong_count = 0;
    bool led_ok = gpio_is_ready_dt(&led) &&
                  gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) == 0;

    if (!device_is_ready(radio)) {
        LOG_ERR("LoRa device not ready");
        return -ENODEV;
    }

    LOG_INF("WL55 responder: 868 MHz, BW250, SF8, CR4/5, private, CRC on");
    LOG_INF("Waiting for four bytes PING; response PONG after 200 ms");

    while (1) {
        int16_t rssi;
        int8_t snr;
        int ret;

        /* Configure RX again after every transmission or receive error. */
        cfg.tx = false;
        ret = lora_config(radio, &cfg);
        if (ret < 0) {
            LOG_ERR("RX config failed: %d", ret);
            k_sleep(K_SECONDS(1));
            continue;
        }

        ret = lora_recv(radio, rx, sizeof(rx), K_FOREVER, &rssi, &snr);
        if (ret < 0) {
            LOG_WRN("RX failed: %d", ret);
            k_sleep(K_MSEC(20));
            continue;
        }
        if (ret != 4 || memcmp(rx, "PING", 4) != 0) {
            LOG_WRN("Ignored %d-byte packet; expected exactly PING", ret);
            continue;
        }

        ping_count++;
        LOG_INF("PING #%u: RSSI %d dBm, SNR %d dB",
                (unsigned int)ping_count, (int)rssi, (int)snr);
        /* Give ObelICS time to leave TX and arm its receiver. */
        k_sleep(K_MSEC(200));

        cfg.tx = true;
        ret = lora_config(radio, &cfg);
        if (ret < 0) {
            LOG_ERR("TX config failed: %d", ret);
            continue;
        }
        ret = lora_send(radio, pong, sizeof(pong));
        if (ret < 0) {
            LOG_ERR("PONG TX failed: %d", ret);
            continue;
        }
        pong_count++;
        LOG_INF("PONG #%u sent", (unsigned int)pong_count);
        if (led_ok) {
            gpio_pin_toggle_dt(&led);
        }
    }
    return 0;
}

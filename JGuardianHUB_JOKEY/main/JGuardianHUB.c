/*
 * JGuardianHUB.c
 *
 *  Created on: Oct 21, 2024
 *      Author: biglap
 */

#include "JGuardianHUB.h"
#include "esp_https_ota.h"


httpd_handle_t server;

extern uint16_t array_modbus[128];

char tmp_macstr[20];
char payload_char[2000];

#define GPIO_INPUT_IO_0 21
#define GPIO_INPUT_PIN_SEL (1ULL << GPIO_INPUT_IO_0)
#define ESP_INTR_FLAG_DEFAULT 0

#define DOOR_OPEN 1
#define DOOR_CLOSE 0

int rele0_status = DOOR_CLOSE;
int rele1_status = DOOR_CLOSE;
int rele2_status = DOOR_CLOSE;

bool toggle = true;


//********************************************************************************//
//**************************** OTA UPGRADE **************************************//
//********************************************************************************//

esp_err_t ota_http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

void check_ota_upgrade()
{
	esp_http_client_config_t config = {
			.url = CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL,
			.cert_pem = NULL,//(char *)server_cert_pem_start,
			.event_handler = ota_http_event_handler,
			.keep_alive_enable = true
	};

	esp_https_ota_config_t ota_config = {
			.http_config = &config,
	};

	esp_err_t ret = esp_https_ota(&ota_config);
	if (ret == ESP_OK) {
		esp_restart();
	} else {
		ESP_LOGE(TAG, "Firmware upgrade failed");
	}
}

//********************************************************************************//
//********************************************************************************//
//********************************************************************************//

void ctrl_tsk(void) {

  get_mac_str(tmp_macstr);

  while (1) {

    if (toggle) {
      gpio_set_level(GPIO_OUTPUT_IO_LED, true);
      toggle = false;
    } else {
      gpio_set_level(GPIO_OUTPUT_IO_LED, false);
      toggle = true;
    }

    if (server == NULL) {
      vTaskDelay(1 * 200 / portTICK_PERIOD_MS);
    } else {
      vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
    }

  } // task main cycle: end

  vTaskDelete(NULL);
}

void app_main(void) {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  //*********************************************//

  //**********************************************************//

  // zero-initialize the config structure.
  gpio_config_t io_conf_led = {};
  io_conf_led.intr_type = GPIO_INTR_DISABLE;
  io_conf_led.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf_led.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_LED;
  io_conf_led.pull_down_en = 0;
  io_conf_led.pull_up_en = 0;
  gpio_config(&io_conf_led);

  // zero-initialize the config structure.
  gpio_config_t io_conf_0 = {};
  io_conf_0.intr_type = GPIO_INTR_DISABLE;
  io_conf_0.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf_0.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_0;
  io_conf_0.pull_down_en = 0;
  io_conf_0.pull_up_en = 0;
  gpio_config(&io_conf_0);

  // zero-initialize the config structure.
  gpio_config_t io_conf_1 = {};
  io_conf_1.intr_type = GPIO_INTR_DISABLE;
  io_conf_1.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf_1.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_1;
  io_conf_1.pull_down_en = 0;
  io_conf_1.pull_up_en = 0;
  gpio_config(&io_conf_1);

  // zero-initialize the config structure.
  gpio_config_t io_conf_2 = {};
  io_conf_2.intr_type = GPIO_INTR_DISABLE;
  io_conf_2.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf_2.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_2;
  io_conf_2.pull_down_en = 0;
  io_conf_2.pull_up_en = 0;
  gpio_config(&io_conf_2);

  // zero-initialize the config structure.
  gpio_config_t io_conf_3 = {};
  io_conf_3.intr_type = GPIO_INTR_DISABLE;
  io_conf_3.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf_3.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_3;
  io_conf_3.pull_down_en = 0;
  io_conf_3.pull_up_en = 0;
  gpio_config(&io_conf_3);

  // zero-initialize the config structure.
  gpio_config_t io_conf_4 = {};
  io_conf_4.intr_type = GPIO_INTR_DISABLE;
  io_conf_4.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf_4.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_4;
  io_conf_4.pull_down_en = 0;
  io_conf_4.pull_up_en = 0;
  gpio_config(&io_conf_4);

  // zero-initialize the config structure.
  gpio_config_t io_conf_5 = {};
  io_conf_5.intr_type = GPIO_INTR_DISABLE;
  io_conf_5.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf_5.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_5;
  io_conf_5.pull_down_en = 0;
  io_conf_5.pull_up_en = 0;
  gpio_config(&io_conf_5);

  gpio_set_level(GPIO_OUTPUT_IO_1, false);
  gpio_set_level(GPIO_OUTPUT_IO_2, false);
  gpio_set_level(GPIO_OUTPUT_IO_3, false);
  gpio_set_level(GPIO_OUTPUT_IO_4, false);
  gpio_set_level(GPIO_OUTPUT_IO_5, false);
  gpio_set_level(GPIO_OUTPUT_IO_6, false);
  gpio_set_level(GPIO_OUTPUT_IO_LED, true);

  //*********************************************************/

  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  wifi_init_sta();

  xTaskCreatePinnedToCore((TaskFunction_t)ctrl_tsk, "ctrl_tsk", 1024 * 5, NULL,
                          4, NULL, 1 /*tskNO_AFFINITY*/);
}

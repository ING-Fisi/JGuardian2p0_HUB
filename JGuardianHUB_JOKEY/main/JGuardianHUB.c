/*
 * JGuardianHUB.c
 *
 *  Created on: Oct 21, 2024
 *      Author: biglap
 */

#include "JGuardianHUB.h"
#include "driver/i2c_master.h"
#include "esp_https_ota.h"


httpd_handle_t server;
esp_io_expander_handle_t io_expander = NULL;
led_strip_handle_t led_strip = NULL;

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


void start_eth_connection(void);

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
			/* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each
			 * color */
			for (int i = 0; i < LED_STRIP_LED_COUNT; i++) {
				ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 5, 5, 5));
			}
			/* Refresh the strip to send data */
			ESP_ERROR_CHECK(led_strip_refresh(led_strip));
			toggle = false;
		} else {
			/* Set all LED off to clear all pixels */
			ESP_ERROR_CHECK(led_strip_clear(led_strip));
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
  led_strip = configure_led();

	i2c_master_bus_handle_t i2c_handle = NULL;
	const i2c_master_bus_config_t bus_config = {
		.i2c_port = I2C_NUM_0,
		.sda_io_num = 42,
		.scl_io_num = 41,
		.clk_source = I2C_CLK_SRC_DEFAULT,
	};
	i2c_new_master_bus(&bus_config, &i2c_handle);

	esp_io_expander_new_i2c_tca9554(
		i2c_handle, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, &io_expander);

	esp_io_expander_print_state(io_expander);

	//**********************************************************//

	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_0,
							IO_EXPANDER_OUTPUT);
	esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_0, 0);

	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_1,
							IO_EXPANDER_OUTPUT);
	esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_1, 0);

	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_2,
							IO_EXPANDER_OUTPUT);
	esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_2, 0);

	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_3,
							IO_EXPANDER_OUTPUT);
	esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_3, 0);

	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_4,
							IO_EXPANDER_OUTPUT);
	esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_4, 0);

	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_5,
							IO_EXPANDER_OUTPUT);
	esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_5, 0);

	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_6,
							IO_EXPANDER_OUTPUT);
	esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_6, 0);

	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_7,
							IO_EXPANDER_OUTPUT);
	esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_7, 0);



//  // zero-initialize the config structure.
//  gpio_config_t io_conf_led = {};
//  io_conf_led.intr_type = GPIO_INTR_DISABLE;
//  io_conf_led.mode = GPIO_MODE_INPUT_OUTPUT;
//  io_conf_led.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_LED;
//  io_conf_led.pull_down_en = 0;
//  io_conf_led.pull_up_en = 0;
//  gpio_config(&io_conf_led);
//
//  // zero-initialize the config structure.
//  gpio_config_t io_conf_0 = {};
//  io_conf_0.intr_type = GPIO_INTR_DISABLE;
//  io_conf_0.mode = GPIO_MODE_INPUT_OUTPUT;
//  io_conf_0.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_0;
//  io_conf_0.pull_down_en = 0;
//  io_conf_0.pull_up_en = 0;
//  gpio_config(&io_conf_0);
//
//  // zero-initialize the config structure.
//  gpio_config_t io_conf_1 = {};
//  io_conf_1.intr_type = GPIO_INTR_DISABLE;
//  io_conf_1.mode = GPIO_MODE_INPUT_OUTPUT;
//  io_conf_1.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_1;
//  io_conf_1.pull_down_en = 0;
//  io_conf_1.pull_up_en = 0;
//  gpio_config(&io_conf_1);
//
//  // zero-initialize the config structure.
//  gpio_config_t io_conf_2 = {};
//  io_conf_2.intr_type = GPIO_INTR_DISABLE;
//  io_conf_2.mode = GPIO_MODE_INPUT_OUTPUT;
//  io_conf_2.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_2;
//  io_conf_2.pull_down_en = 0;
//  io_conf_2.pull_up_en = 0;
//  gpio_config(&io_conf_2);
//
//  // zero-initialize the config structure.
//  gpio_config_t io_conf_3 = {};
//  io_conf_3.intr_type = GPIO_INTR_DISABLE;
//  io_conf_3.mode = GPIO_MODE_INPUT_OUTPUT;
//  io_conf_3.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_3;
//  io_conf_3.pull_down_en = 0;
//  io_conf_3.pull_up_en = 0;
//  gpio_config(&io_conf_3);
//
//  // zero-initialize the config structure.
//  gpio_config_t io_conf_4 = {};
//  io_conf_4.intr_type = GPIO_INTR_DISABLE;
//  io_conf_4.mode = GPIO_MODE_INPUT_OUTPUT;
//  io_conf_4.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_4;
//  io_conf_4.pull_down_en = 0;
//  io_conf_4.pull_up_en = 0;
//  gpio_config(&io_conf_4);
//
//  // zero-initialize the config structure.
//  gpio_config_t io_conf_5 = {};
//  io_conf_5.intr_type = GPIO_INTR_DISABLE;
//  io_conf_5.mode = GPIO_MODE_INPUT_OUTPUT;
//  io_conf_5.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_5;
//  io_conf_5.pull_down_en = 0;
//  io_conf_5.pull_up_en = 0;
//  gpio_config(&io_conf_5);
//
//  gpio_set_level(GPIO_OUTPUT_IO_1, false);
//  gpio_set_level(GPIO_OUTPUT_IO_2, false);
//  gpio_set_level(GPIO_OUTPUT_IO_3, false);
//  gpio_set_level(GPIO_OUTPUT_IO_4, false);
//  gpio_set_level(GPIO_OUTPUT_IO_5, false);
//  gpio_set_level(GPIO_OUTPUT_IO_6, false);
//  gpio_set_level(GPIO_OUTPUT_IO_LED, true);

	//*********************************************************/

	// ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
	// wifi_init_sta();
	start_eth_connection();

  xTaskCreatePinnedToCore((TaskFunction_t)ctrl_tsk, "ctrl_tsk", 1024 * 5, NULL,
                          4, NULL, 1 /*tskNO_AFFINITY*/);
}

//***********************************************************************************************************///
//***********************************************************************************************************///
//***********************************************************************************************************///
//***********************************************************************************************************///
#include "ethernet_init.h"

#define EXAMPLE_STATIC_IP_ADDR "10.100.0.88"
#define EXAMPLE_STATIC_NETMASK_ADDR "255.255.255.0"
#define EXAMPLE_STATIC_GW_ADDR "10.100.0.1"
#define EXAMPLE_MAIN_DNS_SERVER EXAMPLE_STATIC_GW_ADDR
#define EXAMPLE_BACKUP_DNS_SERVER "0.0.0.0"


static esp_err_t example_set_dns_server(esp_netif_t *netif, uint32_t addr,
										esp_netif_dns_type_t type) {
	if (addr && (addr != IPADDR_NONE)) {
		esp_netif_dns_info_t dns;
		dns.ip.u_addr.ip4.addr = addr;
		dns.ip.type = IPADDR_TYPE_V4;
		ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, type, &dns));
	}
	return ESP_OK;
}

static void example_set_static_ip(esp_netif_t *netif) {
	if (esp_netif_dhcpc_stop(netif) != ESP_OK) {
		ESP_LOGE(TAG, "Failed to stop dhcp client");
		return;
	}
	esp_netif_ip_info_t ip;
	memset(&ip, 0, sizeof(esp_netif_ip_info_t));
	ip.ip.addr = ipaddr_addr(EXAMPLE_STATIC_IP_ADDR);
	ip.netmask.addr = ipaddr_addr(EXAMPLE_STATIC_NETMASK_ADDR);
	ip.gw.addr = ipaddr_addr(EXAMPLE_STATIC_GW_ADDR);
	if (esp_netif_set_ip_info(netif, &ip) != ESP_OK) {
		ESP_LOGE(TAG, "Failed to set ip info");
		return;
	}
	ESP_LOGD(TAG, "Success to set static ip: %s, netmask: %s, gw: %s",
			 EXAMPLE_STATIC_IP_ADDR, EXAMPLE_STATIC_NETMASK_ADDR,
			 EXAMPLE_STATIC_GW_ADDR);
	ESP_ERROR_CHECK(example_set_dns_server(
		netif, ipaddr_addr(EXAMPLE_MAIN_DNS_SERVER), ESP_NETIF_DNS_MAIN));
	ESP_ERROR_CHECK(example_set_dns_server(
		netif, ipaddr_addr(EXAMPLE_BACKUP_DNS_SERVER), ESP_NETIF_DNS_BACKUP));
}

#define CONFIG_EXAMPLE_ETH_DEINIT_AFTER_S -1

static void eth_event_handler(void *arg, esp_event_base_t event_base,
							  int32_t event_id, void *event_data) {
	uint8_t mac_addr[6] = {0};
	/* we can get the ethernet driver handle from event data */
	esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

	switch (event_id) {
	case ETHERNET_EVENT_CONNECTED:
		esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
		ESP_LOGI(TAG, "Ethernet Link Up");
		ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
				 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
				 mac_addr[4], mac_addr[5]);
		break;
	case ETHERNET_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "Ethernet Link Down");
		break;
	case ETHERNET_EVENT_START:
		ESP_LOGI(TAG, "Ethernet Started");
		break;
	case ETHERNET_EVENT_STOP:
		ESP_LOGI(TAG, "Ethernet Stopped");
		break;
	default:
		break;
	}
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
								 int32_t event_id, void *event_data) {
	ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
	const esp_netif_ip_info_t *ip_info = &event->ip_info;

	ESP_LOGI(TAG, "Ethernet Got IP Address");
	ESP_LOGI(TAG, "~~~~~~~~~~~");
	ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
	ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
	ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
	ESP_LOGI(TAG, "~~~~~~~~~~~");

	server = start_JGuardian_SERVER();
}

void start_eth_connection(void) {
	// Initialize Ethernet driver
	uint8_t eth_port_cnt = 0;
	esp_eth_handle_t *eth_handles;
	ESP_ERROR_CHECK(example_eth_init(&eth_handles, &eth_port_cnt));

	// Initialize TCP/IP network interface aka the esp-netif (should be called
	// only once in application)
	ESP_ERROR_CHECK(esp_netif_init());
	// Create default event loop that running in background
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_t *eth_netifs[eth_port_cnt];
	esp_eth_netif_glue_handle_t eth_netif_glues[eth_port_cnt];

	// Create instance(s) of esp-netif for Ethernet(s)
	if (eth_port_cnt == 1) {
		// Use ESP_NETIF_DEFAULT_ETH when just one Ethernet interface is used
		// and you don't need to modify default esp-netif configuration
		// parameters.
		esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
		eth_netifs[0] = esp_netif_new(&cfg);
		eth_netif_glues[0] = esp_eth_new_netif_glue(eth_handles[0]);
		// Attach Ethernet driver to TCP/IP stack
		ESP_ERROR_CHECK(esp_netif_attach(eth_netifs[0], eth_netif_glues[0]));
	
		example_set_static_ip(eth_netifs[0]);

	} else {
		// Use ESP_NETIF_INHERENT_DEFAULT_ETH when multiple Ethernet interfaces
		// are used and so you need to modify esp-netif configuration parameters
		// for each interface (name, priority, etc.).
		esp_netif_inherent_config_t esp_netif_config =
			ESP_NETIF_INHERENT_DEFAULT_ETH();
		esp_netif_config_t cfg_spi = {.base = &esp_netif_config,
									  .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH};
		char if_key_str[10];
		char if_desc_str[10];
		char num_str[3];
		for (int i = 0; i < eth_port_cnt; i++) {
			itoa(i, num_str, 10);
			strcat(strcpy(if_key_str, "ETH_"), num_str);
			strcat(strcpy(if_desc_str, "eth"), num_str);
			esp_netif_config.if_key = if_key_str;
			esp_netif_config.if_desc = if_desc_str;
			esp_netif_config.route_prio -= i * 5;
			eth_netifs[i] = esp_netif_new(&cfg_spi);
			eth_netif_glues[i] = esp_eth_new_netif_glue(eth_handles[i]);
			// Attach Ethernet driver to TCP/IP stack
			ESP_ERROR_CHECK(
				esp_netif_attach(eth_netifs[i], eth_netif_glues[i]));
		}
	}

	// Register user defined event handers
	ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID,
											   &eth_event_handler, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP,
											   &got_ip_event_handler, NULL));

	// Start Ethernet driver state machine
	for (int i = 0; i < eth_port_cnt; i++) {
		ESP_ERROR_CHECK(esp_eth_start(eth_handles[i]));
	}

#if CONFIG_EXAMPLE_ETH_DEINIT_AFTER_S >= 0
	// For demonstration purposes, wait and then deinit Ethernet network
	vTaskDelay(pdMS_TO_TICKS(CONFIG_EXAMPLE_ETH_DEINIT_AFTER_S * 1000));
	ESP_LOGI(TAG, "stop and deinitialize Ethernet network...");
	// Stop Ethernet driver state machine and destroy netif
	for (int i = 0; i < eth_port_cnt; i++) {
		ESP_ERROR_CHECK(esp_eth_stop(eth_handles[i]));
		ESP_ERROR_CHECK(esp_eth_del_netif_glue(eth_netif_glues[i]));
		esp_netif_destroy(eth_netifs[i]);
	}
	esp_netif_deinit();
	ESP_ERROR_CHECK(example_eth_deinit(eth_handles, eth_port_cnt));
	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP,
												 got_ip_event_handler));
	ESP_ERROR_CHECK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID,
												 eth_event_handler));
	ESP_ERROR_CHECK(esp_event_loop_delete_default());
#endif // EXAMPLE_ETH_DEINIT_AFTER_S > 0
}

//***********************************************************************************************************///
//***********************************************************************************************************///
//***********************************************************************************************************///
//***********************************************************************************************************///
//***********************************************************************************************************///

/*
 * JGuardianHUB.h
 *
 *  Created on: Oct 21, 2024
 *      Author: biglap
 */

#ifndef MAIN_JGUARDIANHUB_H_
#define MAIN_JGUARDIANHUB_H_

#include "string.h"
#include "esp_check.h"
#include "modbus_params.h"  // for modbus parameters structures
#include "mbcontroller.h"
#include "sdkconfig.h"
#include "esp_http_server.h"
#include "utility.h"
#include "esp_mac.h"


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include <netdb.h>
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_timer.h"


//*****************************************//
#define CONFIG_EXAMPLE_WIFI_SSID "FisitronHUB"
#define CONFIG_EXAMPLE_WIFI_PASSWORD "Fisitron319086"
#define CONFIG_EXAMPLE_MAXIMUM_RETRY 30
#define CONFIG_EXAMPLE_STATIC_IP_ADDR "10.100.0.99"
#define CONFIG_EXAMPLE_STATIC_NETMASK_ADDR "255.255.255.0"
#define CONFIG_EXAMPLE_STATIC_GW_ADDR "10.100.0.1"


#define EXAMPLE_WIFI_SSID             CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_WIFI_PASS             CONFIG_EXAMPLE_WIFI_PASSWORD
#define EXAMPLE_MAXIMUM_RETRY         CONFIG_EXAMPLE_MAXIMUM_RETRY
#define EXAMPLE_STATIC_IP_ADDR        CONFIG_EXAMPLE_STATIC_IP_ADDR
#define EXAMPLE_STATIC_NETMASK_ADDR   CONFIG_EXAMPLE_STATIC_NETMASK_ADDR
#define EXAMPLE_STATIC_GW_ADDR        CONFIG_EXAMPLE_STATIC_GW_ADDR
//#ifdef CONFIG_EXAMPLE_STATIC_DNS_AUTO
#define EXAMPLE_MAIN_DNS_SERVER       EXAMPLE_STATIC_GW_ADDR
#define EXAMPLE_BACKUP_DNS_SERVER     "0.0.0.0"

//*****************************************//

static const char *TAG = "JGUARDIAN_HUB_SPRINKLER";


//************************* RELE ******************************//
#define GPIO_OUTPUT_IO_LED    21
#define GPIO_OUTPUT_PIN_SEL_LED  (1ULL<<GPIO_OUTPUT_IO_LED)

#define GPIO_OUTPUT_IO_1    32
#define GPIO_OUTPUT_PIN_SEL_0  (1ULL<<GPIO_OUTPUT_IO_1)

#define GPIO_OUTPUT_IO_2    33
#define GPIO_OUTPUT_PIN_SEL_1  (1ULL<<GPIO_OUTPUT_IO_2)

#define GPIO_OUTPUT_IO_3    25
#define GPIO_OUTPUT_PIN_SEL_2  (1ULL<<GPIO_OUTPUT_IO_3)


//************************* INGRESSI ******************************//
#define GPIO_INPUT_IO_IN1   19
#define GPIO_INPUT_PIN_SEL_IN1  (1ULL<<GPIO_INPUT_IO_IN1)

#define GPIO_INPUT_IO_IN2   5
#define GPIO_INPUT_PIN_SEL_IN2  (1ULL<<GPIO_INPUT_IO_IN2)

#define GPIO_INPUT_IO_IN3   16
#define GPIO_INPUT_PIN_SEL_IN3  (1ULL<<GPIO_INPUT_IO_IN3)

#define GPIO_INPUT_IO_IN4   0
#define GPIO_INPUT_PIN_SEL_IN4  (1ULL<<GPIO_INPUT_IO_IN4)





httpd_handle_t start_webserver(void);


void wifi_init_sta(void);

#endif /* MAIN_JGUARDIANHUB_H_ */

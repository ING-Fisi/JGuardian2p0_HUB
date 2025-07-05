/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_check.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>

#if !CONFIG_IDF_TARGET_LINUX
#include "esp_eth.h"
#include "nvs_flash.h"
#include <esp_system.h>
#include <esp_wifi.h>
#endif // !CONFIG_IDF_TARGET_LINUX

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN (64)

#include "JGuardianHUB.h"

httpd_handle_t start_webserver(void);

//*************************************//

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

/* An HTTP GET handler */
static esp_err_t request_get_modbus(httpd_req_t *req) {
  char *buf;
  size_t buf_len;

  char resp_str[100]; // = (const char*) req->user_ctx;
  memset(resp_str, 0, sizeof(resp_str));

  if (request_modbus_info(resp_str) == 0) {
    sprintf(resp_str, "MODBUS ERROR");
  }
  /* Send response with custom headers and body set as the
   * string passed in user context*/
  httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

  /* After sending the HTTP response the old HTTP request
   * headers are lost. Check if HTTP request headers can be read now. */
  //    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
  //        ESP_LOGI(TAG, "Request headers lost");
  //    }
  return ESP_OK;
}

static const httpd_uri_t get_modbus = {.uri = "/get_modbus",
                                       .method = HTTP_GET,
                                       .handler = request_get_modbus,
                                       /* Let's pass response string in user
                                        * context to demonstrate it's usage */
                                       .user_ctx = "request_get_modbus!"};

/* An HTTP POST handler */
static esp_err_t set_rele_post_handler(httpd_req_t *req) {
  char buf[100];
  int ret, remaining = req->content_len;

  while (remaining > 0) {
    /* Read the data for the request */
    if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
      if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
        /* Retry receiving if timeout occurred */
        continue;
      }
      return ESP_FAIL;
    }

    /* Send back the same data */
    httpd_resp_send_chunk(req, buf, ret);
    remaining -= ret;
  }

  int rele_id = 0;
  int rele_status = 0;

  sscanf(buf, "RELE[%d][%d]", &rele_id, &rele_status);

  if ((rele_id == 1) && (rele_status == 1)) {
    ESP_LOGI(TAG, "RELE1_ON");
    gpio_set_level(GPIO_OUTPUT_IO_1, true);
  }
  if ((rele_id == 1) && (rele_status == 0)) {
    ESP_LOGI(TAG, "RELE1_OFF");
    gpio_set_level(GPIO_OUTPUT_IO_1, false);
  }
  if ((rele_id == 2) && (rele_status == 1)) {
    ESP_LOGI(TAG, "RELE2_ON");
    gpio_set_level(GPIO_OUTPUT_IO_2, true);
  }
  if ((rele_id == 2) && (rele_status == 0)) {
    ESP_LOGI(TAG, "RELE2_OFF");
    gpio_set_level(GPIO_OUTPUT_IO_2, false);
  }
  if ((rele_id == 3) && (rele_status == 1)) {
    ESP_LOGI(TAG, "RELE3_ON");
    gpio_set_level(GPIO_OUTPUT_IO_3, true);
  }
  if ((rele_id == 3) && (rele_status == 0)) {
    ESP_LOGI(TAG, "RELE3_OFF");
    gpio_set_level(GPIO_OUTPUT_IO_3, false);
  }
  //	if((rele_id == 4)&&(rele_status == 1))
  //    {
  //		ESP_LOGI(TAG, "RELE4_ON");
  //		gpio_set_level(GPIO_OUTPUT_IO_4, true);
  //	}
  //	 if((rele_id == 4)&&(rele_status == 0))
  //    {
  //		ESP_LOGI(TAG, "RELE4_OFF");
  //		gpio_set_level(GPIO_OUTPUT_IO_4, false);
  //	}

  // End response
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static const httpd_uri_t set_rele = {.uri = "/set_rele",
                                     .method = HTTP_POST,
                                     .handler = set_rele_post_handler,
                                     .user_ctx = NULL};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err) {
  if (strcmp("/hello", req->uri) == 0) {
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND,
                        "/hello URI is not available");
    /* Return ESP_OK to keep underlying socket open */
    return ESP_OK;
  } else if (strcmp("/echo", req->uri) == 0) {
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
    /* Return ESP_FAIL to close underlying socket */
    return ESP_FAIL;
  }
  /* For any other URI send 404 and close socket */
  httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
  return ESP_FAIL;
}

static esp_err_t stop_webserver(httpd_handle_t server) {
  // Stop the httpd server
  return httpd_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server) {
    ESP_LOGI(TAG, "Stopping webserver");
    if (stop_webserver(*server) == ESP_OK) {
      *server = NULL;
      esp_restart();
    } else {
      ESP_LOGE(TAG, "Failed to stop http server");
    }
  }
}

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data) {
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server == NULL) {
    ESP_LOGI(TAG, "Starting webserver");
    *server = start_webserver();
  }
}

httpd_handle_t start_JGuardian_SERVER() {
  static httpd_handle_t server = NULL;

  // ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
  // &connect_handler, &server));
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

  server = start_webserver();

  return server;
}

httpd_handle_t start_webserver(void) {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
#if CONFIG_IDF_TARGET_LINUX
  // Setting port as 8001 when building for Linux. Port 80 can be used only by a
  // privileged user in linux. So when a unprivileged user tries to run the
  // application, it throws bind error and the server is not started. Port 8001
  // can be used by an unprivileged user as well. So the application will not
  // throw bind error and the server will be started.
  config.server_port = 8001;
#endif // !CONFIG_IDF_TARGET_LINUX
  config.lru_purge_enable = true;

  // Start the httpd server
  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK) {
    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &get_modbus);
    httpd_register_uri_handler(server, &set_rele);
// httpd_register_uri_handler(server, &ctrl);
// httpd_register_uri_handler(server, &any);
#if CONFIG_EXAMPLE_BASIC_AUTH
    httpd_register_basic_auth(server);
#endif
    return server;
  }

  ESP_LOGI(TAG, "Error starting server!");
  return NULL;
}

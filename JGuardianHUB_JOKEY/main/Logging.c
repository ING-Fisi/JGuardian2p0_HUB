/*
 * Logging.c
 *
 *  Created on: 5 gen 2026
 *      Author: LENOVO
 */

#include "Logging.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "LOGGING";

void Log2File(char *str) {
	FILE *fp = fopen(LOG_PATH, "w");

	if (fp != NULL) {
		fseek(fp, 0L, SEEK_END);
		// calculating the size of the file
		long int res = ftell(fp);
		ESP_LOGI(TAG, "File dimension %lu", res);

		if (res > 256 * 1024) {
			if (remove(LOG_PATH) == 0) {
				ESP_LOGI(TAG, "File %s successfully deleted.\n", LOG_PATH);
			} else {
				ESP_LOGI(TAG, "Error deleting file");

				perror("Error deleting file");
			}
		} else {
			{
				if (fp != NULL) {
					fprintf(fp, "%s\n", str);
					fflush(fp);
					fclose(fp);
				}
			}
		}
	}
}
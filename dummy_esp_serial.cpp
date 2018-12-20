/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include<time.h>

void app_main()
{


    printf("Hello world!\n");

    ///* Print chip information */
    //esp_chip_info_t chip_info;
    //esp_chip_info(&chip_info);
    //printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
    //        chip_info.cores,
    //        (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
    //        (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    //printf("silicon revision %d, ", chip_info.revision);

    //printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
    //        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    //for (int i = 10; i >= 0; i--) {
    //}
	
	time_t timer = 0;
	unsigned int id = 0;

	struct tm local_time;
	
	while(1){
		localtime_r(&timer, &local_time);
		printf("%02d:%02d\n", local_time.tm_min,local_time.tm_sec);

		if(timer % 5 == 0) printf("My name is \n");
		if(timer % 15 == 0) printf("Tekitou na Debug bun ga kokoni aru\n");
		if(timer % 3 == 0) printf("Tanoshiina ESP ha iiyatu da\n");

		if(timer % 10 == 0){
			printf("@%02d:%02d,%02d\n",local_time.tm_min,local_time.tm_sec,id);
			id += 1;
		}
		timer += 1;
        vTaskDelay(1000 / portTICK_PERIOD_MS);	//1 / portTICKにすると， = 0になってしまって大変なことになるのかも？
	}

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

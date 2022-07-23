/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <string.h>

#define PASSWD_SIZE 4

char passwd[PASSWD_SIZE];
int passwd_idx = 0;

static QueueHandle_t idx_queue = NULL;
static QueueHandle_t character_queue = NULL;

static TaskHandle_t task_readkeyboard_handle = NULL;
static TaskHandle_t task_printpassword_handle = NULL;

gpio_num_t row_pins[4] = {GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_27, GPIO_NUM_26};
// gpio_num_t row_pins[4] = {GPIO_NUM_25, GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_35};

char keys[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'},
};

void taskPrintPassword(void *args)
{
	char received;

	while(1) 
	{
		if (xQueueReceive(character_queue, &received, portMAX_DELAY))
		{
			if (received == '#')
			{
				printf("Senha: %s\n", passwd);
				passwd_idx = 0;
				memset(passwd, 0, PASSWD_SIZE);
			}
			else if (received == '0')
			{
				passwd_idx = 0;
			}
			else
			{
				// printf("%c\n", received);
				passwd[passwd_idx] = received;
				passwd_idx++;
				// printf();
			}
		}
	}
}

void taskReadKeyboardMatrix(void *args)
{
	while(1) 
	{
		for (int i = 0; i < 4; i++)
		{
			gpio_set_level(row_pins[0], 0);
			gpio_set_level(row_pins[1], 0);
			gpio_set_level(row_pins[2], 0);
			gpio_set_level(row_pins[3], 0);
			gpio_set_level(row_pins[i], 1);

			if (gpio_get_level(GPIO_NUM_25))
			{
				xQueueSend(character_queue, &keys[i][0], 0);
				// printf("%c\n", keys[i][0]);
				while (gpio_get_level(GPIO_NUM_25)) {};
			}
			else if (gpio_get_level(GPIO_NUM_33))
			{
				xQueueSend(character_queue, &keys[i][1], 0);
				// printf("%c\n", keys[i][1]);
				while (gpio_get_level(GPIO_NUM_33)) {};
			}
			else if (gpio_get_level(GPIO_NUM_32))
			{
				xQueueSend(character_queue, &keys[i][2], 0);
				// printf("%c\n", keys[i][2]);
				while (gpio_get_level(GPIO_NUM_32)) {};
			}
			else if (gpio_get_level(GPIO_NUM_35))
			{
				xQueueSend(character_queue, &keys[i][3], 0);
				// printf("%c\n", keys[i][3]);
				while (gpio_get_level(GPIO_NUM_35)) {};
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

#endif

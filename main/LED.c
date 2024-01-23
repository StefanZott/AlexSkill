#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "esp_log.h"

#include"LED.h"
#include "main.h"
#include "WLAN.h"

static const char* TAG = "LED";
static int count;

QueueHandle_t xLedStateQueue;
ledStateMessage message;
ledStateMessage tempMessage;

ledProfil redLedProfil;
ledProfil greenLedProfil;
ledProfil yellowLedProfil;
ledProfil blueLedProfil;

ledProfil tempRedLedProfil;
ledProfil tempGreenLedProfil;
ledProfil tempYellowLedProfil;
ledProfil tempblueLedProfil;

static void ledInit(void) {
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledRedTimer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledRedTimer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledRedChannel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_RED,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED_GPIO_RED,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledRedChannel));
    strcpy(redLedProfil.state, OFF);
    strcpy(redLedProfil.mode, NORMAL);


    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledGreenTimer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledGreenTimer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledGreenChannel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_GREEN,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED_GPIO_GREEN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledGreenChannel));
    strcpy(greenLedProfil.state, OFF);
    strcpy(greenLedProfil.mode, NORMAL);


    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledYellowTimer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledYellowTimer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledYellowChannel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_YELLOW,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED_GPIO_YELLOW,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledYellowChannel));
    strcpy(yellowLedProfil.state, OFF);
    strcpy(yellowLedProfil.mode, NORMAL);


    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledBlueTimer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledBlueTimer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledBlueChannel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_BLUE,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED_GPIO_BLUE,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledBlueChannel));
    strcpy(blueLedProfil.state, OFF);
    strcpy(blueLedProfil.mode, NORMAL);
}

static void saveSettings() {
    ESP_LOGI(TAG, "Save Settings tempMessage -> LED = %s, State = %s, Mode = %s", tempMessage.led, tempMessage.state, tempMessage.mode);
    ESP_LOGI(TAG, "Save Settings message -> LED = %s, State = %s, Mode = %s", message.led, message.state, message.mode);
    if (strcmp(tempMessage.mode, "test") != 0) {
        strcpy(message.led, tempMessage.led);
        strcpy(message.state, tempMessage.state);
        strcpy(message.mode, tempMessage.mode);

        strcpy(redLedProfil.state, tempRedLedProfil.state);
        strcpy(redLedProfil.mode, tempRedLedProfil.mode);

        strcpy(greenLedProfil.state, tempGreenLedProfil.state);
        strcpy(greenLedProfil.mode, tempGreenLedProfil.mode);

        strcpy(yellowLedProfil.state, tempYellowLedProfil.state);
        strcpy(yellowLedProfil.mode, tempYellowLedProfil.mode);

        strcpy(blueLedProfil.state, tempblueLedProfil.state);
        strcpy(blueLedProfil.mode, tempblueLedProfil.mode);
    }
}

static void loadSettings() { 
    ESP_LOGI(TAG, "Load Settings message -> LED = %s, State = %s, Mode = %s", message.led, message.state, message.mode);
    ESP_LOGI(TAG, "Load Settings tempMessage -> LED = %s, State = %s, Mode = %s", tempMessage.led, tempMessage.state, tempMessage.mode);
    strcpy(tempMessage.led, message.led);
    strcpy(tempMessage.state, message.state);
    strcpy(tempMessage.mode, message.mode);

    strcpy(tempRedLedProfil.state, redLedProfil.state);
    strcpy(tempRedLedProfil.mode, redLedProfil.mode);

    strcpy(tempGreenLedProfil.state, greenLedProfil.state);
    strcpy(tempGreenLedProfil.mode, greenLedProfil.mode);

    strcpy(tempYellowLedProfil.state, yellowLedProfil.state);
    strcpy(tempYellowLedProfil.mode, yellowLedProfil.mode);

    strcpy(tempblueLedProfil.state, blueLedProfil.state);
    strcpy(tempblueLedProfil.mode, blueLedProfil.mode);
}

static void blink() {
    if(ledc_get_duty(LEDC_MODE, LEDC_CHANNEL_BLUE) > 0) {
        ESP_LOGI(TAG, "Blue LED OFF");
        // Set duty to 50%
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_OFF));
        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
    } else {
        ESP_LOGI(TAG, "Blue LED ON");
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_ON));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
    }
}

void ledControlTask( void *pvParameters ) {
    ESP_LOGI(TAG, "Start LedControlTask");
    
    strcpy(tempMessage.state, BLINK);
    strcpy(tempMessage.mode, NORMAL);
    xLedStateQueue = xQueueCreate( 10, sizeof( ledStateMessage ) );

    // Set the LEDC peripheral configuration
    ledInit();

    while (1) {
        saveSettings();

        if(xQueueReceive( xLedStateQueue, (void *) &tempMessage, BLINK_FREQ / portTICK_PERIOD_MS ) == pdTRUE ) {
            ESP_LOGI(TAG, "New tempMessage info. LED = %s, State = %s, Mode = %s", message.led, message.state, message.mode);

            if (strcmp(tempMessage.led, LED_BLUE) == 0) {
                strcpy(tempblueLedProfil.state, tempMessage.state);
                strcpy(tempblueLedProfil.mode, tempMessage.mode);
            } else if(strcmp(tempMessage.led, LED_GREEN) == 0) {
                strcpy(tempGreenLedProfil.state, tempMessage.state);
                strcpy(tempGreenLedProfil.mode, tempMessage.mode);
            } else if(strcmp(tempMessage.led, LED_RED) == 0) {
                strcpy(tempRedLedProfil.state, tempMessage.state);
                strcpy(tempRedLedProfil.mode, tempMessage.mode);
            } else if(strcmp(tempMessage.led, LED_YELLOW) == 0) {
                strcpy(tempYellowLedProfil.state, tempMessage.state);
                strcpy(tempYellowLedProfil.mode, tempMessage.mode);
            }
        } else {
            // TODO: Code überarbeiten. Jede LED ihren Zustand einzeln speichern.
            if (strcmp(tempMessage.mode, "normal") == 0) {
                if (strcmp(tempMessage.state, BLINK) == 0) {
                    blink();
                    vTaskDelay((BLINK_INTERVALL - BLINK_FREQ) / portTICK_PERIOD_MS);
                } else {
                    if (strcmp(tempblueLedProfil.state, ON) == 0){
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_ON));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    } else {
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_OFF));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    }

                    if (strcmp(tempYellowLedProfil.state, ON) == 0){
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_ON));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    } else {
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_OFF));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    }

                    if (strcmp(tempGreenLedProfil.state, ON) == 0){
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_ON));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    } else {
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_OFF));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    }

                    if (strcmp(tempRedLedProfil.state, ON) == 0){
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_ON));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    } else {
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_OFF));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    }

                    // ESP_LOGI(TAG, "Set LED %s %s", message.led, message.state);
                    // if (strcmp(message.led, LED_BLUE) == 0) {
                    //     if (strcmp(message.state, ON) == 0){
                    //         ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_ON));
                    //         ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    //     } else {
                    //         ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_OFF));
                    //         ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
                    //     }
                    // } else if (strcmp(message.led, LED_YELLOW) == 0) {
                    //     if (strcmp(message.state, ON) == 0){
                    //         ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW, LEDC_DUTY_ON));
                    //         ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW));
                    //     } else {
                    //         ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW, LEDC_DUTY_OFF));
                    //         ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW));
                    //     }
                    // } else if (strcmp(message.led, LED_GREEN) == 0) {
                    //     if (strcmp(message.state, ON) == 0){
                    //         ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_DUTY_ON));
                    //         ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));
                    //     } else {
                    //         ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_DUTY_OFF));
                    //         ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));
                    //     }
                    // } else if (strcmp(message.led, LED_RED) == 0) {
                    //     if (strcmp(message.state, ON) == 0){
                    //         ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_DUTY_ON));
                    //         ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));
                    //     } else {
                    //         ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_DUTY_OFF));
                    //         ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));
                    //     }
                    // }
                }
            } else if (strcmp(tempMessage.mode, "test") == 0) {

                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_ON));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));

                vTaskDelay(1000 / portTICK_PERIOD_MS);

                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW, LEDC_DUTY_ON));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW));

                vTaskDelay(1000 / portTICK_PERIOD_MS);

                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_DUTY_ON));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));

                vTaskDelay(1000 / portTICK_PERIOD_MS);

                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_DUTY_ON));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));

                vTaskDelay(1000 / portTICK_PERIOD_MS);

                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_OFF));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));

                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW, LEDC_DUTY_OFF));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW));

                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_DUTY_OFF));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));

                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_DUTY_OFF));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));

                vTaskDelay(1000 / portTICK_PERIOD_MS);

                while (count < 4) {
                    if(count % 2 == 0) {
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_ON));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));

                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW, LEDC_DUTY_ON));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW));

                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_DUTY_ON));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));

                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_DUTY_ON));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));
                    } else {
                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_DUTY_OFF));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));

                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW, LEDC_DUTY_OFF));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_YELLOW));

                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_DUTY_OFF));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));

                        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_DUTY_OFF));
                        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));
                    }
                    count++;
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
                
                count = 0;
                loadSettings();
            } else {
                ESP_LOGI(TAG, "Mode is undefined. Set MODE to normal");
                strcpy(tempMessage.mode, "normal");
            }
        }
    }

    vTaskDelete(NULL);
}
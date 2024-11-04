#include <inttypes.h>
#include "driver/twai.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TX_GPIO_NUM 21  // TX pin for TWAI
#define RX_GPIO_NUM 22  // RX pin for TWAI
#define TAG "TWAI_APP"

// TWAI TX Task: Transmits the string "ayush" on the CAN bus
void tx_task(void *arg)
{
    twai_message_t messages[10];

    for (int i = 0; i < 10; i++) {
        messages[i].identifier = 0xAAAA0 + i;   // Set identifiers from 0xAAAA0 to 0xAAAA9
        messages[i].extd = 1;                   // Set to extended frame format
        messages[i].data_length_code = 8;       // 8 bytes for "ayush000" to "ayush009"

        // ASCII values for 'a', 'y', 'u', 's', 'h'
        messages[i].data[0] = 'a';
        messages[i].data[1] = 'y';
        messages[i].data[2] = 'u';
        messages[i].data[3] = 's';
        messages[i].data[4] = 'h';

        // ASCII values for '0', '0', and the current digit ('0' + i)
        messages[i].data[5] = '0';
        messages[i].data[6] = '0';
        messages[i].data[7] = '0' + i;         // Last character changes from '0' to '9'
    }

    vTaskDelay(200);

    // Example of how to send the messages
    twai_transmit(&messages[0], pdMS_TO_TICKS(10));
    vTaskDelay(10);
    twai_transmit(&messages[1], pdMS_TO_TICKS(10));
    vTaskDelay(10);
    twai_transmit(&messages[2], pdMS_TO_TICKS(10));
    vTaskDelay(10);
    twai_transmit(&messages[3], pdMS_TO_TICKS(10));
    vTaskDelay(10);
    twai_transmit(&messages[4], pdMS_TO_TICKS(10));
    vTaskDelay(10);
    twai_transmit(&messages[5], pdMS_TO_TICKS(10));
    vTaskDelay(10);
    twai_transmit(&messages[6], pdMS_TO_TICKS(10));
    vTaskDelay(10);
    twai_transmit(&messages[7], pdMS_TO_TICKS(10));
    vTaskDelay(10);
    twai_transmit(&messages[8], pdMS_TO_TICKS(10));
    vTaskDelay(10);
    twai_transmit(&messages[9], pdMS_TO_TICKS(10));
    
    for (int i = 0; i < 10; i++) {
        //if (twai_transmit(&messages[i], pdMS_TO_TICKS(7)) == ESP_OK) {
            printf("Message %d with ID %" PRIu32 " sent: %c%c%c%c%c%c%c%c\n", i, messages[i].identifier,
                   messages[i].data[0], messages[i].data[1], messages[i].data[2], messages[i].data[3],
                   messages[i].data[4], messages[i].data[5], messages[i].data[6], messages[i].data[7]);
        //} else {
          //  printf("Failed to send message %d\n", i);
       // }
    }
    vTaskDelete(NULL);
    // Configure the message to transmit
   /* twai_message_t message;
    message.identifier = 0xAAAA;
    message.extd = 1;
    message.data_length_code = 8; // 8 bytes for "ayush123"

    // ASCII values for 'a', 'y', 'u', 's', 'h', '1', '2', '3'
    message.data[0] = 'a';
    message.data[1] = 'y';
    message.data[2] = 'u';
    message.data[3] = 's';
    message.data[4] = 'h';
    message.data[5] = '1';
    message.data[6] = '2';
    message.data[7] = '3';

    // Queue message for transmission
    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        printf("Message 'ayush123' queued for transmission\n");
    } else {
        printf("Failed to queue message 'ayush123' for transmission\n");
    }

    // Delete the task after sending, as it will be rescheduled
    vTaskDelete(NULL);*/
}

// TWAI RX Task: Receives CAN messages and processes them
void rx_task(void *arg)
{
    twai_message_t message;
    if (twai_receive(&message, pdMS_TO_TICKS(10000)) == ESP_OK) {
        printf("Message received\n");

        // Check if the message is in Extended or Standard format
        if (message.extd) {
            printf("Message is in Extended Format\n");
        } else {
            printf("Message is in Standard Format\n");
        }

        // Print the identifier
        printf("ID is %" PRIu32 "\n", message.identifier);

        // Print data if it's a data frame (not a remote frame)
        if (!message.rtr) {
            printf("Received data: ");
            for (int i = 0; i < message.data_length_code; i++) {
                printf("%c", message.data[i]);
            }
            printf("\n");
        }
    } else {
        printf("Failed to receive message\n");
    }

    // Delete the task after receiving, as it will be rescheduled
    vTaskDelete(NULL);
}

// TWAI Setup and Initialization
void setup_twai() {
    // Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NO_ACK);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        printf("Driver installed\n");
    } else {
        printf("Failed to install driver\n");
        return;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK) {
        printf("Driver started\n");
    } else {
        printf("Failed to start driver\n");
        return;
    }
}

// Main application entry
void app_main() {
    setup_twai(); // Initialize and start TWAI driver

    // Schedule TX and RX tasks every 1 second
    const TickType_t xDelay = pdMS_TO_TICKS(1000); // 1-second delay

    while (1) {
        // Schedule TX task
        xTaskCreate(tx_task, "TX_TASK", 4096, NULL, 5, NULL);

        // Schedule RX task
        xTaskCreate(rx_task, "RX_TASK", 4096, NULL, 5, NULL);

        // Delay before rescheduling tasks
        vTaskDelay(xDelay);
   }
}

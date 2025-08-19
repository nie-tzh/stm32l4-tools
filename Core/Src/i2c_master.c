#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "shell.h"
#include "getopt.h"

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
void MX_I2C1_Init(uint32_t rate)
{

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    if (rate == 400) {
        hi2c1.Init.Timing = 0x00F12981;
    } else if (rate == 1000) {
        hi2c1.Init.Timing = 0x00B10E24;
    } else {
        hi2c1.Init.Timing = 0x10D19CE4;
    }    
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief I2C2 Initialization Function
 * @param None
 * @retval None
 */
void MX_I2C2_Init(uint32_t rate)
{

    /* USER CODE BEGIN I2C2_Init 0 */

    /* USER CODE END I2C2_Init 0 */

    /* USER CODE BEGIN I2C2_Init 1 */

    /* USER CODE END I2C2_Init 1 */
    hi2c2.Instance = I2C2;
    if (rate == 400) {
        hi2c2.Init.Timing = 0x00F12981;
    } else if (rate == 1000) {
        hi2c2.Init.Timing = 0x00B10E24;
    } else {
        hi2c2.Init.Timing = 0x10D19CE4;
    } 
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C2_Init 2 */

    /* USER CODE END I2C2_Init 2 */
}

void shell_test_i2c_detect(void * arg)
{
    uint8_t slave_addr = 0x60 << 1;
    uint8_t route_data[] = {0x1b, 0x20, 0x60, 0x41, 0x42, 0x43, 0x20, 
                            0x21, 0x22, 0x23, 0x50, 0x51, 0x52, 0x53, 
                            0x60, 0x41, 0x42, 0x43, 0x20, 0x20, 0x20, 
                            0x20, 0x6e, 0x6e, 0x6e, 0x6e, 0xd6, 0xb4};

    HAL_I2C_Master_Transmit(&hi2c1, slave_addr, route_data, 
            sizeof(route_data), HAL_MAX_DELAY);
#if 1
    printf("transmit:");
    for (uint8_t j = 0; j < sizeof(route_data); j++) {
        printf("%02x ", route_data[j]);
    }
#endif
    printf("\r\n");
    for (uint8_t addr = 0; addr < 0x7F; addr++) {
        if (HAL_I2C_Master_Transmit(&hi2c1, addr << 1, NULL, 0, HAL_MAX_DELAY) == HAL_OK) {
            printf("I2C device found at address 0x%02X\r\n", addr);
        }
    }
}

void shell_test_i2c_rate_change(void *arg)
{
    uint32_t argc, rate;
    char *argv[8];

    argc = cmdline_strtok((char*)arg, argv, 8);
    if (argc != 2) {
        printf("Please input rate.\r\n");
        return;
    }
    rate = atoi(argv[1]);

    HAL_I2C_DeInit(&hi2c1);
    MX_I2C1_Init(rate);
}

void shell_test_i2c_send_message(void * arg)
{
    uint16_t slave_addr = 0x60 << 1;
    // uint8_t data_out[9] = {0x08, 0x06, 0x0b, 0x24, 0xa1, 0x20, 0x04, 0x4e, 0x7a};
	uint8_t data_out[9] = {0x08, 0x06, 0x0b, 0x24, 0x13, 0x0c, 0x04, 0x9d, 0x6e};
    uint8_t data_in[9];
    uint8_t len = 0;

    printf("%s\n", __func__);
    HAL_I2C_Master_Transmit(&hi2c1, slave_addr, data_out, 
        sizeof(data_out), HAL_MAX_DELAY);
#if 1
    printf("transmit:");
    for (uint8_t j = 0; j < sizeof(data_out); j++) {
        printf("%02x ", data_out[j]);
    }
#endif

    memset(data_in, 0x00, sizeof(data_in));
    HAL_I2C_Master_Receive(&hi2c1, slave_addr, data_in,
        8, HAL_MAX_DELAY);
#if 1
    printf("received:");
    for (uint8_t j = 0; j < 8; j++) {
        printf("%02x ", data_in[j]);
    }
#endif
}
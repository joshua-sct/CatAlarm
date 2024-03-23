/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ACC_Pin GPIO_PIN_0
#define ACC_GPIO_Port GPIOA
#define ACC_EXTI_IRQn EXTI0_1_IRQn
#define BLK_Pin GPIO_PIN_1
#define BLK_GPIO_Port GPIOA
#define BLK_EXTI_IRQn EXTI0_1_IRQn
#define AIN_INT1_Pin GPIO_PIN_2
#define AIN_INT1_GPIO_Port GPIOB
#define AIN_INT2_Pin GPIO_PIN_8
#define AIN_INT2_GPIO_Port GPIOA
#define AIN_SCL_Pin GPIO_PIN_9
#define AIN_SCL_GPIO_Port GPIOA
#define AIN_SDA_Pin GPIO_PIN_10
#define AIN_SDA_GPIO_Port GPIOA
#define AOUT_SCL_Pin GPIO_PIN_11
#define AOUT_SCL_GPIO_Port GPIOA
#define AOUT_SDA_Pin GPIO_PIN_12
#define AOUT_SDA_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_8
#define BUZZER_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
void lis2dw12_init(void);
void lis2dw12_poll(void);
void get_acceleration(float*);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

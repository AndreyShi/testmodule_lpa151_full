/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
#define encoder_max 4096

#define QSPI_D2_Pin GPIO_PIN_2
#define QSPI_D2_GPIO_Port GPIOE
#define RESET_AG401_1_1_Pin GPIO_PIN_8
#define RESET_AG401_1_1_GPIO_Port GPIOI
#define SET_AG401_1_1_Pin GPIO_PIN_9
#define SET_AG401_1_1_GPIO_Port GPIOI
#define SET_CH2_AG__Pin GPIO_PIN_10
#define SET_CH2_AG__GPIO_Port GPIOI
#define RESET_CH2_AG__Pin GPIO_PIN_11
#define RESET_CH2_AG__GPIO_Port GPIOI
#define RESET_CH1_AG__Pin GPIO_PIN_0
#define RESET_CH1_AG__GPIO_Port GPIOF
#define SET_CH1_AG__Pin GPIO_PIN_1
#define SET_CH1_AG__GPIO_Port GPIOF
#define SET_AG420_2_CR_Pin GPIO_PIN_2
#define SET_AG420_2_CR_GPIO_Port GPIOF
#define RESET_AG420_2_CR_Pin GPIO_PIN_12
#define RESET_AG420_2_CR_GPIO_Port GPIOI
#define SET_AG420_2_TR2_Pin GPIO_PIN_13
#define SET_AG420_2_TR2_GPIO_Port GPIOI
#define RESET_AG420_2_TR2_Pin GPIO_PIN_14
#define RESET_AG420_2_TR2_GPIO_Port GPIOI
#define SET_AG420_1_TR2_Pin GPIO_PIN_3
#define SET_AG420_1_TR2_GPIO_Port GPIOF
#define RESET_AG420_1_TR2_Pin GPIO_PIN_4
#define RESET_AG420_1_TR2_GPIO_Port GPIOF
#define ISP_DETECT_Pin GPIO_PIN_5
#define ISP_DETECT_GPIO_Port GPIOF
#define ISP_RST_Pin GPIO_PIN_6
#define ISP_RST_GPIO_Port GPIOF
#define ISP_CLK_Pin GPIO_PIN_7
#define ISP_CLK_GPIO_Port GPIOF
#define ISP_MISO_Pin GPIO_PIN_8
#define ISP_MISO_GPIO_Port GPIOF
#define ISP_MOSI_Pin GPIO_PIN_9
#define ISP_MOSI_GPIO_Port GPIOF
#define ISP_EN_Pin GPIO_PIN_10
#define ISP_EN_GPIO_Port GPIOF
#define SET_CR_1_Pin GPIO_PIN_0
#define SET_CR_1_GPIO_Port GPIOC
#define RESET_CR_1_Pin GPIO_PIN_1
#define RESET_CR_1_GPIO_Port GPIOC
#define RESET_AG420_2_CH1_Pin GPIO_PIN_2
#define RESET_AG420_2_CH1_GPIO_Port GPIOC
#define SET_AG420_2_CH1_Pin GPIO_PIN_3
#define SET_AG420_2_CH1_GPIO_Port GPIOC
#define BLUE_LED_Pin GPIO_PIN_0
#define BLUE_LED_GPIO_Port GPIOA
#define GREEN_LED_Pin GPIO_PIN_1
#define GREEN_LED_GPIO_Port GPIOA
#define YELLOW_LED_Pin GPIO_PIN_2
#define YELLOW_LED_GPIO_Port GPIOA
#define SET_CH2_AG_H2_Pin GPIO_PIN_2
#define SET_CH2_AG_H2_GPIO_Port GPIOH
#define RESET_CH2_AG_H3_Pin GPIO_PIN_3
#define RESET_CH2_AG_H3_GPIO_Port GPIOH
#define RESET_AG420_2_CH2_Pin GPIO_PIN_4
#define RESET_AG420_2_CH2_GPIO_Port GPIOH
#define SET_AG420_2_CH2_Pin GPIO_PIN_5
#define SET_AG420_2_CH2_GPIO_Port GPIOH
#define RED_LED_Pin GPIO_PIN_3
#define RED_LED_GPIO_Port GPIOA
#define DispCS_Pin GPIO_PIN_4
#define DispCS_GPIO_Port GPIOA
#define DispSCK_Pin GPIO_PIN_5
#define DispSCK_GPIO_Port GPIOA
#define DispMOSI_Pin GPIO_PIN_7
#define DispMOSI_GPIO_Port GPIOA
#define DispRST_Pin GPIO_PIN_4
#define DispRST_GPIO_Port GPIOC
#define DispDC_Pin GPIO_PIN_5
#define DispDC_GPIO_Port GPIOC
#define SET_AG401_2_2_Pin GPIO_PIN_0
#define SET_AG401_2_2_GPIO_Port GPIOB
#define RESET_AG401_2_2_Pin GPIO_PIN_1
#define RESET_AG401_2_2_GPIO_Port GPIOB
#define QSPI_CLK_Pin GPIO_PIN_2
#define QSPI_CLK_GPIO_Port GPIOB
#define SET_AG401_1_2_Pin GPIO_PIN_15
#define SET_AG401_1_2_GPIO_Port GPIOI
#define RESET_AG401_1_2_Pin GPIO_PIN_11
#define RESET_AG401_1_2_GPIO_Port GPIOF
#define SET_CH1_AG_F12_Pin GPIO_PIN_12
#define SET_CH1_AG_F12_GPIO_Port GPIOF
#define RESET_CH1_AG_F13_Pin GPIO_PIN_13
#define RESET_CH1_AG_F13_GPIO_Port GPIOF
#define SET_AG420_1_CH2_Pin GPIO_PIN_14
#define SET_AG420_1_CH2_GPIO_Port GPIOF
#define RESET_AG420_1_CH2_Pin GPIO_PIN_15
#define RESET_AG420_1_CH2_GPIO_Port GPIOF
#define SET_WARMUP_2_Pin GPIO_PIN_7
#define SET_WARMUP_2_GPIO_Port GPIOE
#define RESET_WARMUP_2_Pin GPIO_PIN_8
#define RESET_WARMUP_2_GPIO_Port GPIOE
#define RESET_AG420_1_TR1_Pin GPIO_PIN_9
#define RESET_AG420_1_TR1_GPIO_Port GPIOE
#define SET_AG420_1_TR1_Pin GPIO_PIN_10
#define SET_AG420_1_TR1_GPIO_Port GPIOE
#define RESET_R1_Pin GPIO_PIN_11
#define RESET_R1_GPIO_Port GPIOE
#define SET_R1_Pin GPIO_PIN_12
#define SET_R1_GPIO_Port GPIOE
#define RESET_R2_Pin GPIO_PIN_13
#define RESET_R2_GPIO_Port GPIOE
#define SET_R2_Pin GPIO_PIN_14
#define SET_R2_GPIO_Port GPIOE
#define RESET_R3_Pin GPIO_PIN_15
#define RESET_R3_GPIO_Port GPIOE
#define Tx_Pin GPIO_PIN_10
#define Tx_GPIO_Port GPIOB
#define Rx_Pin GPIO_PIN_11
#define Rx_GPIO_Port GPIOB
#define SET_R3_Pin GPIO_PIN_6
#define SET_R3_GPIO_Port GPIOH
#define RESET_R4_Pin GPIO_PIN_7
#define RESET_R4_GPIO_Port GPIOH
#define SET_R4_Pin GPIO_PIN_8
#define SET_R4_GPIO_Port GPIOH
#define RESET_R5_Pin GPIO_PIN_9
#define RESET_R5_GPIO_Port GPIOH
#define SET_R5_Pin GPIO_PIN_10
#define SET_R5_GPIO_Port GPIOH
#define RESET_R6_Pin GPIO_PIN_11
#define RESET_R6_GPIO_Port GPIOH
#define SET_R6_Pin GPIO_PIN_12
#define SET_R6_GPIO_Port GPIOH
#define LPA_USB_SET_Pin GPIO_PIN_12
#define LPA_USB_SET_GPIO_Port GPIOB
#define LPA_USB_RESET_Pin GPIO_PIN_13
#define LPA_USB_RESET_GPIO_Port GPIOB
#define LPA_USB_D__Pin GPIO_PIN_14
#define LPA_USB_D__GPIO_Port GPIOB
#define LPA_USB_D_B15_Pin GPIO_PIN_15
#define LPA_USB_D_B15_GPIO_Port GPIOB
#define RS_DE_Pin GPIO_PIN_8
#define RS_DE_GPIO_Port GPIOD
#define ISPOWER_Pin GPIO_PIN_9
#define ISPOWER_GPIO_Port GPIOD
#define QSPI_D0_Pin GPIO_PIN_11
#define QSPI_D0_GPIO_Port GPIOD
#define QSPI_D1_Pin GPIO_PIN_12
#define QSPI_D1_GPIO_Port GPIOD
#define QSPI_D3_Pin GPIO_PIN_13
#define QSPI_D3_GPIO_Port GPIOD
#define SET_LPA_POW_Pin GPIO_PIN_14
#define SET_LPA_POW_GPIO_Port GPIOD
#define RESET_LPA_POW_Pin GPIO_PIN_15
#define RESET_LPA_POW_GPIO_Port GPIOD
#define EncoderA_Pin GPIO_PIN_6
#define EncoderA_GPIO_Port GPIOC
#define EncoderB_Pin GPIO_PIN_7
#define EncoderB_GPIO_Port GPIOC
#define EncoderS_Pin GPIO_PIN_9
#define EncoderS_GPIO_Port GPIOC
#define FRAMEOUT_Pin GPIO_PIN_9
#define FRAMEOUT_GPIO_Port GPIOA
#define FRAMEIN_Pin GPIO_PIN_10
#define FRAMEIN_GPIO_Port GPIOA
#define FRAMEIN_EXTI_IRQn EXTI15_10_IRQn
#define TB_USB_D__Pin GPIO_PIN_11
#define TB_USB_D__GPIO_Port GPIOA
#define TB_USB_D_A12_Pin GPIO_PIN_12
#define TB_USB_D_A12_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define DAC_LDAC_Pin GPIO_PIN_13
#define DAC_LDAC_GPIO_Port GPIOH
#define DAC_CLR_Pin GPIO_PIN_14
#define DAC_CLR_GPIO_Port GPIOH
#define DAC_RESET_Pin GPIO_PIN_15
#define DAC_RESET_GPIO_Port GPIOH
#define DAC_SYNC_Pin GPIO_PIN_0
#define DAC_SYNC_GPIO_Port GPIOI
#define DAC_CLK_Pin GPIO_PIN_1
#define DAC_CLK_GPIO_Port GPIOI
#define DAC_SDO_Pin GPIO_PIN_2
#define DAC_SDO_GPIO_Port GPIOI
#define DAC_SDIN_Pin GPIO_PIN_3
#define DAC_SDIN_GPIO_Port GPIOI
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define SET_SC_Pin GPIO_PIN_10
#define SET_SC_GPIO_Port GPIOC
#define RESET_SC_Pin GPIO_PIN_11
#define RESET_SC_GPIO_Port GPIOC
#define SET_DAC__Pin GPIO_PIN_12
#define SET_DAC__GPIO_Port GPIOC
#define RESET_DAC__Pin GPIO_PIN_0
#define RESET_DAC__GPIO_Port GPIOD
#define SET_DAC_D1_Pin GPIO_PIN_1
#define SET_DAC_D1_GPIO_Port GPIOD
#define RESET_DAC_D2_Pin GPIO_PIN_2
#define RESET_DAC_D2_GPIO_Port GPIOD
#define EN_RELAY_Pin GPIO_PIN_3
#define EN_RELAY_GPIO_Port GPIOD
#define SET_CR_2_Pin GPIO_PIN_4
#define SET_CR_2_GPIO_Port GPIOD
#define RESET_CR_2_Pin GPIO_PIN_5
#define RESET_CR_2_GPIO_Port GPIOD
#define SET_WARMUP_1_Pin GPIO_PIN_6
#define SET_WARMUP_1_GPIO_Port GPIOD
#define RESET_WARMUP_1_Pin GPIO_PIN_7
#define RESET_WARMUP_1_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define JTRST_Pin GPIO_PIN_4
#define JTRST_GPIO_Port GPIOB
#define QSPI_NCS_Pin GPIO_PIN_6
#define QSPI_NCS_GPIO_Port GPIOB
#define SET_BOOT0_Pin GPIO_PIN_7
#define SET_BOOT0_GPIO_Port GPIOB
#define SET_AG420_1_CR_Pin GPIO_PIN_8
#define SET_AG420_1_CR_GPIO_Port GPIOB
#define RESET_AG420_1_CR_Pin GPIO_PIN_9
#define RESET_AG420_1_CR_GPIO_Port GPIOB
#define RESET_AG420_2_TR1_Pin GPIO_PIN_0
#define RESET_AG420_2_TR1_GPIO_Port GPIOE
#define SET_AG420_2_TR1_Pin GPIO_PIN_1
#define SET_AG420_2_TR1_GPIO_Port GPIOE
#define SET_AG420_1_CH1_Pin GPIO_PIN_4
#define SET_AG420_1_CH1_GPIO_Port GPIOI
#define RESET_AG420_1_CH1_Pin GPIO_PIN_5
#define RESET_AG420_1_CH1_GPIO_Port GPIOI
#define SET_AG401_2_1_Pin GPIO_PIN_6
#define SET_AG401_2_1_GPIO_Port GPIOI
#define RESET_AG401_2_1_Pin GPIO_PIN_7
#define RESET_AG401_2_1_GPIO_Port GPIOI

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

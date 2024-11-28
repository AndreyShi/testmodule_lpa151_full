/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
        * Free pins are configured automatically as Analog (this feature is enabled through 
        * the Code Generation settings)
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, RESET_AG401_1_1_Pin|SET_AG401_1_1_Pin|SET_CH2_AG__Pin|RESET_CH2_AG__Pin 
                          |RESET_AG420_2_CR_Pin|SET_AG420_2_TR2_Pin|RESET_AG420_2_TR2_Pin|SET_AG401_1_2_Pin 
                          |DAC_SYNC_Pin|SET_AG420_1_CH1_Pin|RESET_AG420_1_CH1_Pin|SET_AG401_2_1_Pin 
                          |RESET_AG401_2_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, RESET_CH1_AG__Pin|SET_CH1_AG__Pin|SET_AG420_2_CR_Pin|SET_AG420_1_TR2_Pin 
                          |RESET_AG420_1_TR2_Pin|ISP_RST_Pin|ISP_EN_Pin|RESET_AG401_1_2_Pin 
                          |SET_CH1_AG_F12_Pin|RESET_CH1_AG_F13_Pin|SET_AG420_1_CH2_Pin|RESET_AG420_1_CH2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SET_CR_1_Pin|RESET_CR_1_Pin|RESET_AG420_2_CH1_Pin|SET_AG420_2_CH1_Pin 
                          |DispRST_Pin|DispDC_Pin|SET_SC_Pin|RESET_SC_Pin 
                          |SET_DAC__Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BLUE_LED_Pin|GREEN_LED_Pin|YELLOW_LED_Pin|RED_LED_Pin 
                          |DispCS_Pin|FRAMEOUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, SET_CH2_AG_H2_Pin|RESET_CH2_AG_H3_Pin|RESET_AG420_2_CH2_Pin|SET_AG420_2_CH2_Pin 
                          |SET_R3_Pin|RESET_R4_Pin|SET_R4_Pin|RESET_R5_Pin 
                          |SET_R5_Pin|RESET_R6_Pin|SET_R6_Pin|DAC_LDAC_Pin 
                          |DAC_CLR_Pin|DAC_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SET_AG401_2_2_Pin|RESET_AG401_2_2_Pin|LPA_USB_SET_Pin|LPA_USB_RESET_Pin 
                          |SET_BOOT0_Pin|SET_AG420_1_CR_Pin|RESET_AG420_1_CR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, SET_WARMUP_2_Pin|RESET_WARMUP_2_Pin|RESET_AG420_1_TR1_Pin|SET_AG420_1_TR1_Pin 
                          |RESET_R1_Pin|SET_R1_Pin|RESET_R2_Pin|SET_R2_Pin 
                          |RESET_R3_Pin|RESET_AG420_2_TR1_Pin|SET_AG420_2_TR1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RS_DE_Pin|SET_LPA_POW_Pin|RESET_LPA_POW_Pin|RESET_DAC__Pin 
                          |SET_DAC_D1_Pin|RESET_DAC_D2_Pin|EN_RELAY_Pin|SET_CR_2_Pin 
                          |RESET_CR_2_Pin|SET_WARMUP_1_Pin|RESET_WARMUP_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE3 PE4 PE5 PE6 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PIPin PIPin PIPin PIPin 
                           PIPin PIPin PIPin PIPin 
                           PIPin PIPin PIPin PIPin 
                           PIPin */
  GPIO_InitStruct.Pin = RESET_AG401_1_1_Pin|SET_AG401_1_1_Pin|SET_CH2_AG__Pin|RESET_CH2_AG__Pin 
                          |RESET_AG420_2_CR_Pin|SET_AG420_2_TR2_Pin|RESET_AG420_2_TR2_Pin|SET_AG401_1_2_Pin 
                          |DAC_SYNC_Pin|SET_AG420_1_CH1_Pin|RESET_AG420_1_CH1_Pin|SET_AG401_2_1_Pin 
                          |RESET_AG401_2_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : PC13 PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin PFPin PFPin 
                           PFPin PFPin PFPin PFPin 
                           PFPin PFPin PFPin PFPin */
  GPIO_InitStruct.Pin = RESET_CH1_AG__Pin|SET_CH1_AG__Pin|SET_AG420_2_CR_Pin|SET_AG420_1_TR2_Pin 
                          |RESET_AG420_1_TR2_Pin|ISP_RST_Pin|ISP_EN_Pin|RESET_AG401_1_2_Pin 
                          |SET_CH1_AG_F12_Pin|RESET_CH1_AG_F13_Pin|SET_AG420_1_CH2_Pin|RESET_AG420_1_CH2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ISP_DETECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ISP_DETECT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin 
                           PCPin PCPin PCPin PCPin 
                           PCPin */
  GPIO_InitStruct.Pin = SET_CR_1_Pin|RESET_CR_1_Pin|RESET_AG420_2_CH1_Pin|SET_AG420_2_CH1_Pin 
                          |DispRST_Pin|DispDC_Pin|SET_SC_Pin|RESET_SC_Pin 
                          |SET_DAC__Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin 
                           PAPin PAPin */
  GPIO_InitStruct.Pin = BLUE_LED_Pin|GREEN_LED_Pin|YELLOW_LED_Pin|RED_LED_Pin 
                          |DispCS_Pin|FRAMEOUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PHPin PHPin PHPin PHPin 
                           PHPin PHPin PHPin PHPin 
                           PHPin PHPin PHPin PHPin 
                           PHPin PHPin */
  GPIO_InitStruct.Pin = SET_CH2_AG_H2_Pin|RESET_CH2_AG_H3_Pin|RESET_AG420_2_CH2_Pin|SET_AG420_2_CH2_Pin 
                          |SET_R3_Pin|RESET_R4_Pin|SET_R4_Pin|RESET_R5_Pin 
                          |SET_R5_Pin|RESET_R6_Pin|SET_R6_Pin|DAC_LDAC_Pin 
                          |DAC_CLR_Pin|DAC_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PA6 PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin 
                           PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = SET_AG401_2_2_Pin|RESET_AG401_2_2_Pin|LPA_USB_SET_Pin|LPA_USB_RESET_Pin 
                          |SET_BOOT0_Pin|SET_AG420_1_CR_Pin|RESET_AG420_1_CR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PJ0 PJ1 PJ2 PJ3 
                           PJ4 PJ5 PJ6 PJ7 
                           PJ8 PJ9 PJ10 PJ11 
                           PJ12 PJ13 PJ14 PJ15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7 
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

  /*Configure GPIO pins : PG0 PG1 PG2 PG3 
                           PG4 PG5 PG6 PG7 
                           PG8 PG9 PG10 PG11 
                           PG12 PG13 PG14 PG15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7 
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin 
                           PEPin PEPin PEPin PEPin 
                           PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = SET_WARMUP_2_Pin|RESET_WARMUP_2_Pin|RESET_AG420_1_TR1_Pin|SET_AG420_1_TR1_Pin 
                          |RESET_R1_Pin|SET_R1_Pin|RESET_R2_Pin|SET_R2_Pin 
                          |RESET_R3_Pin|RESET_AG420_2_TR1_Pin|SET_AG420_2_TR1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin 
                           PDPin PDPin PDPin PDPin 
                           PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = RS_DE_Pin|SET_LPA_POW_Pin|RESET_LPA_POW_Pin|RESET_DAC__Pin 
                          |SET_DAC_D1_Pin|RESET_DAC_D2_Pin|EN_RELAY_Pin|SET_CR_2_Pin 
                          |RESET_CR_2_Pin|SET_WARMUP_1_Pin|RESET_WARMUP_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ISPOWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ISPOWER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PD10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PK0 PK1 PK2 PK3 
                           PK4 PK5 PK6 PK7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = EncoderS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EncoderS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = FRAMEIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FRAMEIN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

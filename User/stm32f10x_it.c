/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  
  * @param //RXNE：接收非空 ((Receive)，Not Empty)
					//NE：非空 (Not Empty)RESET (0)：表示“复位”、“没有发生”、“无效”、“低电平”。
					//SET (1)：表示“置位”、“发生了”、“有效”、“高电平”。外部数据通过 RX 引脚进入 STM32。
						STM32 硬件把数据搬运到数据寄存器 (DR) 中。一旦数据搬进去，寄存器就不空了，硬件会自动把 RXNE 标志位 打成 1 (SET)。
  * @retval 你会发现参数都叫 USART_IT_RXNE，但 USART_ITConfig 函数内部会去操作 CR1 (开关)，而 USART_GetITStatus 内部会去读取 SR (状态)
  */
#include "stm32f10x_usart.h"

/* extern 回调 —— 只声明，不实现 */

void USART1_IRQHandler(void)
{
	// 分发1：监测是否有数据进入，USART_IT_RXNE和USART_IT_IDLE是参考状态寄存器的功能。
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)      //为什么要写 != RESET 而不是 == SET？RXNE：读数据寄存器非空 (Read data register not empty) 位5 
																															//当RDR移位寄存器中的数据被转移到USART_DR寄存器中，该位被硬件置位。如果
	{																												//USART_CR1寄存器中的RXNEIE为1，则产生中断。对USART_DR的读操作可以将该位清
																															//零。RXNE位也可以通过写入0来清除，只有在多缓存通讯中才推荐这种清除程序。
																												//在某些底层寄存器定义中，表示“成功”或“发生”的值可能不仅仅是 1，可能是非 0 的任何值（比如 0xF
		extern void USART1_RxEvent(void);

		USART1_RxEvent(); // 仅通知
	}

	
//下面可以用于发不定长数据的中断
	
	
///* 空闲中断（一帧结束） */
//	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
//	{
//		extern void USART1_IdleEvent(void);
//		USART1_IdleEvent(); // 仅通知


//	/* 清 IDLE：读 SR + DR */
//		(void)USART1->SR;
//		(void)USART1->DR;
//	}
}

void USART2_IRQHandler(void) 
{
		extern void USART2_RxEvent(void);

		USART2_RxEvent(); // 仅通知
}

/* 清 RXNE：读 DR 后硬件自动清RXNE 是“读 DR 自动清除”
你如果不读 USART_ReceiveData()：中断会一直进，程序会 卡死在中断里 。*/



/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

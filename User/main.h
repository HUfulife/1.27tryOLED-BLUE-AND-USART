#ifndef __main_H
#define ___main_H

#include "stm32f10x.h" // 必须引入，否则识别不了USARTTypeDef这些类型             // Device header
#include <stdio.h>     // 为了使用printf
#include "bsp_usart.h"
#include "bsp_delay.h"
#include "bsp_sys.h"
#include "OLED.h"
#include "BLUE.h"
#include "MyI2C.h"

//#include "OLED_Font.h"    字符库只有OLED用到了，只要放到OLED.c里面。
//简单来说，编译器发现 OLED_F8x16（这通常是字库数组）在 oled.h 和 main.h 里各存了一份，它不知道该用哪一个，所以罢工了。

#endif

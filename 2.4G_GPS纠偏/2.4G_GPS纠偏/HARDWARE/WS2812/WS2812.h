#ifndef __WS2812_H
#define __WS2812_H
#include "sys.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "time.h"
//#include "GlobalVariable.h"
/*
			必须要每次刷新种子。每次的随机数才会不一样
			srand()在#include"stdlib.h"
			rand()在
*/



/* Define --------------------------------------------------------------------*/
//硬件spi模拟ws2811时序（用spi的3位数据模拟ws281x的一位数据）
//要将系统时钟设置为72M，分频数设置为32，则SPI的通信频率为2.25M，传输一位数据的时间约为444纳秒（ns）
//444ns   888ns  符合WS281X芯片的通信时序。
//  __
// |  |_|   0b110  high level
//  _
// | |__|   0b100  low level
#define WS2812_HIG  (0x06)
#define WS2812_LOW  (0x04)
//WS2812的数量
#define WS2812_NUM   12//24
#define  Debug_SPIx_Tx  0x4000380C  //外设地址
extern uint8_t LED_StateBuff[WS2812_NUM];
extern uint8_t Flag_WS2812;
/* typedef -------------------------------------------------------------------*/
typedef struct
{
    uint8_t hig;
    uint8_t mid;
    uint8_t low;
} ws2812ColorBit_t;
/**
 * @brief struct WS2812
 *
 */
typedef struct
{
    uint32_t Info;                      //reserve
    union
    {
        uint8_t Buff[9];
        struct
        {
            uint8_t G[3];               //G First
            uint8_t R[3];
            uint8_t B[3];               //
        }RGB;
    }Col[WS2812_NUM];
} ws2812_t;

/* types ---------------------------------------------------------------------*/
extern ws2812_t g_ws2812;

/* macro ---------------------------------------------------------------------*/
/* Unsigned.  */
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;


/* functions -----------------------------------------------------------------*/
//void ws2812_spi_init();

uint32_t WS2812_Color(uint8_t red, uint8_t green, uint8_t blue);
void WS2812_OneSet( uint8_t num, uint32_t RGB );

void WS2812_CloseAll(void);
void WS2812_SetAll(uint32_t RGB);

void WS2812_Show(void);
//void LED_Pro(void);
void LED_Pro(uint8_t *LED_State,uint8_t num);
void ws2812_SPI_Init(void);
void SPI_SendData_DMA(uint8_t* txbuff,uint8_t len);
uint32_t get_rand(void);
void ws2812_RandShow(uint32_t Rand,uint8_t num);

#ifdef __cplusplus
}

#endif








#endif

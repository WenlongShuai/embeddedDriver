#ifndef __AD_H
#define __AD_H

extern uint16_t AD_Value[150];

void AD_Init(void);
uint32_t Advalue_SUM(uint16_t *advalue,uint8_t num);

#endif

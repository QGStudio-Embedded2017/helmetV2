#ifndef __USART3_H
#define	__USART3_H

#include "stm32f10x.h"
#include <stdio.h>

void u3_printf(char* fmt,...);  
extern void USART3_Config(u32 bound);
extern void USART3_printf(USART_TypeDef* USARTx, char *Data,...);
extern void USART3_IRQHandler(void);
extern char * get_usart3_Rxbuf_len(uint8_t *len);
extern char * get_usart3_Rxbuf(void);
extern void clean_usart3_Rxbuf(void);

#endif /* __USART3_H */

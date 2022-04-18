#ifndef BSP_RC_H
#define BSP_RC_H
#include "struct_typedef.h"

#define RX_BUF_NUM	36

void usart_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num);
void usart1_tx_dma_init(void);
//void usart1_tx_dma_enable(uint8_t *data, uint16_t len);
void usart_printf(const char *fmt,...);
	
#endif

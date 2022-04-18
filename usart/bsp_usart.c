#include "bsp_usart.h"
#include "main.h"
#include <stdarg.h>

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

//receive data, 18 bytes one frame, but set 36 bytes 
//接收原始数据，为18个字节，给了36个字节长度，防止DMA传输越界
static uint8_t rx_buf[2][RX_BUF_NUM];

void usart_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
    //enable the DMA transfer for the receiver request
    //使能DMA串口接收
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAR);

    //enalbe idle interrupt
    //使能空闲中断
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    //disable DMA
    //失效DMA
    __HAL_DMA_DISABLE(&hdma_usart1_rx);
    while(hdma_usart1_rx.Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(&hdma_usart1_rx);
    }

    hdma_usart1_rx.Instance->PAR = (uint32_t) & (USART1->DR);
    //memory buffer 1
    //内存缓冲区1
    hdma_usart1_rx.Instance->M0AR = (uint32_t)(rx1_buf);
    //memory buffer 2
    //内存缓冲区2
    hdma_usart1_rx.Instance->M1AR = (uint32_t)(rx2_buf);
    //data length
    //数据长度
    hdma_usart1_rx.Instance->NDTR = dma_buf_num;
    //enable double memory buffer
    //使能双缓冲区
    SET_BIT(hdma_usart1_rx.Instance->CR, DMA_SxCR_DBM);

    //enable DMA
    //使能DMA
    __HAL_DMA_ENABLE(&hdma_usart1_rx);

}

//串口中断
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
    if(huart1.Instance->SR & UART_FLAG_RXNE)//接收到数据
    {
        __HAL_UART_CLEAR_PEFLAG(&huart1);
    }
    else if(USART1->SR & UART_FLAG_IDLE)
    {
        static uint16_t this_time_rx_len = 0;

        __HAL_UART_CLEAR_PEFLAG(&huart1);

        if ((hdma_usart1_rx.Instance->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */
    
            //disable DMA
            //失效DMA
            __HAL_DMA_DISABLE(&hdma_usart1_rx);

            //get receive data length, length = set_data_length - remain_length
            //获取接收数据长度,长度 = 设定长度 - 剩余长度
            this_time_rx_len = RX_BUF_NUM - hdma_usart1_rx.Instance->NDTR;

            //reset set_data_lenght
            //重新设定数据长度
            hdma_usart1_rx.Instance->NDTR = RX_BUF_NUM;

            //set memory buffer 1
            //设定缓冲区1
            hdma_usart1_rx.Instance->CR |= DMA_SxCR_CT;
            
            //enable DMA
            //使能DMA
            __HAL_DMA_ENABLE(&hdma_usart1_rx);

//            if(this_time_rx_len == RC_FRAME_LENGTH)
//            {
//                sbus_to_rc(rx_buf[0], &rc_ctrl);
//            }
        }
        else
        {
            /* Current memory buffer used is Memory 1 */
            //disable DMA
            //失效DMA
            __HAL_DMA_DISABLE(&hdma_usart1_rx);

            //get receive data length, length = set_data_length - remain_length
            //获取接收数据长度,长度 = 设定长度 - 剩余长度
            this_time_rx_len = RX_BUF_NUM - hdma_usart1_rx.Instance->NDTR;

            //reset set_data_lenght
            //重新设定数据长度
            hdma_usart1_rx.Instance->NDTR = RX_BUF_NUM;

            //set memory buffer 0
            //设定缓冲区0
            DMA1_Stream1->CR &= ~(DMA_SxCR_CT);
            
            //enable DMA
            //使能DMA
            __HAL_DMA_ENABLE(&hdma_usart1_rx);

//            if(this_time_rx_len == RC_FRAME_LENGTH)
//            {
//                //处理遥控器数据
//                sbus_to_rc(rx_buf[1], &rc_ctrl);
//            }
        }
    }
}

void usart1_tx_dma_init(void)
{
    //enable the DMA transfer for the transmit request
    //使能DMA串口发送
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAT);
}

//无法使用
//void usart1_tx_dma_enable(uint8_t *data, uint16_t len)
//{

//    //disable DMA
//    //失效DMA
//    __HAL_DMA_DISABLE(&hdma_usart1_tx);
//    while(hdma_usart1_tx.Instance->CR & DMA_SxCR_EN) //DMA_SxCR置1，准备传输状态
//    {
//        __HAL_DMA_DISABLE(&hdma_usart1_tx);	//置0，下面才能清除标志位
//    }

//    //clear flag
//    //清除标志位
//    __HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_HISR_TCIF7);
//    __HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_HISR_HTIF7);

////	hdma_usart1_rx.Instance->PAR = (uint32_t)&huart1.Instance->DR;
//    //set data address
//    //设置数据地址
//    hdma_usart1_tx.Instance->M0AR = (uint32_t)(data);
//    //set data length
//    //设置数据长度
//    hdma_usart1_tx.Instance->NDTR = len;

//    //enable DMA
//    //使能DMA
//    __HAL_DMA_ENABLE(&hdma_usart1_tx);
//}

void usart_printf(const char *fmt,...)
{
	static uint8_t tx_buf[256] = {0};
	static va_list ap;
	static uint16_t len;
	va_start(ap,fmt);
	
	len = vsprintf( (char*)tx_buf, fmt, ap );
	va_end(ap);
	
	//usart1_tx_dma_enable(tx_buf, len);
	HAL_UART_Transmit_DMA(&huart1,tx_buf,len);
}


/* ---------- hal库串口重定向 ---------- */
// 需要在mdk5里使能microlib
#include <stdio.h>

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);//注意把&huart1改为自己的stm32使用的串口号

	return ch;
}
/* ---------- hal库串口重定向 ---------- */
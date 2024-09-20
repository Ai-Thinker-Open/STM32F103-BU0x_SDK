#include "Generic.h"
#include "Generic_cmd.h"

#include "hal_usart.h"
#include "hal_flash.h"
#include "hal_usb.h"
#include "OSAL.h"

enum UART_FRAME
{
    UART_FRAME_JS
};


static QUEUE uart_queue;


#define USART1_RX_ISR_MAX_LEN   64

static uint16_t usart1_rx_sta = 0 ;
static uint8_t usart1_rx_buf[USART1_RX_ISR_MAX_LEN] = { 0 };
static uint16_t usb_rx_sta = 0 ;
static uint8_t usb_rx_buf[USART1_RX_ISR_MAX_LEN] = { 0 };


int App_Module_CMD_Queue_Init (void)
{
    memset (usart1_rx_buf, 0, sizeof (usart1_rx_buf));
    memset (usb_rx_buf, 0, sizeof (usb_rx_buf));

    osal_CreateQueue (&uart_queue, QUEUE_MSG_MAX); //Data queue reset
}


int App_Module_Process_USB_CMD (void)
{
    int ret = 0;
    uint8_t USB_RxBuff[128];
    memset (USB_RxBuff, 0, sizeof (USB_RxBuff));

    int len = HalUsbRead (USB_RxBuff, sizeof (USB_RxBuff));

    if (len != 0)
    {
        for (int i = 0; i < len; i++)
        {
            if (usart1_recv_msg_handler (USB_RxBuff[i], &usb_rx_sta, usb_rx_buf))
            {
                command_parser (usb_rx_buf, len);
                //Clear the structure
                usb_rx_sta = 0;
                memset (usb_rx_buf, 0, sizeof (usb_rx_buf));
            }
        }
    }

    return ret;
}

int App_Module_Process_USART_CMD (void)
{
    Message msg;

    if (osal_Dequeue (&uart_queue, &msg) == TRUE)
    {
        if (sys_para.param_Config.s.userConfig.workmode != 1)
        {
            //_dbg_printf("msg.buf:%s\r\n",msg.buf);
            command_parser (msg.buf, msg.len);
        }
        else
        {
#ifndef JOHHN
            at_cmd_recv (msg.buf, msg.len);
//          _dbg_printf("产测模式\n");
            //产测AT
#endif
        }
    }
}

/*
 *  功能:串口发送函数1
 *
 */
int App_Module_Uart_Send (uint8_t* buf, uint16_t len)
{
    int ret = 0;

    if ( (sys_uart_dma_buf.uart_dma_index + len) < sizeof (sys_uart_dma_buf.uart_dma_tx_tmp))
    {
        /*赋值到缓存区*/
        memcpy (sys_uart_dma_buf.uart_dma_tx_tmp + sys_uart_dma_buf.uart_dma_index, buf, len);
        sys_uart_dma_buf.uart_dma_index += len;
    }
    else
    {
//      while(1);
    }

    if (sys_uart_dma_buf.uart_dma_report == true && sys_uart_dma_buf.uart_dma_index != 0)
    {
        /*赋值到发送区*/
        memcpy (sys_uart_dma_buf.uart_dma_tx, sys_uart_dma_buf.uart_dma_tx_tmp, sys_uart_dma_buf.uart_dma_index);
        USART1_SendBuffer (sys_uart_dma_buf.uart_dma_tx, sys_uart_dma_buf.uart_dma_index, false);
        sys_uart_dma_buf.uart_dma_report = false;
        sys_uart_dma_buf.uart_dma_index = 0;
    }

    //HalUsbWrite(buf, len);
    return ret;
}

/*
*   功能:数据发送函数（uart1）
 *
 */
void port_tx_msg (uint8_t* buf, uint16_t len)
{
    App_Module_Uart_Send (buf, len);
    HalUsbWrite (buf, len);
}

/*
 *  功能:数据发送函数(usb)
 *
 */
void data_tx_msg (unsigned char* str, char len)
{
    //USART2SendDatas(str, len);
    HalUsbWrite (str, len);
}

/*
*   功能：数据发送函数(uart2)
 *
 */
void wifi_tx_msg (unsigned char* str, char len)
{
    USART2SendDatas (str, len);
    //HalUsbWrite(str, len);
}

uint8_t get_Xor_CRC (uint8_t* bytes, int offset, int len)
{
    uint8_t xor_crc = 0;
    int i;

    for (i = 0; i < len; i++)
    {
        xor_crc ^= bytes[offset + i];
    }

    return xor_crc;
}


static uint8_t usart1_recv_msg_handler (uint8_t ch, uint16_t* rx_sta, uint8_t* rx_buf)
{
    uint8_t ret = false;

    if ( (*rx_sta & 0x8000) == 0)
    {
        rx_buf[ *rx_sta & 0x3FFF] = ch ;
        (*rx_sta)++;

        if (*rx_sta > (USART1_RX_ISR_MAX_LEN - 1))
            *rx_sta = 0;

        if (ch == 0x0a)
        {
            *rx_sta |= 0x8000;
            ret = true;
        }
    }

    return ret;
}

void DMA1_Channel5_IRQHandler (void)
{
    if (DMA_GetITStatus (DMA1_IT_TC5) != RESET)
    {
        if (usart1_recv_msg_handler (USART1_DMA_RX_BYTE, &usart1_rx_sta, usart1_rx_buf))
        {
            //写入队列
            Message msg;
            memset (&msg, 0, sizeof (msg));
            msg.flag = UART_FRAME_JS;
            msg.len = usart1_rx_sta & 0x3FFF ;
            memcpy (msg.buf, usart1_rx_buf, msg.len);

            osal_Enqueue (&uart_queue, msg);
            //osal_TraverseQueue(&uart_queue);

            //清零结构体
            usart1_rx_sta = 0;
            memset (usart1_rx_buf, 0, sizeof (usart1_rx_buf));
        }
    }

    DMA_ClearITPendingBit (DMA1_IT_TC5);
}

void DMA1_Channel4_IRQHandler (void)
{
    if (DMA_GetFlagStatus (DMA1_FLAG_TC4))
    {
        sys_uart_dma_buf.uart_dma_report = true;
        DMA_ClearFlag (DMA1_FLAG_TC4);
    }
}

int App_Module_format_conver_uint8 (Pdoa_Msg_Hex_Output_t msg, uint8_t* buf)
{
    int index = 0;
    buf[index++] = msg.header;
    buf[index++] = msg.length;
    buf[index++] = msg.gerneal_para.sn;
    buf[index++] = LO_UINT16 (msg.gerneal_para.Addr);
    buf[index++] = HI_UINT16 (msg.gerneal_para.Addr);

    buf[index++] = BREAK_UINT32 (msg.gerneal_para.angle, 0);
    buf[index++] = BREAK_UINT32 (msg.gerneal_para.angle, 1);
    buf[index++] = BREAK_UINT32 (msg.gerneal_para.angle, 2);
    buf[index++] = BREAK_UINT32 (msg.gerneal_para.angle, 3);

    buf[index++] = BREAK_UINT32 (msg.gerneal_para.distance, 0);
    buf[index++] = BREAK_UINT32 (msg.gerneal_para.distance, 1);
    buf[index++] = BREAK_UINT32 (msg.gerneal_para.distance, 2);
    buf[index++] = BREAK_UINT32 (msg.gerneal_para.distance, 3);

    buf[index++] = LO_UINT16 (msg.gerneal_para.user_cmd);
    buf[index++] = HI_UINT16 (msg.gerneal_para.user_cmd);

    buf[index++] = BREAK_UINT32 (* (int*) &msg.gerneal_para.F_Path_Power_Level, 0);
    buf[index++] = BREAK_UINT32 (* (int*) &msg.gerneal_para.F_Path_Power_Level, 1);
    buf[index++] = BREAK_UINT32 (* (int*) &msg.gerneal_para.F_Path_Power_Level, 2);
    buf[index++] = BREAK_UINT32 (* (int*) &msg.gerneal_para.F_Path_Power_Level, 3);

    buf[index++] = BREAK_UINT32 (* (int*) &msg.gerneal_para.RX_Level, 0);
    buf[index++] = BREAK_UINT32 (* (int*) &msg.gerneal_para.RX_Level, 1);
    buf[index++] = BREAK_UINT32 (* (int*) &msg.gerneal_para.RX_Level, 2);
    buf[index++] = BREAK_UINT32 (* (int*) &msg.gerneal_para.RX_Level, 3);

    buf[index++] = LO_UINT16 (msg.gerneal_para.Acc_X);
    buf[index++] = HI_UINT16 (msg.gerneal_para.Acc_X);

    buf[index++] = LO_UINT16 (msg.gerneal_para.Acc_Y);
    buf[index++] = HI_UINT16 (msg.gerneal_para.Acc_Y);

    buf[index++] = LO_UINT16 (msg.gerneal_para.Acc_Z);
    buf[index++] = HI_UINT16 (msg.gerneal_para.Acc_Z);

    buf[index++] = get_Xor_CRC (buf, 2, msg.length);
    buf[index++] = msg.footer;

    return (msg.length + 4);
}



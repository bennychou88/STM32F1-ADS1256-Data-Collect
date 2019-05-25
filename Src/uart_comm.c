/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "usb.h"
#include "gpio.h" 
#include "core_cm3.h"
#include "uart_comm.h"
#include "string.h"
#include "stdio.h"


/*
# ano  
anoӦ��˵��  
����ֲ���޸�˵��  
����1.���ano.h��ano.c�ļ�����Ĺ���  
����2.ano.c ���޸�������A&B����  
���鿴ʵ��demo  
��ano��λ������  
����1.�߼��������ʹ�ܶ��õĹ���֡  
����2.����֡��ʽ�������㶨����������趨  
����3.�趨������Դ����������  
����4.�����ݲ��Σ�ѡ���û����ݣ�������Ӧͨ��  
�� �ߣ�meetwit  
�� �ڣ�2019��3��20��  
*/

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

uart_data_stuc_t uart_data_stuc;


/*
*********************************************************************************************************
* name:  
* function:   
* parameter: Vref : 
* The return value:  NULL
*********************************************************************************************************
*/
static void uart_irq_init( void )
{ 
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE); 
    HAL_UART_Receive_DMA(&huart1,uart_data_stuc.rxbuf,sizeof(uart_data_stuc.rxbuf));
}

/*
*********************************************************************************************************
* name:  ano_report
* function: uint8_t func ������,�Զ��壬0xF1-0xFF; uint8_t*data Ҫ���͵����ݲ���,uint8_t len Ҫ�������ݵĳ��ȣ���λ��byte 
* parameter: Vref : ����������֡ͷ���������Լ�У��Ͳ��ҷ��͵�ano��λ��
* The return value:  NULL
*********************************************************************************************************
*/
static void ano_report(uint8_t func, uint8_t *data, uint8_t len)
{
    uint8_t i,cnt=0,sum = 0;
    
    uart_data_stuc.txbuf[len+5]=0; //У��������
    
    uart_data_stuc.txbuf[cnt++]=0xAA;  //֡ͷ
    uart_data_stuc.txbuf[cnt++]=0x05;  //S_ADDR    
    uart_data_stuc.txbuf[cnt++]=0xAF;  //D_ADDR    
    uart_data_stuc.txbuf[cnt++]=func;   //������
    uart_data_stuc.txbuf[cnt++]=len;  //���ݳ���
   
    for(i=0;i<len;i++)
        uart_data_stuc.txbuf[cnt++]=data[i];             //��������
    
    for(i=0;i<cnt;i++)
        sum += uart_data_stuc.txbuf[i];
    
    uart_data_stuc.txbuf[cnt++]=sum;
         
    uart_data_stuc.txlen = cnt;
    
    HAL_UART_Transmit_DMA(&huart1,uart_data_stuc.txbuf,uart_data_stuc.txlen);  //�������ݵ�����
    //for(i=0;i<len+5;i++)usart_send_char(send_buf[i]);     //�������ݵ�����
}

/*
*********************************************************************************************************
* name:  ano_send
* function: uint8_t func ������,�Զ��壬0xF1-0xFF; uint8_t * sp	��ʾ���ݵĵ�ַ,uint8_t sizenum 	�������ͳ���;uint8_t len Ҫ�������ݵĳ��ȣ���λ��byte 
* parameter: Vref : ����������ݷ��͵���������Ҫ������ѡ�ö�Ӧ���ȵĽ���
* The return value:  NULL
*********************************************************************************************************
*/ 
static void ano_send(uint8_t fun,uint8_t *sp,uint8_t sizenum,uint8_t len)
{
    uint8_t tbuf[80]={0},i,j; 
    uint8_t *p;
    for(i=0;i<len/sizenum;i++){   		 
        p=sp+sizenum*i;      
        for(j=0;j<sizenum;j++){
            tbuf[j+4*i]=(uint8_t)(*(p+3-j)); 		 
        }
    }
    ano_report(fun,tbuf,len);  
}    

/*
*********************************************************************************************************
* name:  
* function:   
* parameter: Vref : 
* The return value:  NULL
*********************************************************************************************************
*/
void uart_send_msg( uint8_t *msg, uint8_t size, uint8_t num )
{  
    ano_send(0xf1,(uint8_t *)msg,size,num);
}

/*
*********************************************************************************************************
  * @brief  Rx Transfer completed callbacks.
* @param  huart: pointer to a UART_HandleTypeDef structure that contains
*                the configuration information for the specified UART module.
* @retval None   
*********************************************************************************************************
*/
void uart_idle_rxcb( void )
{  
    if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) != RESET)){ 
        __HAL_UART_CLEAR_IDLEFLAG(&huart1); 
        HAL_UART_DMAStop(&huart1); 
        uint16_t temp = hdma_usart1_rx.Instance->CNDTR; 
        uart_data_stuc.rxlen =  sizeof(uart_data_stuc.rxbuf) - temp; 
        
        HAL_UART_Receive_DMA(&huart1,uart_data_stuc.rxbuf,sizeof(uart_data_stuc.rxbuf));
    }  
}

/*
*********************************************************************************************************
* name:  
* function:   
* parameter: Vref : 
* The return value:  NULL
*********************************************************************************************************
*/
void uart_init( void )
{ 
    uart_irq_init();
}
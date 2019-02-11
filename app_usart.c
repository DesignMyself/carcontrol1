/*
 * File      : app_uart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 *2017-12-15      DQL          the first version 
 */

#include <rthw.h>
#include <rtthread.h>
#include <math.h>
#include "HDMI.H"
#include "app_uSart.h"

/* ����1�����¼���־ */
#define UART1_RX_EVENT (1 << 0)


/* ����2�����¼���־ */
#define UART2_RX_EVENT (1 << 1)

/* ����3�����¼���־ */
#define UART3_RX_EVENT (1 << 2)

/* �¼����ƿ� */
static struct rt_event event1;
static struct rt_event event2;//����2���տ����¼�
static struct rt_event event3;//����2���տ����¼�

/* ����1�豸��� */
static rt_device_t uart_device1 = RT_NULL;//���ڵ�һ������ USART1

/* ����1�豸��� */
static rt_device_t uart_device2 = RT_NULL;//���ڵ�һ������ USART2

/* ����1�豸��� */
static rt_device_t uart_device3 = RT_NULL;//���ڵ�һ������ USART3
/*******���ڰ�ASCIIת��Ϊʮ���Ƶ�����
j��ʾ��������ݣ�j��ʾ**************/
float shiji(uint8_t i,uint8_t j)
{
	uint8_t i1=0;
	float kk1=1.0f;
	for(i1=j-1;i1>0;i1--)
	{
		kk1*=10.0f;
	}
	kk1*=i;
	return kk1;
}
/*******���ڰ�ASCIIת��Ϊʮ���Ƶ�С����i��ʾ������������飬j��ʾ��С�����λ��**************/
float Ascii_Xiaoshu(uint8_t i,uint8_t j)
{
	uint8_t i1=0;
	float kk1=1.0f;
	for(i1=0;i1<j;i1++)
	{
		kk1/=10.0f;
	}
	kk1*=i;
	return kk1;
}


/***********����ASCII��ת��Ϊ����******************/
float Get_AutoRun(uint8_t *kk,uint8_t j)
{
	uint8_t i=0;
	uint8_t b=0;
	float total=0.0f;
//	if(j==0)
//	{
//		return() ;//��ֻ��11������ʱ����ʾû���������룬���ݱ��ֲ���
//	}else{
		for(i=0;i<j;i++)//EE B1 11 00 00 00 09 11 00 FF FC FF FF 
		{
			
			if(kk[i]==0x2e)
			{
				
				b=i;
				break;
			}else{
				
				b=0;
			}
		}
			if(b>0)//������С����ʱ
			{
				
				for(i=0;i<b;i++)
				{
					
					total+=shiji(kk[i]-0x30,b-i);//�����������,b��ʾС����λ�õ����꣬
					
//					uart1_putchar(total);
				}
				
				
				for(i=b+1;i<j;i++)
				{
					
					total+=Ascii_Xiaoshu(kk[i]-0x30,i-b);
					
				}
				
			}
		if(b==0)
		{
				for(i=0;i<j;i++)
				{
					total+=(float)(shiji(kk[i]-0x30,j-i));//�����������
			}
		}
		
	return total;
		
}
	
	
/* �ص����� ��������1�����ж�ʱ���������ص�����*/
static rt_err_t uart1_intput(rt_device_t dev, rt_size_t size)
{
    /* �����¼� */
    rt_event_send(&event1, UART1_RX_EVENT);
    return RT_EOK;
}
/* �ص����� ��������2�����ж�ʱ���������ص�����*/
static rt_err_t uart2_intput(rt_device_t dev, rt_size_t size)
{
    /* �����¼� */
    rt_event_send(&event2, UART2_RX_EVENT);
    return RT_EOK;
}
/* �ص����� ��������3�����ж�ʱ���������ص�����*/
static rt_err_t uart3_intput(rt_device_t dev, rt_size_t size)
{
    /* �����¼� */
    rt_event_send(&event3, UART3_RX_EVENT);
    return RT_EOK;
}

/***********����1��ȡ8λ����***************/
rt_uint8_t uart1_getchar(void)
{
    rt_uint32_t e;
    rt_uint8_t ch;

    /* ��ȡ1�ֽ����� */
    while (rt_device_read(uart_device1, 0, &ch, 1) != 1)
    {
         /* �����¼� */
        rt_event_recv(&event1, UART1_RX_EVENT,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &e);
				

			
		}
		return ch;
}

/***********����2��ȡ8λ����***************/
rt_uint8_t uart2_getchar(void)
{
    rt_uint32_t e;
    rt_uint8_t ch;

    /* ��ȡ1�ֽ����� */
    while (rt_device_read(uart_device2, 0, &ch, 1) != 1)
    {
         /* �����¼� */
        rt_event_recv(&event2, UART2_RX_EVENT,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &e);
				

			
		}
		return ch;
}
/***********����3��ȡ8λ����***************/
rt_uint8_t uart3_getchar(void)
{
    rt_uint32_t e;
    rt_uint8_t ch1;

    /* ��ȡ1�ֽ����� */
    while (rt_device_read(uart_device3, 0, &ch1, 1) != 1)
    {
         /* �����¼� */
        rt_event_recv(&event3, UART3_RX_EVENT,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &e);
				

			
		}
		return ch1;
}

/***********����1����8λ����***************/
void uart1_putchar(const rt_uint8_t c)
{
	
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;

	
    do
    {
        len = rt_device_write(uart_device1, 0, &c, 1);
        timeout++;
    }
    while (len != 1 && timeout < 500);
}

/***********����2����8λ����***************/
void uart2_putchar(const rt_uint8_t c)
{
		
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;

    do
    {
        len = rt_device_write(uart_device2, 0, &c, 1);
        timeout++;
    }
    while (len != 1 && timeout < 500);
}
/***********����3����8λ����***************/
void uart3_putchar(const rt_uint8_t c)
{
	
	 rt_size_t len = 0;
    rt_uint32_t timeout = 0;

    do
    {
        len = rt_device_write(uart_device3, 0, &c, 1);
        timeout++;
    }
    while (len != 1 && timeout < 500);
	
}

/***********����1�����ַ���***************/
void uart1_putstring(const rt_uint8_t *s)
{
    while(*s)
    {
        uart1_putchar(*s++);
    }
}
/***********����1�����ַ���***************/
void uart2_putstring( char *s)
{
    while(*s)
    {
        uart2_putchar(*s++);
    }
}

/***********����1�����ַ���***************/
void uart3_putstring( char *s)
{
    while(*s)
    {
        uart3_putchar(*s++);
    }
}

/***********�򿪴���1***************/
rt_err_t uart1_open(const char *name)
{
    rt_err_t res;

    /* ����ϵͳ�еĴ����豸 */
    uart_device1= rt_device_find(name);   
    /* ���ҵ��豸����� */
    if (uart_device1 != RT_NULL)
    {   
       
        res = rt_device_set_rx_indicate(uart_device1, uart1_intput);//rt_device_set_rx_indicate�ǵ������յ�����ʱ��֪ͨ�ϲ�Ӧ���߳������ݵ��� �������ûص�����uart_input(������û��Լ����壩
        /* ��鷵��ֵ */
        if (res != RT_EOK)
        {
            rt_kprintf("set %s rx indicate error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* ���豸���Կɶ�д���жϷ�ʽ */
        res = rt_device_open(uart_device1, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX );       
        /* ��鷵��ֵ */
        if (res != RT_EOK)
        {
            rt_kprintf("open %s device error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* ��ʼ���¼����� */
        rt_event_init(&event1, "event1", RT_IPC_FLAG_FIFO); //�����¼������rt_event_createһ������ʼ��һ���¼�����
    }
    else
    {
        rt_kprintf("can't find %s device.\n",name);
        return -RT_ERROR;
    }

    return RT_EOK;
}
/***********�򿪴���2***************/
rt_err_t uart2_open(const char *name)
{
    rt_err_t res;

    /* ����ϵͳ�еĴ����豸 */
    uart_device2= rt_device_find(name);   
    /* ���ҵ��豸����� */
    if (uart_device2 != RT_NULL)
    {   
       
        res = rt_device_set_rx_indicate(uart_device2, uart2_intput);//rt_device_set_rx_indicate�ǵ������յ�����ʱ��֪ͨ�ϲ�Ӧ���߳������ݵ��� �������ûص�����uart_input(������û��Լ����壩
        /* ��鷵��ֵ */
        if (res != RT_EOK)
        {
            rt_kprintf("set %s rx indicate error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* ���豸���Կɶ�д���жϷ�ʽ */
        res = rt_device_open(uart_device2, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX );       
        /* ��鷵��ֵ */
        if (res != RT_EOK)
        {
            rt_kprintf("open %s device error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* ��ʼ���¼����� */
        rt_event_init(&event2, "event2", RT_IPC_FLAG_FIFO); //�����¼������rt_event_createһ��
    }
    else
    {
        rt_kprintf("can't find %s device.\n",name);
        return -RT_ERROR;
    }

    return RT_EOK;
}/***********�򿪴���1***************/
rt_err_t uart3_open(const char *name)
{
    rt_err_t res;

    /* ����ϵͳ�еĴ����豸 */
    uart_device3= rt_device_find(name);   
    /* ���ҵ��豸����� */
    if (uart_device3 != RT_NULL)
    {   
       
        res = rt_device_set_rx_indicate(uart_device3, uart3_intput);//rt_device_set_rx_indicate�ǵ������յ�����ʱ��֪ͨ�ϲ�Ӧ���߳������ݵ��� �������ûص�����uart_input(������û��Լ����壩
        /* ��鷵��ֵ */
        if (res != RT_EOK)
        {
            rt_kprintf("set %s rx indicate error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* ���豸���Կɶ�д���жϷ�ʽ */
        res = rt_device_open(uart_device3, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX );       
        /* ��鷵��ֵ */
        if (res != RT_EOK)
        {
            rt_kprintf("open %s device error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* ��ʼ���¼����� */
        rt_event_init(&event3, "event3", RT_IPC_FLAG_FIFO); //�����¼������rt_event_createһ��
    }
    else
    {
        rt_kprintf("can't find %s device.\n",name);
        return -RT_ERROR;
    }

    return RT_EOK;
}


#ifndef __HMDI_H__
#define __HMDI_H__
#include <rtthread.h>
#include <rtdevice.h>
void TX_8(uint8_t uart_value,uint8_t P1); //���͵����ֽ�

void TX_16(uint8_t uart_value,uint16_t P1);   //����16λ����
/********��ͨ������********/
void HDMI0_1(char *str1,uint32_t num);
void HDMI0_2(uint8_t uart_value,char *str1,float num);
void HDMI_0_2(char *str1,uint32_t num);
void HDMI_val(uint8_t uart_value,char *str1,uint8_t num);
void HDMI_str(char *str1,char *str2);
void HMI_End(uint8_t uart_value);
void uart_putchar_f(uint8_t uart_value,uint8_t value);
void Value_Asii(uint8_t uart_value,float value);

/*****���ݴ�ʴ�����***************/
void SetProgressValue(rt_uint16_t screen_id,rt_uint16_t control_id,char *str1,uint8_t uart_value);//����Ļ������ֵ
void SetProgress_mess_value(rt_uint16_t screen_id,rt_uint16_t control_id,char *str,float value,uint8_t uart_value);//����Ļ������Ϣ+��ֵ
void BEGIN_CMD(uint8_t uart_value);
void Send_Icon(rt_uint16_t screen_id,rt_uint16_t control_id,rt_uint8_t zhen_id,uint8_t uart_value);//������ʾͼƬ����
void Send_Angle(rt_uint16_t screen_id,rt_uint16_t control_id,uint32_t value,uint8_t uart_value);//������ʾ�Ǳ����
#endif

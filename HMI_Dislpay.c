#include "HDMI.H"
#include "app_uSart.h"
#include "math.h"
/******�����������ַ�********/
void HMI_End(uint8_t uart_value)
{
	uart_putchar_f(uart_value,0xff);
	uart_putchar_f(uart_value,0xff);
	uart_putchar_f(uart_value,0xff);
}


/********************ѡ�񴮿ڷ��͵����ַ�****************/
void uart_putchar_f(uint8_t uart_value,uint8_t value)
{
	if(uart_value==3)
				{
					uart3_putchar(value);
				}
			if(uart_value==1)
				{
						uart1_putchar(value);
				}
	
	
}

/********************������Ľ��տ�ʼָ��****************/
void BEGIN_CMD(uint8_t uart_value) 
{
	if(uart_value==3)
	{
		uart3_putchar(0XEE);
	}
	else if(uart_value==1)
	{
		uart1_putchar(0XEE);
		
	}
}
/********************ѡ�񴮿ڷ��͵����ַ�****************/
void TX_8(uint8_t uart_value,uint8_t P1)
{
		if(uart_value==3)
	{
		uart3_putchar((P1)&0xFF);
	}
	else if(uart_value==1)
	{
		uart1_putchar((P1)&0xFF);
		
	}
	 
}

/*************ѡ�񴮿ڷ����ַ���**********/
void send_string(uint8_t uart_value,char *str2)
{
	
		if(uart_value==3)
				{
					rt_kprintf(str2);
				}
			if(uart_value==1)
				{
						uart1_putstring(str2);
				}
	
}

/********************ѡ�񴮿ڷ���2���ֽ�****************/
void TX_16(uint8_t uart_value,uint16_t P1)
{
	
	TX_8(uart_value,(P1)>>8);
	TX_8(uart_value,P1);
	
}
void TX_32(uint8_t uart_value,uint32_t P1)
{
	
	TX_16(uart_value,(P1)>>16);
	TX_16(uart_value,(P1)&0xFFFF);
	
	
}

/********��������Ϊ0.01*************/
void HDMI0_2(uint8_t uart_value,char *str1,float num)
{
	num=(uint32_t)num;
	send_string(uart_value,str1);
	//uart_putchar_f(uart_value,34);//˫��������ASII�룬������Ϊʮ����
	Value_Asii(uart_value,num);
	//uart_putchar_f(uart_value,34);//˫��������ASII��
	HMI_End(uart_value);
}


void HDMI_val(uint8_t uart_value,char *str1,uint8_t num)
{
	send_string(uart_value,str1);
	uart_putchar_f(uart_value,num);
	HMI_End(uart_value);
}



void END_CMD(uint8_t uart_value)
{
	TX_8(uart_value,0XFF);
	TX_8(uart_value,0XFC);
	TX_8(uart_value,0XFF);
	TX_8(uart_value,0XFF);
	
	
}

/*****************����ֵתΪASII****/
void Value_Asii(uint8_t uart_value,float value)
{
	uint8_t zero_sign=0;
	uint8_t ch1[12];
	uint8_t i=0;
	int32_t num;
	uint16_t num1;
	num=(int32_t)fabs(value);

			num1=(value-num)*100;
			ch1[0]='-';
			ch1[1]=num/1000000+48;
			ch1[2]=num%1000000/100000+48;
			ch1[3]=num%1000000%100000/10000+48;
			ch1[4]=num%1000000%100000%10000/1000+48;
			ch1[5]=num%1000000%100000%10000%1000/100+48;
			ch1[6]=num%1000000%100000%10000%1000%100/10+48;
			ch1[7]=num%1000000%100000%10000%1000%100%10+48;
			ch1[8]=46;
			ch1[9]=num1/10+48;
			ch1[10]=num1%10+48;
			
			if(uart_value==3)
				{
					if(value<0)
					{
						uart3_putchar(ch1[0]);
					}
						for(i=1;i<10;i++)
					{
						if(ch1[i]!=48)
						{
							zero_sign=1;
							
						}
							if(zero_sign==1)
							{
								uart3_putchar(ch1[i]);
							}
					}
				}
			if(uart_value==1)
				{
					if(value<0)
					{
						uart1_putchar(ch1[0]);
					}
						for(i=1;i<10;i++)
					{
							if(ch1[i]!=48)
							{
								zero_sign=1;
								
							}
							if(zero_sign==1)
							{
								uart1_putchar(ch1[i]);
							}
					}
				}
			if(uart_value==2)
				{
					if(value<0)
					{
						uart2_putchar(ch1[0]);
					}
						for(i=1;i<10;i++)
					{
								if(ch1[i]!=48)
							{
								zero_sign=1;
								
							}
							if(zero_sign==1)
							{
								uart2_putchar(ch1[i]);
							}
					}
				}
}


/***************�����ʾ��Ϣ************/
void SetProgressValue(rt_uint16_t screen_id,rt_uint16_t control_id,char *str,uint8_t uart_value)
{
	BEGIN_CMD(uart_value);
	TX_8(uart_value,0xB1);
	TX_8(uart_value,0x10);
	TX_16(uart_value,screen_id);
	TX_16(uart_value,control_id);
	send_string( uart_value,str);
	END_CMD(uart_value);
}
/***************�����ʾ���Ӻ���ֵ************/
void SetProgress_mess_value(rt_uint16_t screen_id,rt_uint16_t control_id,char *str1,float value,uint8_t uart_value)
{
//	char * val;
//	*val=(char)value;
	BEGIN_CMD(uart_value);
	TX_8(uart_value,0xB1);
	TX_8(uart_value,0x10);
	TX_16(uart_value,screen_id);
	TX_16(uart_value,control_id);
	send_string( uart_value,str1);
	Value_Asii(uart_value,value);
	//rt_kprintf(val);
	END_CMD(uart_value);
}



/*************��Ļ����ʾͼ��*******/
void Send_Icon(rt_uint16_t screen_id,rt_uint16_t control_id,rt_uint8_t zhen_id,uint8_t uart_value)
{
	BEGIN_CMD(uart_value);//EE
	TX_8(uart_value,0xB1);
	TX_8(uart_value,0x23);
	TX_16(uart_value,screen_id);
	TX_16(uart_value,control_id);
	TX_8(uart_value,zhen_id);
	END_CMD(uart_value);
	
}
/*************��Ļ����ʾͼ��*******/
void Send_Angle(rt_uint16_t screen_id,rt_uint16_t control_id,uint32_t value,uint8_t uart_value)
{
	BEGIN_CMD(uart_value);//EE
	TX_8(uart_value,0xB1);
	TX_8(uart_value,0x10);
	TX_16(uart_value,screen_id);
	TX_16(uart_value,control_id);
	TX_32(uart_value,value);
	END_CMD(uart_value);
	
}

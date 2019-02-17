#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/watchdog.h>
#define IWDG_DEVICE_NAME    "iwg"    /* ���Ź��豸���� */

 rt_device_t wdg_dev;         /* ���Ź��豸��� */

// void idle_hook(void)
//{
//    /* �ڿ����̵߳Ļص�������ι�� */
//    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
//    rt_kprintf("feed the dog!\n ");
//}

 int iwdg_sample(void)
{
    rt_err_t ret = RT_EOK;
//    rt_uint32_t timeout = 1000;    /* ���ʱ�� */
 
    /* �����豸���Ʋ��ҿ��Ź��豸����ȡ�豸��� */
    wdg_dev = rt_device_find(IWDG_DEVICE_NAME );
    if (!wdg_dev)
    {
        rt_kprintf("find %s failed!\n", IWDG_DEVICE_NAME );
        return RT_ERROR;
    }
    /* ��ʼ���豸 */
    ret = rt_device_init(wdg_dev);
    if (ret != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n",IWDG_DEVICE_NAME );
        return RT_ERROR;
    }
//    /* ���ÿ��Ź����ʱ�� */
//    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
//    if (ret != RT_EOK)
//    {
//        rt_kprintf("set %s timeout failed!\n", IWDG_DEVICE_NAME );
//        return RT_ERROR;
//    }
		rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE , NULL);
		
    /* ���ÿ����̻߳ص����� */
    //rt_thread_idle_sethook(idle_hook);
		
    return ret;
}
#include <stdio.h>
#include <string.h>
#include <unistd.h>//system file
#include <stdlib.h>
#include <drv_gpio.h>
#include "lcd.h"
#include "board_config.h"
#include <rtthread.h>
#include <rt_ai.h>
#include <rt_ai_log.h>
#include <rt_ai_test_model.h>
/******function related definition******/
extern uint8_t data;
extern rt_device_t serial;
extern rt_uint8_t lockHelper;
rt_ai_uint16_t fps=0;
rt_uint8_t personCount=0;
rt_uint8_t fitMode=1;
//rt_uint8_t lockFlag=0;
rt_uint8_t assureFlag=0;
rt_uint8_t firstFlag=1;
rt_uint8_t validFlag=0;
extern void unlock();
/******function related definition done******/
typedef struct dAllSturct{//结构体，d_all为距离平方和，flag为该组datazoom的属性值
    rt_uint16_t d_all;
    rt_uint16_t flag;
} dlls;

static int ai_done(void *ctx)
{
    *((uint32_t*)ctx)= 1;
    return 0;
}

/*callback function*/
volatile uint32_t g_dvp_finish_flag;
rt_err_t camera_cb(rt_device_t dev, size_t size)
{
    g_dvp_finish_flag = 1;
    return RT_EOK;
}
/******AI related definition******/
rt_int8_t person;
rt_int8_t reflective;
rt_int8_t background;
rt_int8_t *datazoom;
dlls *store;
rt_int8_t AI_run_flag;
/******AI related definition done******/

void change(dlls *a,dlls *b)
{
    dlls temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

uint8_t KNN_dataProcess(rt_int8_t per,rt_int8_t ref,rt_int8_t back,rt_uint8_t N)
{
    int i, d_per, d_ref, d_back, sum=0;
    rt_uint16_t d_all;
    rt_uint8_t result[3];
    rt_uint8_t iX4;
    for(i=0;i<data;i++)
    {
        iX4 = i*4;
        //rt_kprintf("\n");
        d_per = (per-datazoom[iX4+1])*(per-datazoom[iX4+1]);
        d_ref = (ref-datazoom[iX4+2])*(per-datazoom[iX4+2]);
        d_back = (back-datazoom[iX4+3])*(per-datazoom[iX4+3]);
        d_all = d_per + d_ref + d_back;
        if(i<=2)
            {
            store[i].d_all = d_all;
            store[i].flag = datazoom[iX4];
            if(i==2)
            {
                store[0].d_all>store[1].d_all ? (store[0].d_all>store[2].d_all ? change(&store[0], &store[2]) : 1) : (store[1].d_all>store[2].d_all ? change(&store[1],&store[2]) : 1);//got the max
                store[0].d_all>store[1].d_all ? change(&store[0],&store[1]) : 1;

            }
            }

        else
        {
            if(d_all < store[0].d_all)
            {
                store[2] = store[1];
                store[1] = store[0];
                store[0].d_all = d_all;
                store[0].flag = datazoom[iX4];
            }
            else {
                    if(d_all < store[1].d_all)
                    {
                    store[2] = store[1];
                    store[1].d_all = d_all;
                    store[1].flag = datazoom[iX4];
                    }
                    else {
                          if(d_all < store[2].d_all)
                          {
                              store[2].d_all = d_all;
                              store[2].flag = datazoom[iX4];
                          }
                         }
                 }

        }
    }
    //rt_kprintf("%d %d %d\n",store[0].flag,store[1].flag,store[2].flag);
    sum = store[0].flag + store[1].flag + store[2].flag;
    if (sum >= 2) return 1;
    else return 0;

}
extern void key_init();
extern void myUart_init();
void _thread_Predict(void *params)
{
    rt_kprintf("Hello SSD thread\n");
    volatile uint32_t g_ai_done_flag;
    static uint8_t *kpu_image;//Operational image's address
    static uint8_t *display_image;//Image displayed
    static uint8_t *test_img;//for test
    static float *pred;
    rt_uint8_t str[5];
    uint8_t finalResult = 0;
    rt_ai_t mobileNet = NULL;
    /*give space to KNN-data*/
     datazoom = rt_malloc(50*(3+1));
     store = rt_malloc(sizeof(dlls)*3);
     rt_memset(store, 0, sizeof(dlls)*3);
     rt_memset(datazoom, 0, 50*4);
     key_init();
     myUart_init();
    /*LCD INIT*/
    rt_kprintf("LCD init\n");
    lcd_init();
    lcd_clear(BLACK);

    /*read a picture*/
    display_image = rt_malloc((240 * 320 * 2)+(240 * 320 * 3));
    kpu_image = display_image + (240 * 320 * 2); //display 240*320*2 calculate 240*320*3
    rt_device_t camera_dev = rt_device_find(CAMERA);
    if(!camera_dev){
        rt_kprintf("find camera err!\n");
        return -1;
    };
    /* INIT CAMERA AND OPEN IT*/
    rt_device_init(camera_dev);
    rt_device_open(camera_dev, RT_DEVICE_OFLAG_RDWR);
    rt_device_set_rx_indicate(camera_dev, camera_cb);
    /*init model */
    mobileNet = rt_ai_find(RT_AI_TEST_MODEL_NAME);
    RT_ASSERT(mobileNet);
    rt_kprintf("im going to init\n");
    if (rt_ai_init(mobileNet,(rt_ai_buffer_t *)kpu_image) != 0)
    {
        rt_kprintf("\nmodel init error\n");
        while (1) {};
    }
    rt_kprintf("rt_ai_init complete..\n");

/***********************Circular judgment************************/
    while(1){
                g_dvp_finish_flag = 0;
                rt_device_read(camera_dev,0, display_image,0);
                while (g_dvp_finish_flag == 0){};
                /*run detect*/
                g_ai_done_flag= 0;
                /*********AI run detection********/

                if(rt_ai_run(mobileNet,ai_done,&g_ai_done_flag) != 0)
                {
                    ai_log("rtak run fail!\n");
                    while (1){}  ;
                }
                while(!g_ai_done_flag){};
                pred = (float*)rt_ai_output(mobileNet, 0);
                person=(rt_int8_t)(100*pred[0]);
                reflective=(rt_int8_t)(100*pred[1]);
                background=(rt_int8_t)(100*pred[2]);

                /*********AI run detection done********/
                /*********Subsequent treatment********/
                __pixel_reversal((uint16_t *) display_image, 320, 240);
                lcd_draw_picture(0,0,240,320, (uint32_t *)display_image);

                finalResult = KNN_dataProcess(person,reflective,background,3);
                if(finalResult==1)
                {
                       lcd_draw_string(10, 10, "Go through~",GREEN);
                       if(!fitMode)
                       {
                           if(assureFlag&&firstFlag)//only when it is the first time and the pin value is low
                           {
                               personCount++;
                               rt_device_write(serial, NULL, &personCount, 1);
                               firstFlag = 0;
                               validFlag = 1;
                           }
                           if(validFlag)
                           {
                               if(!rt_pin_read(23))
                               {
                                   assureFlag = 0;
                                   lockHelper = 0;
                                   firstFlag = 1;
                                   validFlag = 0;
                               }
                           }
                           unlock();
                       }

                }
                else
                {
                       lcd_draw_string(10, 10, "Please wear reflective clothes!",RED);
                 }

                /***fps test***/
                //rt_kprintf("%d\n",fps++);
                //rt_kprintf("%d\n",personCount);
                /*********Subsequent treatment done********/
            }
/***********************Circular judgment done************************/
}





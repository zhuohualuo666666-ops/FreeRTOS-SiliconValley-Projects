#include "freertos_demo.h"
/* freertos相关的头文件，必须的 */
#include "FreeRTOS.h"
#include "task.h"
/* 需要用到的其他头文件 */
#include "LED.h"
#include "Key.h"
#include "stdio.h"
/* 启动任务的配置 */
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;
void start_task(void *pvParameters);

/* 任务1的配置 */
#define TASK1_STACK 128
#define TASK1_PRIORITY 2
TaskHandle_t task1_handle;
void task1(void *pvParameters);


/**
 * @description: 启动FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{
    /* 1.创建一个启动任务 */
    xTaskCreate((TaskFunction_t)start_task,               // 任务函数的地址
                (char *)"start_task",                     // 任务名字符串
                (configSTACK_DEPTH_TYPE)START_TASK_STACK, // 任务栈大小，默认最小128，单位4字节
                (void *)NULL,                             // 传递给任务的参数
                (UBaseType_t)START_TASK_PRIORITY,         // 任务的优先级
                (TaskHandle_t *)&start_task_handle);      // 任务句柄的地址

    /* 2.启动调度器:会自动创建空闲任务 */
    vTaskStartScheduler();
}

/**
 * @description: 启动任务：用来创建其他Task
 * @param {void} *pvParameters
 * @return {*}
 */
void start_task(void *pvParameters)
{
    /* 进入临界区:保护临界区里的代码不会被打断 */
    taskENTER_CRITICAL();

    /* 创建3个任务 */
    xTaskCreate((TaskFunction_t)task1,
                (char *)"task1",
                (configSTACK_DEPTH_TYPE)TASK1_STACK,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIORITY,
                /*(TaskHandle_t *)*/&task1_handle);

    /* 启动任务只需要执行一次即可，用完就删除自己 */
    vTaskDelete(NULL);

    /* 退出临界区 */
    taskEXIT_CRITICAL();
}

/**
 * @description: 任务一：实现LED1每500ms闪烁一次
 * @param {void} *pvParameters
 * @return {*}
 */
void task1(void *pvParameters)
{
//	printf("task1运行...\r\n");

	uint8_t key=0;
    while (1)
    {	
		key=Key_Detect();
		/**/
		if(key == KEY1_PRESS)
		{
		printf(">>>>关中断...\r\n");
		portDISABLE_INTERRUPTS();
		}
		else if(key == KEY2_PRESS)
		{
		printf(">>>>开中断...\r\n");

		portENABLE_INTERRUPTS();
		}
		HAL_Delay(500);
//        vTaskDelay(500);
    }
}


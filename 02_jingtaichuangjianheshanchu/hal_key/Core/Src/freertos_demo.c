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
StackType_t start_task_stack[START_TASK_STACK]; // 静态任务的任务栈，以数组形式存储
StaticTask_t start_task_tcb;                    // 静态任务的TCB结构体类型
void start_task(void *pvParameters);

/* 任务1的配置 */
#define TASK1_STACK 128
#define TASK1_PRIORITY 2
TaskHandle_t task1_handle;
StackType_t task1_stack[TASK1_STACK]; // 静态任务的任务栈，以数组形式存储
StaticTask_t task1_tcb;               // 静态任务的TCB结构体类型
void task1(void *pvParameters);

/* 任务2的配置 */
#define TASK2_STACK 128
#define TASK2_PRIORITY 3
TaskHandle_t task2_handle;
StackType_t task2_stack[TASK2_STACK]; // 静态任务的任务栈，以数组形式存储
StaticTask_t task2_tcb;               // 静态任务的TCB结构体类型
void task2(void *pvParameters);

/* 任务3的配置 */
#define TASK3_STACK 128
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle;
StackType_t task3_stack[TASK3_STACK]; // 静态任务的任务栈，以数组形式存储
StaticTask_t task3_tcb;               // 静态任务的TCB结构体类型
void task3(void *pvParameters);

/* ======================= 静态创建方式，需要手动指定2个特殊任务的资源=============================== */
/* 空闲任务的配置 */
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE]; // 静态任务的任务栈，以数组形式存储
StaticTask_t idle_task_tcb;                            // 静态任务的TCB结构体类型

/* 软件定时器任务的配置 */
StackType_t timer_task_stack[configTIMER_TASK_STACK_DEPTH]; // 静态任务的任务栈，以数组形式存储
StaticTask_t timer_task_tcb;                            // 静态任务的TCB结构体类型

/* 分配空闲任务的资源 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &idle_task_tcb;
    *ppxIdleTaskStackBuffer = idle_task_stack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* 分配软件定时器任务的资源 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &timer_task_tcb;
    *ppxTimerTaskStackBuffer = timer_task_stack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/**
 * @description: 启动FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{
    /* 1.创建一个启动任务 */
    start_task_handle = xTaskCreateStatic(
        (TaskFunction_t)start_task,       // 任务函数的地址
        (char *)"start_task",             // 任务名字符串
        (uint32_t)START_TASK_STACK,       // 任务栈大小，默认最小128，单位4字节
        (void *)NULL,                     // 传递给任务的参数
        (UBaseType_t)START_TASK_PRIORITY, // 任务的优先级
        (StackType_t *)start_task_stack,  // 我们创建的任务栈（数组存储）的地址
        (StaticTask_t *)&start_task_tcb   // 我们创建的tcb的地址
    );

    /* 2.启动调度器:会自动创建空闲任务和软件定时器（如果开启），
       静态创建的方式需要去实现2个分配资源的接口函数 */
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

    /* 使用静态创建3个任务 */
    task1_handle = xTaskCreateStatic(
        (TaskFunction_t)task1,
        (char *)"task1",
        (configSTACK_DEPTH_TYPE)TASK1_STACK,
        (void *)NULL,
        (UBaseType_t)TASK1_PRIORITY,
        (StackType_t *)task1_stack, // 我们创建的任务栈（数组存储）的地址
        (StaticTask_t *)&task1_tcb  // 我们创建的tcb的地址
    );

    task2_handle = xTaskCreateStatic(
        (TaskFunction_t)task2,
        (char *)"task2",
        (configSTACK_DEPTH_TYPE)TASK2_STACK,
        (void *)NULL,
        (UBaseType_t)TASK2_PRIORITY,
        (StackType_t *)task2_stack, // 我们创建的任务栈（数组存储）的地址
        (StaticTask_t *)&task2_tcb  // 我们创建的tcb的地址
    );

    task3_handle = xTaskCreateStatic(
        (TaskFunction_t)task3,
        (char *)"task3",
        (configSTACK_DEPTH_TYPE)TASK3_STACK,
        (void *)NULL,
        (UBaseType_t)TASK3_PRIORITY,
        (StackType_t *)task3_stack, // 我们创建的任务栈（数组存储）的地址
        (StaticTask_t *)&task3_tcb  // 我们创建的tcb的地址
    );

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

    while (1)
    {
        printf("task1运行...\r\n");
        LED_Toggle(LED1_Pin);
        vTaskDelay(500);
    }
}

/**
 * @description: 任务二：实现LED2每500ms闪烁一次
 * @param {void} *pvParameters
 * @return {*}
 */
void task2(void *pvParameters)
{
    while (1)
    {
        printf("task2运行...\r\n");
        LED_Toggle(LED2_Pin);
        vTaskDelay(500);
    }
}

/**
 * @description: 任务三：判断按键KEY1是否按下，按下则删掉task1
 * @param {void} *pvParameters
 * @return {*}
 */
void task3(void *pvParameters)
{
    uint8_t key = 0;
    while (1)
    {
        printf("task3运行...\r\n");
        key = Key_Detect();
        if (key == KEY1_PRESS)
        {
            /* 判断是否已经删过，避免重复执行删除 */
            if (task1_handle != NULL)
            {
                /* key1按下了，删除task1 */
                printf("执行删除Task1....\r\n");
                vTaskDelete(task1_handle);
                task1_handle = NULL;
            }
        }
        vTaskDelay(500);
        // HAL_Delay(500); //这种delay不会让任务进入阻塞态，不会让出cpu使用权
    }
}

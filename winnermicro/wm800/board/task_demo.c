/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Description: Provide a task example.
 */

#include "task_demo.h"
#include "stdio.h"
#include "los_task.h"
#include "utils_file.h"

#define TASK1_PRIORITY        6
#define TASK2_PRIORITY        7

VOID TaskSampleEntry2(VOID)
{
    while (1) {
#if 1
        UINT64 cycle = LOS_SysCycleGet();
        UINT64 nowNsec = (cycle / OS_SYS_CLOCK) * OS_SYS_NS_PER_SECOND +
                         (cycle % OS_SYS_CLOCK) * OS_SYS_NS_PER_SECOND / OS_SYS_CLOCK;

        UINT32 tv_sec = nowNsec / OS_SYS_NS_PER_SECOND;
        UINT32 tv_nsec = nowNsec % OS_SYS_NS_PER_SECOND;
        printf("TaskSampleEntry2 running... tv_sec %d, tv_nsec %d\n", tv_sec, tv_nsec);
        (VOID)LOS_TaskDelay(500); // 500:2000 millisecond
#endif
    }
}

VOID TaskSampleEntry1(VOID)
{
    while (1) {
        printf("TaskSampleEntry1 running...\n");
        (VOID)LOS_TaskDelay(400); // 400:2000 millisecond
    }
}

VOID TaskSample(VOID)
{
    UINT32 ret;
    UINT32 taskID1;
    UINT32 taskID2;
    TSK_INIT_PARAM_S stTask = {0};

    printf("TaskSample: Task1 create start...\n");
    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskSampleEntry1;
    stTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTask.pcName = "TaskSampleEntry1";
    stTask.usTaskPrio = TASK1_PRIORITY;
    ret = LOS_TaskCreate(&taskID1, &stTask);
    if (ret != LOS_OK) {
        printf("Task1 create failed\n");
    }

    printf("TaskSample: Task2 create start...\n");
    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskSampleEntry2;
    stTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTask.pcName = "TaskSampleEntry2";
    stTask.usTaskPrio = TASK1_PRIORITY;
    ret = LOS_TaskCreate(&taskID2, &stTask);
    if (ret != LOS_OK) {
        printf("Task2 create failed\n");
    }
}

VOID RunTaskSample(VOID)
{
    UINT32 ret;
    ret = LOS_KernelInit();
    if (ret == LOS_OK) {
        TaskSample();
        (VOID)LOS_Start();
    }
}
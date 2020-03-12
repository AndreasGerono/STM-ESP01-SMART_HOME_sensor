#include "tasker.h"
#include "stdlib.h"
#include "stm32l0xx_hal.h"



struct TaskElement{
	_Bool active;
	uint32_t interval;
	uint32_t lastTick;
	taskFuncptr taskFunc;
};


Task task_make(uint32_t interval_ms, taskFuncptr func) {
	Task task = malloc( sizeof * task);
	if (NULL != task){
		task->active = RESET;
		task->interval = interval_ms;
		task->taskFunc = func;
		task->lastTick = HAL_GetTick();
	}
	return task;
}

void task_change(Task task, uint32_t interval){
	task->interval = interval;
}

_Bool task_state(Task task, void *args) {
	if ((HAL_GetTick() - task->lastTick >= task->interval) && task->active) {
		task->lastTick = HAL_GetTick();
		if (NULL != task->taskFunc)
			task->taskFunc(args);
		return SET;
	}
	return RESET;
}

void task_reset(Task task) {
	task->lastTick = HAL_GetTick();
}

void task_stop(Task task){
	task->lastTick = HAL_GetTick();
	task->active = RESET;
}

void task_start(Task task){
	task->active = SET;
}

void task_destroy(Task task){
	free(task);
}


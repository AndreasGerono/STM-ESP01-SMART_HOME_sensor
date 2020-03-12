//
//library for managing time tasks/events
//

//Add function pointer to task or use with if -> add NULL function
//Function needs void pointer as argument -> void funn(void *arg);
//Tasks have to be started

#include "stdint.h"


typedef void (*taskFuncptr)(void *args);

typedef struct TaskElement *Task;


Task task_make(uint32_t interval_ms, taskFuncptr);
void task_destroy(Task task);

void task_change(Task, uint32_t);
void task_reset(Task);
void task_stop(Task);
void task_start(Task);
_Bool task_state(Task, void *args);







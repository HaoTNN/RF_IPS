#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef struct _task{
	signed char state;
	unsigned long period;
	unsigned long elapsedTime;
	int (*TickFunc)(int);
} task;

#endif
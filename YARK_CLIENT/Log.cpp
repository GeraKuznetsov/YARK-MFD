#include "Log.h"
#include <string.h>
#include <iostream>

Log logs[LOG_NUM_ENTRIES];
int FirstLog = LOG_NUM_ENTRIES;

bool have = false;

void ClearLog() {
	have = false;
	FirstLog = LOG_NUM_ENTRIES ;
}

void UpdateLog(Log* l) {
	Log* last = &logs[LOG_NUM_ENTRIES - 1];
	if (!have || l->time > last->time + 1) {
		have = true;
		memmove(logs, logs + 1, (LOG_NUM_ENTRIES - 1) * sizeof(Log));
		logs[LOG_NUM_ENTRIES - 1] = *l;
		FirstLog--;
		if (FirstLog == -1) FirstLog = 0;
	}
	else if (l->time < last->time) {
		ClearLog();
	}
}

#pragma once
#include <stdint.h>

#define LOG_NUM_ENTRIES (60*5)

struct Log {
	float time;
	float logData[5];
};

extern int FirstLog;
extern Log logs[LOG_NUM_ENTRIES];

void ClearLog();
void UpdateLog(Log* l);
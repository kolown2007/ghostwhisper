#ifndef REALTIME_MANAGER_H
#define REALTIME_MANAGER_H

#include <Arduino.h>

void setupRealtime(const char* url);
void pollRealtime();

#endif // REALTIME_MANAGER_H

#pragma once

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

String getServiceTag(void);
bool initStorage(void);
void closeStorage(void);

#endif /* STORAGE_H */
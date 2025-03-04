#pragma once

#ifndef SERVICES_H
#define SERVICES_H

#include <ArduinoJson.h>

enum CartState {
    OFF,
    INIT,
    STAND_BY,
    RUNNING,
    NEED_HELP,
    SECURITY_ISSUE
};

CartState getCartState(void);
void monitorStatus(CartState state);
void powerOn(void);
void powerOff(void);

#endif /* SERVICES_H */

#pragma once

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <Arduino.h>

class smart_cart_error : public std::exception
{
private:
    const char *message;
    const char *errorCode;

public:
    smart_cart_error(const char *message, const char *errorCode);
    ~smart_cart_error();
    virtual const char *what();
    virtual const char *getErrorCode();
};

#endif /* EXCEPTIONS_H */

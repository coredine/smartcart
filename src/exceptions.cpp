#include <exceptions.h>

smart_cart_error::smart_cart_error(const char *message, const char *errorCode)
{
    this->message = message;
    this->errorCode = errorCode;
}

smart_cart_error::~smart_cart_error()
{
}

const char *smart_cart_error::what()
{
    return message;
}

const char *smart_cart_error::getErrorCode()
{
    return errorCode;
}
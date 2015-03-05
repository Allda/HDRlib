#ifndef NOTENOUGHIMAGESEXCEPTION_H
#define NOTENOUGHIMAGESEXCEPTION_H
#include <exception>

class NotEnoughImagesException : public std::exception
{
public:
    NotEnoughImagesException();
    const char * what();
    virtual ~NotEnoughImagesException();
};

#endif // NOTENOUGHIMAGESEXCEPTION_H

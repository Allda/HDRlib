#ifndef IMAGESIZEEXCEPTION_H
#define IMAGESIZEEXCEPTION_H
#include <exception>


class ImageSizeException : public std::exception
{
public:
    ImageSizeException();
    const char * what();
    ~ImageSizeException();
};

#endif // IMAGESIZEEXCEPTION_H

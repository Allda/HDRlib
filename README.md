#HDR library

C++ library which can create HDR image from multiexposure sequence of LDR images.
It can also apply tonemapping operators.

## Composing of images
* Weight function Debevec Malik
* Min 3 frames

## Tonemapping
* Linear operator
* Logaritmic operator
* Exponencial operator 
* Reinhard global operator

### Launch

```sh
$ cd HDRlib
$ qmake
$ make
$ ./HDRlib
```

### Dependencies
* [OpenCV] - open source coputer vision software library
* [Exiv2] - library which can read and write Exif information from image

### Licence
GNU GENERAL PUBLIC LICENSE Version 3
more in LICENSE file

[OpenCV]:http://opencv.org/
[Exiv2]:http://www.exiv2.org/
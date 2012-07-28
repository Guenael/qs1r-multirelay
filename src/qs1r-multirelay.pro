HEADERS = \
        audio.h \
        qs1r.h \
        manager.h \
        qs_io_libusb.h

SOURCES = \
        main.cpp \
        audio.cpp \
        qs1r.cpp \
        manager.cpp \
        qs_io_libusb.cpp

QT     += multimedia

LIBS   += ../libusb/lib/libusb.a

DESTDIR	= ../bin

CONFIG += c++11 debug link_pkgconfig
QT += widgets network
PKGCONFIG += xmlrpc_client++

SOURCES += \
    src/mainwindow.cc \
    src/main.cc \
    src/oshandling.cc \
    src/utils.cc \
    src/subdownloader.cc
HEADERS += \
    src/config.h \
    src/mainwindow.h \
    src/oshandling.h \
    src/globals.h \
    src/utils.h \
    src/subdownloader.h

TARGET = qsubber

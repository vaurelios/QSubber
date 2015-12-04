CONFIG += c++11 debug link_pkgconfig
QT += widgets network

unix: PKGCONFIG += xmlrpc_client++
unix: PKGCONFIG += sqlite3

FORMS += src/mainwindow.ui
SOURCES += \
    src/mainwindow.cc \
    src/main.cc \
    src/oshandling.cc \
    src/utils.cc \
    src/subdownloader.cc \
    src/settings.cc \
    src/configdialog.cc \
    src/subdata.cc
HEADERS += \
    src/config.h \
    src/mainwindow.h \
    src/oshandling.h \
    src/globals.h \
    src/utils.h \
    src/subdownloader.h \
    src/settings.h \
    src/configdialog.h \
    src/subdata.h

DESTDIR = build
OBJECTS_DIR = build/.obj
MOC_DIR = build/.moc
RCC_DIR = build/.rcc
UI_DIR = build/.ui

TARGET = qsubber

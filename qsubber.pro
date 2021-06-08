!versionAtLeast(QT_VERSION, 5.15.0) {
    message("Cannot use Qt $${QT_VERSION}")
    error("Use Qt 5.15 or newer")
}

CONFIG += c++11
QT += sql xml widgets network

FORMS += src/mainwindow.ui src/settingsdialog.ui
SOURCES += \
    src/application.cc \
    src/mainwindow.cc \
    src/main.cc \
    src/oshandling.cc \
    src/utils.cc \
    src/subdownloader.cc \
    src/settings.cc \
    src/settingsdialog.cc \
    src/subdata.cc \
    src/subtitlemodel.cc \
    src/rest.cc \
    src/httptransport.cc
HEADERS += \
    src/config.h \
    src/application.hh \
    src/mainwindow.hh \
    src/oshandling.hh \
    src/utils.hh \
    src/subdownloader.hh \
    src/settings.hh \
    src/settingsdialog.hh \
    src/subdata.hh \
    src/subtitlemodel.hh \
    src/rest.hh \
    src/httptransport.hh

DESTDIR = build
OBJECTS_DIR = build/.obj
MOC_DIR = build/.moc
RCC_DIR = build/.rcc
UI_DIR = build/.ui

TARGET = qsubber

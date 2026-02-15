QT       += core gui widgets
CONFIG   += c++11
TARGET   = macsfancontrol
TEMPLATE = app

# Source files
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/smcinterface.cpp \
    src/fancontrolwidget.cpp \
    src/temperaturepanel.cpp

# Header files
HEADERS += \
    src/mainwindow.h \
    src/smcinterface.h \
    src/fancontrolwidget.h \
    src/temperaturepanel.h

# Installation
target.path = /usr/local/bin
INSTALLS += target

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra

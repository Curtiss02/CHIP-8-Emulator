
CONFIG += c++11 console
CONFIG -= app_bundle
CONFIG -= qt


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CHIP-8
TEMPLATE = app

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        CHIP_8.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    CHIP_8.h

LIBS += -L C:\SDL\lib -lmingw32 -lSDL2main -lSDL2
INCLUDEPATH += C:\SDL\include

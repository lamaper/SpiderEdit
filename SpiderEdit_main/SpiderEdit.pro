QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# @Author:lamaper
# @Email:lamaper@qq.com
# @Update:2022/11/21
# Release 1.0.0.1 bulid221121
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basework.cpp \
    main.cpp \
    mainwindow.cpp \
    scommunicate.cpp \
    tabledisplay.cpp

HEADERS += \
    basework.h \
    mainwindow.h \
    scommunicate.h \
    interactive.h \
    tabledisplay.h

FORMS += \
    basework.ui \
    mainwindow.ui \
    tabledisplay.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += -I ../release/Python38/include
LIBS += -L../release/Python38/libs/ -lpython38
RC_FILE += logo.rc

DISTFILES += \
    logo.rc

RESOURCES += \
    Spider-pic.qrc


#-------------------------------------------------
#
# Project created by QtCreator 2019-11-19T16:51:24
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VisualInspection
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        VisualInspection.cpp \
    RedrawComboBox.cpp \
    HanlePicThread.cpp \
    Packaging_Inspection.cpp

HEADERS += \
        VisualInspection.h \
    RedrawComboBox.h \
    HanlePicThread.h \
    Packaging_Inspection.h

FORMS += \
        VisualInspection.ui

INCLUDEPATH += $$PWD/   \
               $$PWD/opencv\
               $$PWD/opencv2\
               $$PWD/Halconinclude


LIBS+=      $$PWD/lib/opencv_ts300.lib \ #opencv
            $$PWD/lib/opencv_ts300d.lib\
            $$PWD/lib/opencv_world300.lib\
            $$PWD/lib/opencv_world300d.lib\
            $$PWD/lib/halcon.lib\  #halcon
            $$PWD/lib/halconc.lib\
            $$PWD/lib/halconcpp.lib\
            $$PWD/lib/halconcppxl.lib\
            $$PWD/lib/halconcxl.lib\
            $$PWD/lib/halconxl.lib\
            $$PWD/lib/hdevenginecpp.lib\
            $$PWD/lib/hdevenginecppxl.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

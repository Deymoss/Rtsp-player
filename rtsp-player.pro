TEMPLATE = app

QT += qml quick widgets core gui
CONFIG += c++17

DEFINES += GST_USE_UNSTABLE_API

INCLUDEPATH += $$PWD/include \
    $(GSTREAMER_ROOT)\include\gstreamer-1.0 \
    $(GSTREAMER_ROOT)\include\glib-2.0 \
    $(GSTREAMER_ROOT)\lib\glib-2.0\include \
    $(GSTREAMER_ROOT)\lib\gstreamer-1.0\include

LIBS += \
    $(GSTREAMER_ROOT)\lib\gstreamer-1.0.lib \
    $(GSTREAMER_ROOT)\lib\gstbase-1.0.lib \
    $(GSTREAMER_ROOT)\lib\glib-2.0.lib \
    $(GSTREAMER_ROOT)\lib\gobject-2.0.lib \
    $(GSTREAMER_ROOT)\lib\gstapp-1.0.lib \
    $(GSTREAMER_ROOT)\lib\gstsdp-1.0.lib \
    $(GSTREAMER_ROOT)\lib\gstvideo-1.0.lib

SOURCES += main.cpp \
    videoitem.cpp

RESOURCES += qmlsink.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

HEADERS += \
    videoitem.h

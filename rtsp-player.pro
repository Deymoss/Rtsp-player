TEMPLATE = app

QT += qml quick widgets core gui
CONFIG += c++17

DEFINES += GST_USE_UNSTABLE_API

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG = \
    gstreamer-1.0 \
    gstreamer-video-1.0 \
    gstreamer-gl-1.0
PKGCONFIG +=  gstreamer-base-1.0 glib-2.0 gobject-2.0 gstreamer-app-1.0 gstreamer-sdp-1.0
INCLUDEPATH += $$PWD/include

SOURCES += main.cpp \
    videoitem.cpp

RESOURCES += qmlsink.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

HEADERS += \
    videoitem.h

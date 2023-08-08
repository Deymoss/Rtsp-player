#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
#include <QTimer>
#include <gst/gst.h>
#include "videoitem.h"

int main(int argc, char *argv[])
{
    int ret;
    gst_init (&argc, &argv);
    QApplication app(argc, argv);
    qmlRegisterType<VideoItem>("ACME.VideoItem", 1, 0, "VideoItem");
    gst_element_factory_make ("qmlglsink", NULL);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    ret = app.exec();
    gst_deinit ();
    return ret;
}

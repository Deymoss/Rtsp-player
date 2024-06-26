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
    {
    QApplication app(argc, argv);
    qmlRegisterType<VideoItem>("ACME.VideoItem", 1, 0, "VideoItem");
    GstElement *sink = gst_element_factory_make ("qmlglsink", NULL);
    gst_object_unref(sink);
    guint major, minor, micro, nano;
    gst_version(&major, &minor, &micro, &nano);

    // Вывод версии GStreamer
    qDebug() << "GStreamer version: "
              << major << "."
              << minor << "."
              << micro << "."
             << nano;
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    ret = app.exec();
    }
    gst_deinit ();
    return ret;
}

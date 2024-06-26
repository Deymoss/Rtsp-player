#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
#include <QTimer>
#include <QDebug>
#include <gst/gst.h>
#include "videoitem.h"

int main(int argc, char *argv[])
{
    int ret;

    gst_init (&argc, &argv);
    QApplication app(argc, argv);
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QQuickWindow::setSceneGraphBackend("opengl");
    qmlRegisterType<VideoItem>("ACME.VideoItem", 1, 0, "VideoItem");
    if(gst_element_factory_make ("qml6glsink", NULL)) {
        qDebug()<<"qml6glsink Element found!";
    } else {
        qDebug()<<"qml6glsink Element not found!";
    }
    qDebug()<<"version:"<<GST_VERSION_MAJOR<<"."<<GST_VERSION_MINOR;
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    ret = app.exec();
    gst_deinit ();
    return ret;
}

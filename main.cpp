#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
#include <QTimer>
#include <gst/gst.h>
#include "videoitem.h"

class SetPlaying : public QRunnable
{
public:
  SetPlaying(GstElement *);
  ~SetPlaying();

  void run ();

private:
  GstElement * pipeline_;
};

SetPlaying::SetPlaying (GstElement * pipeline)
{
  this->pipeline_ = pipeline ? static_cast<GstElement *> (gst_object_ref (pipeline)) : NULL;
}

void video_pad_added_handler(GstElement *srcElement, GstPad *new_pad, GstElement *sinkElement)
{
  qDebug("Received new video pad\n");
  //    GstPad *source_pad = gst_element_get_static_pad (data->source, "source");
  GstPad* sink_pad = gst_element_get_static_pad(sinkElement, "sink");
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure* new_pad_struct = NULL;
  const gchar* new_pad_type = NULL;
  if (gst_pad_is_linked (sink_pad)) {
      g_print ("We are already linked. Ignoring.\n");
  } else {
      new_pad_caps = gst_pad_get_current_caps (new_pad);
      new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
      new_pad_type = gst_structure_get_name (new_pad_struct);
      if((g_str_has_prefix(new_pad_type, "application/x-rtp") && g_str_has_prefix (gst_structure_get_string(new_pad_struct, "media"), "video"))
          || (g_str_has_prefix(new_pad_type, "video/x-raw"))) {
          ret = gst_pad_link (new_pad, sink_pad);
          if (GST_PAD_LINK_FAILED (ret)) {
              qDebug("link failed.\n");
          } else {
              qDebug("Link succeeded.\n");
          }
      }
  }
  if (new_pad_caps != NULL) {
      qDebug("gst_caps_unref \n");
      gst_caps_unref (new_pad_caps);
  }
  gst_object_unref (sink_pad);
  qDebug("sourceExit\n");
  //    //qDebug()<<"sourceExit= "<<_d->own->source<<_d->own->camera()->name()<<sink_pad<<_d.get();
}

SetPlaying::~SetPlaying ()
{
  if (this->pipeline_)
    gst_object_unref (this->pipeline_);
}

void SetPlaying::run ()
{
  if (this->pipeline_)
    gst_element_set_state (this->pipeline_, GST_STATE_PLAYING);

}

int main(int argc, char *argv[])
{
  int ret;

  gst_init (&argc, &argv);
    QGuiApplication app(argc, argv);

    GstElement *pipeline = gst_pipeline_new (NULL);
    GstElement *src = gst_element_factory_make ("rtspsrc", NULL);
    g_object_set(src, "location", gchararray(""), nullptr);
    GstElement *decode = gst_element_factory_make ("decodebin", NULL);
    GstElement *convert = gst_element_factory_make ("videoconvert", NULL);
    GstElement *glupload = gst_element_factory_make ("glupload", NULL);
    /* the plugin must be loaded before loading the qml file to register the
     * GstGLVideoItem qml item */
    GstElement *sink = gst_element_factory_make ("qmlglsink", NULL);

    g_assert (src && glupload && sink);
    g_signal_connect(src, "pad-added", G_CALLBACK(video_pad_added_handler), decode);
    g_signal_connect(decode, "pad-added", G_CALLBACK(video_pad_added_handler), convert);
    gst_bin_add_many (GST_BIN (pipeline), src, decode ,convert, glupload, sink, NULL);
    gst_element_link_many (convert, glupload, sink, NULL);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QQuickItem *videoItem;
    QQuickWindow *rootObject;
    qmlRegisterType<VideoItem>("ACME.VideoItem", 1, 0, "VideoItem");
    /* find and set the videoItem on the sink */
    rootObject = static_cast<QQuickWindow *> (engine.rootObjects().first());
    videoItem = rootObject->findChild<QQuickItem *> ("videoItem");
    g_assert (videoItem);
    g_object_set(sink, "widget", videoItem, NULL);

    rootObject->scheduleRenderJob (new SetPlaying (pipeline),QQuickWindow::BeforeSynchronizingStage);
    QTimer::singleShot(7000, [=](){gst_element_set_state (pipeline, GST_STATE_NULL); gst_object_unref(pipeline);});
    ret = app.exec();
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

  gst_deinit ();

  return ret;
}


#include "videoitem.h"
#include <QQuickWindow>
#include <QQmlEngine>
#include <QRunnable>
#include <QDebug>
#include <QProcess>
#include <QFuture>
#include <QTimer>
static bool pipelineReady = false;
struct RenderJob : public QRunnable {
    using Callable = std::function<void()>;

    explicit RenderJob(Callable c) : _c(c) { }

    void run() { _c(); }

private:
    Callable _c;
};
namespace {

static gboolean messageHandler(GstBus * /*bus*/, GstMessage *message, gpointer videoItem)
{
    g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));

    switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
        GError *err;
        gchar *debug;

        gst_message_parse_error (message, &err, &debug);
        g_print ("Error: %s\n", err->message);
        g_error_free (err);
        g_free (debug);

        // g_main_loop_quit (loop);
        break;
    }
    case GST_MESSAGE_EOS:
        /* end-of-stream */
        // g_main_loop_quit (loop);
        break;
    default:
        /* unhandled message */
        break;
    }
}

} // end namespace
VideoItem::VideoItem(QQuickItem *parent)
    : QQuickItem(parent), m_videoPipe(new VideoItemPrivate())
{
    createPipeline();
    // gst_bus_set_sync_handler(m_videoPipe->bus, messageHandler, m_videoPipe.data(), nullptr);
    // gst_element_set_state(m_videoPipe->pipeline, GST_STATE_READY);
    // gst_element_get_state(m_videoPipe->pipeline, nullptr, nullptr, 5000 * GST_MSECOND);
}

VideoItem::~VideoItem()
{

}

void VideoItem::close()
{
    // Stop the pipeline
    pipelineReady = false;
    gst_element_set_state(m_videoPipe->pipeline, GST_STATE_NULL);
    g_signal_handlers_disconnect_by_func(GST_ELEMENT(m_videoPipe->src),gpointer(video_pad_added_handler), m_videoPipe->videoDecode);
    g_signal_handlers_disconnect_by_func(GST_ELEMENT(m_videoPipe->videoDecode),gpointer(video_pad_added_handler), m_videoPipe->flip);
    // Remove the bus sync handler
    // gst_bus_set_sync_handler(m_videoPipe->bus, nullptr, nullptr, nullptr);

    // gst_element_set_state(GST_ELEMENT(m_videoPipe->pipeline),GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(m_videoPipe->pipeline));
    // gst_object_unref(GST_OBJECT(m_videoPipe->bus));
    g_source_remove (m_videoPipe->watchId);
    qDebug()<<"REFS";
    // gst_object_unref(GST_OBJECT(m_videoPipe->glSink));
   // QQuickItem *videoItem = findChild<QQuickItem *>("videoItem");
    //delete videoItem;

}

void VideoItem::setState(State state)
{
    if (m_videoPipe->state == state)
        return;
    m_videoPipe->state = state;
    emit stateChanged(m_videoPipe->state);
}

VideoItem::State VideoItem::state() const
{
    return m_videoPipe->state;
}

void VideoItem::play()
{
    qDebug() << "start play";
    gst_element_seek_simple(m_videoPipe->pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, 0);
    if (gst_element_set_state(m_videoPipe->pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
        qDebug() << "GStreamer error: unable to start playback";
    }
    qDebug() << "finish";
}

void VideoItem::stop()
{
    qDebug() << "start";
    if (m_videoPipe->state > STATE_NULL) {
        if (gst_element_set_state(m_videoPipe->pipeline, GST_STATE_NULL) == GST_STATE_CHANGE_FAILURE)
            qWarning() << "GStreamer error: unable to stop playback";
    }
    qDebug() << "finish";
}

void VideoItem::setSource(QString source)
{
    if(pipelineReady == false) {
        createPipeline();
    }
    qDebug() << "start set source";
    if (!source.isEmpty()) {
        m_videoPipe->source = source;
        gst_element_set_state(m_videoPipe->pipeline, GST_STATE_NULL);
        gst_element_set_state(m_videoPipe->pipeline, GST_STATE_READY);
        g_object_set(m_videoPipe->src, "location", QString(source).toStdString().c_str(), nullptr);
        play();
    }
}

void VideoItem::componentComplete()
{
    QQuickItem::componentComplete();
    QQuickItem *videoItem = findChild<QQuickItem *>("videoItem");
    Q_ASSERT(videoItem); // should not fail: check VideoItem.qml
    //needed for proper OpenGL context setup for GStreamer elements (QtQuick renderer)
    auto setRenderer = [=](QQuickWindow *window) {
        if (window) {
            m_videoPipe->glSink = gst_element_factory_make("qmlglsink", nullptr);
            Q_ASSERT(m_videoPipe->glSink);

            GstState current {GST_STATE_NULL}, pending {GST_STATE_NULL}, target {GST_STATE_NULL};
            auto status = gst_element_get_state(m_videoPipe->pipeline, &current, &pending, 0);

            switch (status) {
            case GST_STATE_CHANGE_FAILURE: {
                qWarning() << "GStreamer error: while setting renderer: pending state change failure";
                return;
            }
            case GST_STATE_CHANGE_SUCCESS:
                Q_FALLTHROUGH();
            case GST_STATE_CHANGE_NO_PREROLL: {
                target = current;
                break;
            }
            case GST_STATE_CHANGE_ASYNC: {
                target = pending;
                break;
            }
            }
            gst_element_set_state(m_videoPipe->pipeline, GST_STATE_NULL);
            m_videoPipe->glSink = GST_ELEMENT(gst_object_ref(m_videoPipe->glSink));
            window->scheduleRenderJob(new RenderJob([=] {
                                          g_object_set(m_videoPipe->glSink, "widget", videoItem, nullptr);
                                          g_object_set(m_videoPipe->videoSink, "sink", m_videoPipe->glSink, nullptr);
                                           gst_element_set_state(m_videoPipe->pipeline, target);
                                      }),
                                      QQuickWindow::BeforeSynchronizingStage);

            gst_object_unref(m_videoPipe->glSink);
        }
    };
    setRenderer(window());
    connect(this, &QQuickItem::windowChanged, this, setRenderer);
}
// static GstPadProbeReturn
// count_fps(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
// {
//     static int frame_count = 0;
//     static auto start_time = std::chrono::steady_clock::now();

//     frame_count++;

//     auto current_time = std::chrono::steady_clock::now();
//     std::chrono::duration<double> elapsed_seconds = current_time - start_time;

//     if (elapsed_seconds.count() >= 1.0) {
//         qDebug() << "FPS: " << frame_count / elapsed_seconds.count();
//         frame_count = 0;
//         start_time = current_time;
//     }

//     return GST_PAD_PROBE_OK;
// }

void VideoItem::createPipeline()
{
    qDebug()<<"Create pipeline";
    pipelineReady = true;
    m_videoPipe->pipeline = gst_pipeline_new("nullptr");
    m_videoPipe->src = gst_element_factory_make ("rtspsrc", "src");
    g_object_set (G_OBJECT (m_videoPipe->src), "latency", 0, NULL);
    g_object_set (G_OBJECT (m_videoPipe->src), "do-retransmission", FALSE, NULL);
       // g_object_set (G_OBJECT (m_videoPipe->src), "protocols", 4, NULL);
    m_videoPipe->videoDecode = gst_element_factory_make ("decodebin", nullptr);
    m_videoPipe->videoSink = gst_element_factory_make("glimagesink", nullptr);
    m_videoPipe->flip = gst_element_factory_make("videoflip", nullptr);
    g_object_set (G_OBJECT (m_videoPipe->flip), "video-direction", 8, NULL);
    g_object_set (G_OBJECT (m_videoPipe->videoSink), "force-aspect-ratio", TRUE, NULL);
    g_object_set (G_OBJECT (m_videoPipe->videoSink), "throttle-time", 0, NULL);
    GstElement *fakesink = gst_element_factory_make("fakesink", nullptr);
    m_videoPipe->bus  = gst_pipeline_get_bus(GST_PIPELINE(m_videoPipe->pipeline));
    m_videoPipe->watchId = gst_bus_add_watch (m_videoPipe->bus, messageHandler, NULL);
    gst_object_unref (m_videoPipe->bus);
    g_object_set(m_videoPipe->videoSink, "sink", fakesink, nullptr);
    g_object_set (G_OBJECT (m_videoPipe->videoSink), "sync", FALSE, NULL);
    g_object_set (G_OBJECT (m_videoPipe->videoSink), "async", TRUE, NULL);
    gst_bin_add_many(GST_BIN(m_videoPipe->pipeline), m_videoPipe->src, m_videoPipe->videoDecode, m_videoPipe->flip,/*gload,*/ m_videoPipe->videoSink, nullptr);

    if (!gst_element_link( m_videoPipe->flip, /*gload,*/ m_videoPipe->videoSink))
        qDebug() << "Linking GStreamer video pipeline elements failed";
    g_signal_connect(m_videoPipe->videoDecode, "pad-added", G_CALLBACK(video_pad_added_handler), m_videoPipe->flip);
    g_signal_connect(m_videoPipe->src, "pad-added", G_CALLBACK(video_pad_added_handler), m_videoPipe->videoDecode);
    gst_element_set_state(m_videoPipe->pipeline, GST_STATE_READY);
    GstPad *sink_pad = gst_element_get_static_pad(m_videoPipe->videoSink, "sink");
    // gst_pad_add_probe(sink_pad, GST_PAD_PROBE_TYPE_BUFFER, count_fps, NULL, NULL);
}

void VideoItem::video_pad_added_handler(GstElement *srcElement, GstPad *new_pad, GstElement *sinkElement)
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


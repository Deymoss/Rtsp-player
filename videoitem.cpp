
#include "videoitem.h"
#include <QQuickWindow>
#include <QQmlEngine>
#include <QRunnable>
#include <QDebug>
#include <QProcess>
#include <QFuture>
#include <QTimer>

struct RenderJob : public QRunnable {
    using Callable = std::function<void()>;

    explicit RenderJob(Callable c) : _c(c) { }

    void run() { _c(); }

private:
    Callable _c;
};
namespace {

GstBusSyncReply messageHandler(GstBus * /*bus*/, GstMessage *msg, gpointer videoItem)
{
    VideoItem *priv = static_cast<VideoItem *>(videoItem);
    //    qDebug()<<"messageHandler"<<priv->->camera()->name()<<GST_MESSAGE_TYPE(msg);
    switch (GST_MESSAGE_TYPE(msg)) {
    //    case GST_MESSAGE_LATENCY:
    //        gst_bin_recalculate_latency (GST_BIN (priv->pipeline));
    //        break;
    case GST_MESSAGE_EOS: {
        qDebug()<<"GST_MESSAGE_EOS"<<GST_MESSAGE_SRC_NAME(msg);
//        if(!priv->->healthCheck->isActive()) {
//            emit priv->->startHealthCheck();
//        }
//        emit priv->camera()->needRtsp(priv->camera(), priv->->camera()->stream());

    }
    break;

    case GST_MESSAGE_ERROR: {
        GError *error { nullptr };
        QString str { "GStreamer error: " };
        gst_message_parse_error(msg, &error, nullptr);
        qDebug()<<"GST_MESSAGE_ERROR"<<GST_MESSAGE_SRC(msg)<<error->code<<error->message;
//        if(!priv->->healthCheck->isActive()) {
//            emit priv->->startHealthCheck();
//        }
//        str.append(error->message);
//        if (priv-> != nullptr && !priv->error){
//            priv->error = (int)error->code;
//            if (!priv->->IsRemoveCam)
//                emit priv->->sigCheckRtsp();
//            qWarning()<<"GST_MESSAGE_ERROR"<<priv->->camera()->name()<<str<<error->code;
//        }
        g_error_free(error);

    } break;

    case GST_MESSAGE_HAVE_CONTEXT: {
        //    qDebug()<<"GST_MESSAGE_HAVE_CONTEXT"<<priv->->source<<GST_MESSAGE_TYPE(msg);
        GstContext *context { nullptr };

        gst_message_parse_have_context(msg, &context);

        if (gst_context_has_context_type(context, GST_GL_DISPLAY_CONTEXT_TYPE)) {
            gst_element_set_context(priv->m_videoPipe->pipeline, context);
            qDebug()<<"GST_MESSAGE_HAVE_CONTEXT"<<GST_MESSAGE_SRC(msg);
        }
        return GST_BUS_DROP;
    }  break;

    case GST_MESSAGE_STATE_CHANGED: {
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(priv->m_videoPipe->pipeline)) {
            GstState oldState { GST_STATE_NULL }, newState { GST_STATE_NULL };
            gst_message_parse_state_changed(msg, &oldState, &newState, nullptr);
            priv->setState(static_cast<VideoItem::State>(newState));
            if (newState == GST_STATE_NULL){
                priv->setState(static_cast<VideoItem::State>(newState));
            }
            if(newState == GST_STATE_PLAYING) {
//                if(priv->healthCheck->isActive()) {
//                    emit priv->stopHealthCheck();
//                }
//                if(priv->->camera()->hasSound())
//                    g_object_set(G_OBJECT(priv->volume), "mute", TRUE, NULL);
            }
        } else {
            GstState oldState { GST_STATE_NULL }, newState { GST_STATE_NULL };
            gst_message_parse_state_changed(msg, &oldState, &newState, nullptr);
        }
    } break;

    case GST_MESSAGE_STREAM_STATUS:
    {
        GstStreamStatusType type;
        GstElement *owner;
        const GValue *val;
        gchar *path;

        //     g_message ("received STREAM_STATUS");
        gst_message_parse_stream_status (msg, &type, &owner);

        val = gst_message_get_stream_status_object (msg);

        //   g_message ("type:   %d", type);
        path = gst_object_get_path_string (GST_MESSAGE_SRC (msg));
        //    g_message ("source: %s", path);
        g_free (path);
        path = gst_object_get_path_string (GST_OBJECT (owner));
        //   g_message ("owner:  %s", path);
        g_free (path);
        //    g_message ("object: type %s, value %p", G_VALUE_TYPE_NAME (val),
        //               g_value_get_object (val));

        /* see if we know how to deal with this object */
        if (G_VALUE_TYPE (val) == GST_TYPE_TASK) {
            g_value_get_object (val);
        }

        switch (type) {
        case GST_STREAM_STATUS_TYPE_CREATE:
            //  g_message ("created task %p",  g_value_get_object (val));
            break;
        case GST_STREAM_STATUS_TYPE_ENTER:
            //g_message ("raising task priority");
            /* setpriority (PRIO_PROCESS, 0, -10); */

            break;
        case GST_STREAM_STATUS_TYPE_LEAVE:
            qDebug()<<"GST_STREAM_STATUS_TYPE_LEAVE"<<GST_MESSAGE_SRC(msg);
//            if(!priv->->camera()->isSoundOn()) {
//                priv->->camera()->setIsSoundOn(false);
//            }
//            if(!priv->->healthCheck->isActive()) {
//                priv->->startHealthCheck();
//            }
//            if (!priv->->badRtspStart) {
//                //                qDebug()<<"GST_STREAM_STATUS_TYPE_LEAVE"<<priv->->camera()->name()<<GST_MESSAGE_SRC(msg);
//                if (!priv->->IsRemoveCam) {
//                    emit priv->->sigCheckRtsp();
//                }
//                priv->->badRtspStart = true;
//            }
            break;
        default:
            break;
        }
        break;
    }
    default:
        //        g_print("msg=%s (from %s)\n",gst_message_type_get_name(GST_MESSAGE_TYPE(msg)), GST_MESSAGE_SRC_NAME(msg));
        //   qDebug()<<"default"<<priv->->source<<GST_MESSAGE_TYPE(msg);

        break;
    }
    return GST_BUS_PASS;
}

} // end namespace
VideoItem::VideoItem(QQuickItem *parent)
    : QQuickItem(parent), m_videoPipe(new VideoItemPrivate())
{
    createPipeline();
//    gst_bus_set_sync_handler(m_videoPipe->bus, messageHandler, m_videoPipe.data(), nullptr);
    g_signal_connect(m_videoPipe->videoDecode, "pad-added", G_CALLBACK(video_pad_added_handler), m_videoPipe->flip);
    g_signal_connect(m_videoPipe->src, "pad-added", G_CALLBACK(video_pad_added_handler), m_videoPipe->videoDecode);
    gst_element_set_state(m_videoPipe->pipeline, GST_STATE_READY);
    gst_element_get_state(m_videoPipe->pipeline, nullptr, nullptr, 5000 * GST_MSECOND);
}

VideoItem::~VideoItem()
{

}

void VideoItem::close()
{
    // Stop the pipeline
    gst_element_set_state(m_videoPipe->pipeline, GST_STATE_NULL);

    // Remove the bus sync handler
    gst_bus_set_sync_handler(m_videoPipe->bus, nullptr, nullptr, nullptr);

    // Unref the pipeline and its elements
    gst_object_unref(m_videoPipe->pipeline);

    // Unref the bus
    gst_object_unref(m_videoPipe->bus);

    // Reset pointers
    // delete m_videoPipe->pipeline;
    // delete m_videoPipe->src;
    // delete m_videoPipe->videoDecode;
    // delete m_videoPipe->flip;
    // delete m_videoPipe->videoSink;
    delete m_videoPipe->own;
    m_videoPipe->own = nullptr;
    delete m_videoPipe;
    qDebug()<<"REFS";
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
    if(m_videoPipe->videoDecode == nullptr) {
        createPipeline();
    }
    qDebug() << "start set source";
    if (!source.isEmpty()) {
        m_videoPipe->source = source;
        gst_element_set_state(m_videoPipe->pipeline, GST_STATE_NULL);
        gst_element_set_state(m_videoPipe->pipeline, GST_STATE_READY);
        g_object_set(m_videoPipe->src, "location", source.toStdString().c_str(), nullptr);
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
            GstElement *glsink = gst_element_factory_make("qmlglsink", nullptr);
            Q_ASSERT(glsink);

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
            glsink = GST_ELEMENT(gst_object_ref(glsink));
            window->scheduleRenderJob(new RenderJob([=] {
                                          g_object_set(glsink, "widget", videoItem, nullptr);
                                          g_object_set(m_videoPipe->videoSink, "sink", glsink, nullptr);
                                           gst_element_set_state(m_videoPipe->pipeline, target);
                                      }),
                                      QQuickWindow::BeforeSynchronizingStage);
            gst_object_unref(glsink);
        }
    };
    setRenderer(window());
    connect(this, &QQuickItem::windowChanged, this, setRenderer);
}

void VideoItem::createPipeline()
{
    qDebug()<<"Create pipeline";
    m_videoPipe = new VideoItemPrivate();
    m_videoPipe->pipeline = gst_pipeline_new("nullptr");
    m_videoPipe->src = gst_element_factory_make ("rtspsrc", "src");
    g_object_set (G_OBJECT (m_videoPipe->src), "latency", 500, NULL);
    g_object_set (G_OBJECT (m_videoPipe->src), "do-retransmission", FALSE, NULL);
    //    g_object_set (G_OBJECT (m_videoPipe->src), "protocols", 4, NULL);
    m_videoPipe->videoDecode = gst_element_factory_make ("decodebin", nullptr);
    m_videoPipe->videoSink = gst_element_factory_make("glimagesink", nullptr);
    m_videoPipe->flip = gst_element_factory_make("videoflip", nullptr);
    g_object_set (G_OBJECT (m_videoPipe->flip), "video-direction", 8, NULL);
    g_object_set (G_OBJECT (m_videoPipe->videoSink), "force-aspect-ratio", TRUE, NULL);
    g_object_set (G_OBJECT (m_videoPipe->videoSink), "throttle-time", 500, NULL);
    GstElement *fakesink = gst_element_factory_make("fakesink", nullptr);
    m_videoPipe->bus  = gst_pipeline_get_bus(GST_PIPELINE(m_videoPipe->pipeline));
    g_object_set(m_videoPipe->videoSink, "sink", fakesink, nullptr);
    g_object_set (G_OBJECT (m_videoPipe->videoSink), "sync", FALSE, NULL);
    g_object_set (G_OBJECT (m_videoPipe->videoSink), "async", TRUE, NULL);//10.3.1.183

    gst_bin_add_many(GST_BIN(m_videoPipe->pipeline), m_videoPipe->src, m_videoPipe->videoDecode, m_videoPipe->flip,/*gload,*/ m_videoPipe->videoSink, nullptr);

    if (!gst_element_link( m_videoPipe->flip, /*gload,*/ m_videoPipe->videoSink))
        qDebug() << "Linking GStreamer video pipeline elements failed";
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


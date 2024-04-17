
#ifndef VIDEOITEM_H
#define VIDEOITEM_H


#include <QQuickItem>
#include <gst/gst.h>
#include <gst/gl/gl.h>



class VideoItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
public:
    enum State {
        STATE_VOID_PENDING = 0,
        STATE_NULL = 1,
        STATE_READY = 2,
        STATE_PAUSED = 3,
        STATE_PLAYING = 4,
    };
    explicit VideoItem(QQuickItem *parent = nullptr);
    ~VideoItem();
    Q_INVOKABLE void close();

    State state() const;
    void setState(State state);
    void pause();
    void resume();
    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setSource(QString source);
    static void audio_decoder_pad_added_handler(GstElement *srcElement, GstPad *new_pad, GstElement *sinkElement);
protected:
    void componentComplete() override;
signals:
    void stateChanged(VideoItem::State state);
private:
    struct VideoItemPrivate {
        explicit VideoItemPrivate()  { };

        VideoItem *own { nullptr };
        GstElement *pipeline { nullptr };
        GstElement *src { nullptr };
        GstElement *videoDecode { nullptr };
        GstElement *videoConvert { nullptr };
        GstElement  *flip { nullptr };
        GstElement *gload { nullptr };
        GstElement *videoSink { nullptr };

        GstBus *bus { nullptr };

        VideoItem::State state { VideoItem::STATE_VOID_PENDING };
        QString source = "";

        int error = 0;
    };
    static void video_pad_added_handler(GstElement *srcElement, GstPad *new_pad, GstElement *sinkElement);
    void createPipeline();
public:
    VideoItemPrivate* m_videoPipe;
};

#endif // VIDEOITEM_H

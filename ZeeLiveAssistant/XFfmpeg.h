#pragma once
#include <QObject>
#include <QtCore/qglobal.h>
#include "EeoImage.h"
#ifdef XFFMPEG_LIBRARY
#define XFFMPEGSHARED_EXPORT Q_DECL_EXPORT
#else
#define XFFMPEGSHARED_EXPORT Q_DECL_IMPORT
#endif
extern "C"
{
#include <libavutil/rational.h>
#include <libavutil/frame.h>
}

class XDemuxThread;
struct AVFrame;
typedef void(*AVFrameCallBack)(AVFrame* frame);
class XFFMPEGSHARED_EXPORT XFfmpeg : QObject
{
public:
    XFfmpeg();
    bool initialize(std::string url,AVFrameCallBack getAVFrameCB);
    void startDemuxThread();
    void stopDemuxThread();
    void pauseDemuxThread(bool bPause);
    //AV_FILTER_INFO getAVFilterInfo();
private:
    XDemuxThread* m_demuxThread=nullptr;
    bool initResult=false;
    AVFrameCallBack m_getAVFrameCB;
    std::string m_url;
};

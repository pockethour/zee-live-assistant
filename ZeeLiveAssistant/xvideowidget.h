#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "rgbrenderer.h"
#include <QBasicTimer>
typedef struct _color_scalar {
    int hOrR;
    int sOrG;
    int vOrB;
}ColorScalar;

typedef struct _ChromaKey_Paras {
    ColorScalar rgbColor;
    int similarity;
    int smoothness;
    int spill;
}CHROMA_KEY_PARAS;
struct SwsContext;
//class FCam;
class EeoLiveWebcam;
class ShaderWorker;
class PlayerAVRecorder;
class GeometryEngine;
class AudioPlayer;

class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    XVideoWidget(QWidget* parent);
    ~XVideoWidget();


public slots:
    void stopRecord();
    void startRecord();

protected:
    void initialize();
    //刷新显示
    void  paintGL() override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void timerEvent(QTimerEvent *e) override;

    //初始化gl
    void initializeGL() override;

    void paintAVFrame();
    void startReadCamera();

private:
    bool initResult = false;
    RgbRenderer* rgbRender = nullptr;
    EeoLiveWebcam* fcam = nullptr;
    QImage bgImg;
    QImage staticImg;
    QImage lastImg;
    bool isBgChanged = false;
    bool isBeautyEnabled = true;
    bool paintStatic = false;
    SwsContext* yuv2RgbaCtx = nullptr;
    bool isRemove = false;
    //std::shared_ptr<OpenglImageManager> imgManager = nullptr;
    int sceneId;
    int channelId;
    bool isFilterInit = false;
    int progress = 0;
    bool isInited = false;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ebo;
    PlayerAVRecorder* m_pAVRecorder = nullptr;
    AudioPlayer* m_pAudioPlayer = nullptr;
    AudioPlayer* m_pMicPlayer = nullptr;
private:
    QBasicTimer timer;
    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed = 0;
    QQuaternion rotation;
};

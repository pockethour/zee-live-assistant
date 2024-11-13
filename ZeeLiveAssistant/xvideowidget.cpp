#include "xvideowidget.h"
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <list>
#include <iostream>
#include <chrono>
#include <mutex>
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
//#include "fcam.h"
#include "ParamTypes.h"
#include <QDateTime>
#include "EeoExternalMediaManager.h"
#include "AudioPlayerInterface.h"
#include "imageeffect.h"
#include "opencv2/opencv.hpp"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include <libswscale/swscale.h>
}
#include <QMouseEvent>
using namespace cv;
static std::mutex frameMutex;
static std::list<AVFrame*> frameList;
//const std::string pullStreamAddr="rtmp://liveplay.eeo.im/eeolive/576f5a8c97d9-18329153e4bce95?txSecret=55a054bb3eb97ff34b7ae177ca17029c&txTime=7d8d37cd";
//const std::string pullStreamAddr="rtsp://dev:eeoa1234@10.0.127.214:554/Streaming/Channels/101";
const std::string pullStreamAddr = "rtmp://58.200.131.2:1935/livetv/cctv6hd";
static ImageFilterType bgMode = ImageFilterType::Image_Bg_AI_Dnn;
static ImageFilterType beautyModeBasic = ImageFilterType::Image_Beauty_Basic;
static ImageFilterType beautyModeSlimFace = ImageFilterType::Image_Beauty_SlimFace;
static ImageFilterType beautyModeSmooth = ImageFilterType::Image_Beauty_Smooth;
static ImageFilterType beautyModeWhitening = ImageFilterType::Image_Beauty_Whitening;
static ImageFilterType beautyModeSticker = ImageFilterType::Image_Beauty_Sticker;
static ColorScalar hsvColor = { 75,110,130 };//rgb:{74,130,102};//green
//static ColorScalar hsvColor = { 110,248,232 };//rgb:{6,84,232};//blue
//static ColorScalar hsvColor = { 175,200,100 };//rgb:{132,21,38};//red
//static ColorScalar hsvColor = { 105,114,168 };//rgb:{ 93,131,168 };//light blue

static CHROMA_KEY_PARAS chromaKeyParas = { {58,145,102},20,80,100 };//green
//static CHROMA_KEY_PARAS chromaKeyParas = { {6,84,232},20,80,100 };//blue
//static CHROMA_KEY_PARAS chromaKeyParas = { {114,9,26 },20,80,100 };//red
//static CHROMA_KEY_PARAS chromaKeyParas = { {95,133,170},20,80,100 };//light blue
static int beautyStrength = 5;
const std::string rgbWindowName = "rgb parameter";
const std::string openglWindowName = "opengl parameter";
const std::string beautyWindowName = "beauty parameter";

static void beautyCallBack(int pos, void* userdata)
{
	switch ((size_t)userdata) {
	case 0:
		beautyStrength = pos;
		qDebug() << "beautyStrength:" << pos;
		break;
	default:
		break;
	}
}

static void chromaKeyCallBack(int pos, void* userdata)
{
	switch ((size_t)userdata) {
	case 0:
		chromaKeyParas.similarity = pos;
		qDebug() << "similarity:" << pos;
		break;
	case 1:
		chromaKeyParas.smoothness = pos;
		qDebug() << "smoothness:" << pos;
		break;
	case 2:
		chromaKeyParas.spill = pos;
		qDebug() << "spill:" << pos;
		break;
	default:
		break;
	}
}

static void hsvCallBack(int pos, void* userdata)
{
	switch ((size_t)userdata) {
	case 0:
		hsvColor.hOrR = pos;
		qDebug() << "色调(H):" << pos;
		break;
	case 1:
		hsvColor.sOrG = pos;
		qDebug() << "饱和度(S):" << pos;
		break;
	case 2:
		hsvColor.vOrB = pos;
		qDebug() << "亮度(V):" << pos;
		break;
	default:
		break;
	}
}

static void getAVFrameCallBack(AVFrame* frame)
{
	frameMutex.lock();
	frameList.push_back(frame);
	if (frameList.size() > 5)
	{
		AVFrame* frame = frameList.front();
		av_frame_free(&frame);
		frameList.pop_front();
	}
	frameMutex.unlock();
}

XVideoWidget::XVideoWidget(QWidget* parent)
	: QOpenGLWidget(parent)
{
	initialize();
}

XVideoWidget::~XVideoWidget()
{
}

//刷新显示
void XVideoWidget::paintGL()
{
    paintAVFrame();
    //paint3DModel();
}


//初始化opengl
void XVideoWidget::initializeGL()
{
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

    f->glDisable(GL_DEPTH_TEST);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //启动定时器
    QTimer* ti = new QTimer(this);
    connect(ti, SIGNAL(timeout()), this, SLOT(update()));
    ti->start(50);
}

void XVideoWidget::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Request an update
        update();
    }
}

void XVideoWidget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
}

void XVideoWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}

void XVideoWidget::initialize()
{
    sceneId = OpenglImageManager::instance()->genSceneFilters();
    channelId = OpenglImageManager::instance()->genChannelFilters(sceneId);
    //staticImg = QImage("d:\\tmp\\green3.jpg");
    //staticImg = staticImg.convertToFormat(QImage::Format_RGBA8888);
    if (!rgbRender)
        rgbRender = new RgbRenderer;
    if (isBgChanged)
    {
        if (bgMode == ImageFilterType::Image_Bg_Green_Hsv)
        {
            cv::imshow(rgbWindowName, cv::Mat(50, 450, CV_8UC3));
            waitKey(3);
            int maxTrackerValue = 255;

            //调节B
            createTrackbar("H", rgbWindowName, &hsvColor.hOrR, maxTrackerValue, hsvCallBack, reinterpret_cast<void*>(0));
            //调节G
            createTrackbar("S", rgbWindowName, &hsvColor.sOrG, maxTrackerValue, hsvCallBack, reinterpret_cast<void*>(1));
            //调节R
            createTrackbar("V", rgbWindowName, &hsvColor.vOrB, maxTrackerValue, hsvCallBack, reinterpret_cast<void*>(2));

            //ColorScalar bgc = bgOperator->resetHsvColor(hsvColor);
        }
        else if (bgMode == ImageFilterType::Image_Bg_Green_Opengl)
        {
            cv::imshow(rgbWindowName, cv::Mat(50, 450, CV_8UC3));
            waitKey(3);
            int maxTrackerValue = 255;

            //调节B
            createTrackbar("R", rgbWindowName, &chromaKeyParas.rgbColor.hOrR, maxTrackerValue, hsvCallBack, reinterpret_cast<void*>(0));
            //调节G
            createTrackbar("G", rgbWindowName, &chromaKeyParas.rgbColor.sOrG, maxTrackerValue, hsvCallBack, reinterpret_cast<void*>(1));
            //调节R
            createTrackbar("B", rgbWindowName, &chromaKeyParas.rgbColor.vOrB, maxTrackerValue, hsvCallBack, reinterpret_cast<void*>(2));

            cv::imshow(openglWindowName, cv::Mat(50, 450, CV_8UC3));
            waitKey(3);
            maxTrackerValue = 1000;
            //相似度 similarity
            createTrackbar("similarity", openglWindowName, &chromaKeyParas.similarity, maxTrackerValue, chromaKeyCallBack, reinterpret_cast<void*>(0));
            //平滑 smoothness
            createTrackbar("smoothness", openglWindowName, &chromaKeyParas.smoothness, maxTrackerValue, chromaKeyCallBack, reinterpret_cast<void*>(1));
            //主色泄露减少 spill
            createTrackbar("spill", openglWindowName, &chromaKeyParas.spill, maxTrackerValue, chromaKeyCallBack, reinterpret_cast<void*>(2));
            //bgOperator->resetChromaKeyParas(chromaKeyParas);
        }
    }
    if (isBeautyEnabled)
    {
        cv::imshow(beautyWindowName, cv::Mat(50, 450, CV_8UC3));
        waitKey(3);
        int maxTrackerValue = 150;
        createTrackbar("strength", beautyWindowName, &beautyStrength, maxTrackerValue, beautyCallBack, reinterpret_cast<void*>(0));
    }
    if (!paintStatic)
    {
        startReadCamera();
    }
	if (!m_pAudioPlayer)
	{
		m_pAudioPlayer = CreateAudioPlayer(AUDIO_PCM);
		EeoExternalMediaManager::instance()->startPlayerAudio();
	}
	if (!m_pMicPlayer)
	{
		m_pMicPlayer = CreateAudioPlayer(AUDIO_MIC);
		connect(m_pMicPlayer, &AudioPlayer::micphoneReady, this, [&]() {
			m_pMicPlayer->set_mic_volume(1.0);
			});
	}
}

void XVideoWidget::startReadCamera()
{
    std::vector<EeoAVDeviceInfo> devices;
    EeoExternalMediaManager::instance()->listVideoDevices(devices);
    EeoLiveWebcamParams fCamParams;
    //FCamParams fCamParams;
    fCamParams.filename = devices.at(0).device_name.toStdString();
    fCamParams.upload = false;
	fCamParams.width = 1280; //1280;
	fCamParams.height = 720;//720;
    fCamParams.flip = true;
    fCamParams.force_fps = 30;
    fCamParams.bitrateX = 1.0;
    fCamParams.resolution = EeoVideoResolution::VIDEO_RES_CLASSINCAM;
    if (!fcam)
        fcam = new EeoLiveWebcam(fCamParams);
    connect(fcam, &EeoLiveWebcam::imageReady, this, [=](const QImage& img) {
		if (m_pAVRecorder) {
			QImage recImg = img.convertToFormat(QImage::Format_BGR888);
			m_pAVRecorder->updateSrcImage(recImg);
		}
        lastImg = img;
        });
}

void XVideoWidget::stopRecord()
{
    qDebug()<<"stopRecord!!!";
	if (m_pAVRecorder) {
		m_pAVRecorder->stopAVRecorder();
		m_pAVRecorder->disconnect();
		delete m_pAVRecorder;
		m_pAVRecorder = nullptr;
	}
	if (m_pAudioPlayer)
	{
		m_pAudioPlayer->close_audio();
	}
}

void XVideoWidget::startRecord()
{
	qDebug() << "startRecord!!!";

	std::vector<EeoAudioInDevInfo> vecAudioInDevInfo;
	EeoExternalMediaManager::instance()->listAudioInDevices(vecAudioInDevInfo);
	QString  micGuid = vecAudioInDevInfo.at(0).device_guid;
	int iOpenAudio = m_pMicPlayer->open_audio(micGuid);
	if (iOpenAudio != 0)
	{
		qInfo() << ">>>>>>>>>>>>>>>AudioPlayer::open_audio() failed, return" << iOpenAudio;
	}

	if (m_pAVRecorder)
	{
		m_pAVRecorder->disconnect();
		delete m_pAVRecorder;
		m_pAVRecorder = nullptr;
	}
	QString videoOutput_res_fps = QString("hd720_") + QString::number(30);
	QString windowTitle("ClassinCamHW");
	m_pAVRecorder = new PlayerAVRecorder();
	m_pAVRecorder->startAVRecorder("d:\\save.mp4", "", "", windowTitle, "", "", true, true, false, videoOutput_res_fps);
}


#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

void yuv2rgb(int y, int u, int v, int* r, int* g, int* b)
{
	*r = (((y << 8) + (v << 8) + (v << 6) + (v << 3)) >> 8);
	*g = (((y << 8) + (u << 5) + (u << 4) + (u << 2) + (u << 1) + u + (v << 6) + (v << 5) + v) >> 8);
	*b = (((y << 8) + (u << 9) + (u << 5) + u) >> 8);
	*r = CLAMP(*r, 0, 255);
	*g = CLAMP(*g, 0, 255);
	*b = CLAMP(*b, 0, 255);
}

void XVideoWidget::paintAVFrame()
{
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::map<std::string, double> paras;
	if (!isFilterInit)
	{
		isFilterInit = true;
		BeautySmoothValue svalue;
		svalue.strength = 5;
		svalue.isFaceDetect = true;
		OpenglImageManager::instance()->addChannelImageFilter(sceneId, channelId, beautyModeSmooth, &svalue);

		//BeautyWhiteningValue wvalue;
		//wvalue.strength = 5;
		//wvalue.isFaceDetect = true;
		//OpenglImageManager::instance()->addChannelImageFilter(sceneId, channelId, beautyModeWhitening, &wvalue);

		//BeautySlimFaceValue slvalue;
		//slvalue.strength = 5;
		//OpenglImageManager::instance()->addChannelImageFilter(sceneId, channelId, beautyModeSlimFace, &slvalue);

		//BeautyStickerValue stvalue;
		//stvalue.stickerImgPath = "D:/Project/imagedemo/thirdparty/sticker/spaceman.svg";
		//stvalue.isFaceDetect = true;
		//OpenglImageManager::instance()->addChannelImageFilter(sceneId, channelId, beautyModeSticker, &stvalue);

		//BeautyBasicValue bvalue;
		//bvalue.scale = 0.0;
		//bvalue.brightness = 0;
		//bvalue.contrast = 0;
		//bvalue.saturation = 0;
		//OpenglImageManager::instance()->addChannelImageFilter(sceneId, channelId, beautyModeBasic, &bvalue);
	}
	else
	{
		BeautySmoothValue svalue;
		svalue.strength = beautyStrength;
		OpenglImageManager::instance()->setChannelFilterParas(sceneId, channelId, beautyModeSmooth, FilterParaType::BtSmoothStrength, &svalue);

		//BeautyWhiteningValue wvalue;
		//wvalue.strength = beautyStrength;
		//OpenglImageManager::instance()->setChannelFilterParas(sceneId, channelId, beautyModeWhitening, FilterParaType::BtWhiteningStrength, &wvalue);

		//BeautySlimFaceValue slvalue;
		//slvalue.strength = beautyStrength;
		//OpenglImageManager::instance()->setChannelFilterParas(sceneId, channelId, beautyModeSlimFace, FilterParaType::BtSlimFaceStrength, &slvalue);

		//BeautyBasicValue bvalue;
		//bvalue.scale = 0.0;
		//bvalue.brightness = 0.0;
		//bvalue.contrast = 0.0;
		//bvalue.alpha = 1.0;
		//bvalue.xRotation = 0.0;
		//bvalue.yRotation = beautyStrength;
		//bvalue.zRotation = 0.0;
		//bvalue.cornerRadius = 50;
		//OpenglImageManager::instance()->setChannelFilterParas(sceneId, channelId, beautyModeBasic, FilterParaType::BtBasicStrength, &bvalue);
	}
	if (lastImg.isNull())
		return;
	QImage outImg(staticImg.width(), staticImg.height(), staticImg.format());
	MergeImgParam mip;
	mip.mergeType = MergeType::OPEN_DOOR;
	mip.srcImage = staticImg;
	mip.dstImage = lastImg;
	mip.progress = (progress += 2);
	mip.chanRect = QRect(0, 0, 1280, 720);
	if (progress == 100)
		progress = 0;

	//new merge channellist
	ChannelMergeParam channelMergeParam;
	channelMergeParam.id = channelId;
	channelMergeParam.rect = QRect(0, 0, 1280, 720);
	channelMergeParam.rotation = 0;
	QList<ChannelMergeParam> channelList;
	channelList.push_back(channelMergeParam);
	QColor color(0, 0, 0, 0);
	//OpenglImageManager::instance()->merge2Images(sceneId, channelId, mip);
	OpenglImageManager::instance()->sendChannelFrame(sceneId, channelId, lastImg);
	QImage graffiti;
	QImage waterMark;
	OpenglImageManager::instance()->mergeImages(sceneId, channelList, 1280, 720, color, graffiti, waterMark);
	//if (OpenglImageManager::instance()->getChannelFrame(sceneId, channelId, outImg))
	if (OpenglImageManager::instance()->getMergeImg(sceneId, outImg))
	{
		if (outImg.format() != QImage::Format_RGBA8888)
		{
			outImg = outImg.convertToFormat(QImage::Format_RGBA8888);
		}
		if (outImg.width() != 0 && outImg.height() != 0)
		{
			makeCurrent();
			rgbRender->drawFrame(outImg.bits(), outImg.width(), outImg.height());
			doneCurrent();
		}
	}
}

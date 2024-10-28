#ifndef _EEO_EXTERNAL_MEDIA_MANAGER_H
#define _EEO_EXTERNAL_MEDIA_MANAGER_H

#include <cstdint>
#include <string>
#include <vector>

using std::string;
using std::vector;

#include "eeocamera_global.h"
#include <QObject>
#include <QVector>
#include <QPair>
#include <QMetaType>
#include <QVariant>


#define MAX_FRIENDLY_NAME_LENGTH    128
typedef struct _TEeoDeviceName{
	wchar_t FriendlyName[MAX_FRIENDLY_NAME_LENGTH];
	int index;
} TEeoDeviceName;

typedef struct _EeoAudioInDevInfo {
	QString device_name;
	QString device_guid;
	int index;
} EeoAudioInDevInfo;

typedef struct _EeoAudioOutDevInfo {
	QString device_name;
	QString device_guid;
} EeoAudioOutDevInfo;

class EEOCAMERASHARED_EXPORT EeoFrameData {
public:
	explicit EeoFrameData();
	virtual ~EeoFrameData();
	void reset(const char*data, int data_size, const int* linesize, int width, int height, int pix_format);
	int getWidth(void) const { return width; }
	int getHeight(void) const { return height; }
	const QByteArray & getData() const { return data; }
	int getLineSize(int index) const { if (index < linesize.size()) return linesize[index]; else return 0; }
	int getPixFormat(void) const { return pix_format; }

private:
	QByteArray data;
	QVector<int> linesize;
	int width;
	int height;
	int pix_format;
};
Q_DECLARE_METATYPE(EeoFrameData);

class AVDevMonitor;
class EEOCAMERASHARED_EXPORT EeoFFmpeg
{
public:
	static void registerAll();
};

class EEOCAMERASHARED_EXPORT EeoExternalMediaManager : public QObject
{
    Q_OBJECT
public:
	static EeoExternalMediaManager * instance();
		
	int startPlayerAudio(long long uid = 0);
	int stopPlayerAudio();	

	/*  volume range:  from 0 to 1.0.
	return value: 0 success, -1 failure
	*/
	void set_audio_output_volume(float volume);
	void get_audio_output_volume();
    void get_audio_output_mute();

    void StartListenOSSpeakerVolume();
    void StopListenOSSpeakerVolume();
    void SendOSSpeakerVolume(float fValue);
    void SendOSSpeakerVolume_CallBack(QString qstrGUID, float fValue);
    void SendOSMute_CallBack(bool bMute);

	int listAudioInDevices(std::vector<EeoAudioInDevInfo> &info);
	int listAudioOutDevices(std::vector<EeoAudioOutDevInfo> &info);
	void QueryCurrentSpeakerGUID(QString& qstrGUID);
	void QueryCurrentSpeakerName(QString& qstrName);
	int selectAudioOutDevice(QString qstrGUID);
	int listVideoDevices(std::vector<EeoAVDeviceInfo> &devices);
    bool listVideoDeviceOptions(const EeoAVDeviceInfo &device, EeoVideoDevCaps &devcaps);

	void SendAudioOutputChangedSignal();
	void SendAudioInputChangedSignal();
    void SendAudioAggExceptSignal();

	void SendVideoInputChangedSignal();
	int setAudioPlayerDelay(int delayMS);
    //void setRecordSysAudioFlag(bool recordSysAudio);
	~EeoExternalMediaManager();
	void cleanResource();
	long long getLocalUid(){ return _uid; };
	void setSpeakerUseRNN(bool flag);
	bool getSpeakerRNNUsed();
    bool useEeoAudioDriver();

	void setClassroomId(quint64 id);
	void mergeVideoFiles(const std::vector<QString>& srcFileList, const QString& dstVideoFile);
	void split2VideoFiles(const QString& filename, qreal pos, const QString& fileName1, const QString& fileName2);
	void splitVideoFiles(const QString& srcVideoFile, const std::vector<qreal>& posList, const std::vector<QString>& dstFileList);
    // Douzhq Added 20210908
    // open video decodec
    bool openVideo(const QString& fileName);
    // close video
    void closeVideo(void);
    // get Video Infos
    void getVideoImageSize(int& width, int& height);
    qreal getVideoDuration(void);

    // Split Video File
    void splitVideoFile(const QString& fileName, qreal pos, const QString& fileName1, const QString& fileName2);
	void clampVideoFile(const QString& fileName, qreal pos1, qreal pos2, const QString& destFileName);
	void clipGroupVideoFile(const QString& fileName, const QVector<QPair<qreal, qreal>>& clipFrameGroup, const QString& destFileName);
	void breakClipGroupVideoFile(void);

    // Seek Video
    void seekVideo(qreal frame);
    // get Frame Image
    void getFrameImage(void);
	void getFrameImage(qreal frame);

    int64_t getAudioRecorderMS();
signals:
	void AuidoMixerStartRequest();
	void AuidoMixerStopRequest();
	void AuidoProcessStartRequest();
	void AudioOutputChangedSig();
    void AudioAggExceptSignal();
	void AudioInputChangedSig();
	void VideoInputChangedSig();
	void audioDelayMS(int);
	void mergeVideoFilesFinish(const QString& dstVideoFile);
	void mergeVideoFilesError(int errorNo);
	void split2VideoFilesFinish(const QString& fileName1, const QString& fileName2);
	void splitVideoFilesFinish(const std::vector<QString>& dstFileList);
	void splitVideoFilesError(int errorNo);

    void splitVideoFileFinish(const QString& fileName1, const QString& fileName2);
    //void splitVideoFileFinish(const QString& fileName);
	void clampVideoFileFinish(const QString& fileName);
    void getFrameImageFinished(QImage image, qreal srcFrame, qreal frame);
	void clipGroupFrameFinished(const QString& fileName);
	void clipGroupFrameFileError(const QString& fileName, bool isBreak);
	void clipGroupVideoProceValue(int value);
    void OSVolume_Speaker(float fValue);// for listen os volume
    void OSVolume_Speaker_CallBack(QString qstrGUID, float fValue); // for get speaker volume once
    void OSMute_CallBack(bool bMute); // for get OS mute once

private:
	EeoExternalMediaManager(QObject *parent = 0);
	EeoExternalMediaManager(EeoExternalMediaManager const &);
	EeoExternalMediaManager& operator = (const EeoExternalMediaManager &);
    bool listVideoDeviceOptionsWin(const EeoAVDeviceInfo &device, EeoVideoDevCaps &devcaps);
    int listVideoDevicesWin(std::vector<EeoAVDeviceInfo> &devices);
    int listVideoDevicesMac(std::vector<EeoAVDeviceInfo> &devices);
	int listDshowAudioInDevices(std::vector<EeoAudioInDevInfo> &info);
	void setLocalUid(long long uid){ _uid = uid; };


private:
	QThread   *audioProcessThread;
	QThread   *audioMixerThread;
	QThread   *micVolumeThread;
	QThread   *AVRecThread;
	QObject  *pAVRecObj;
	AVDevMonitor *pDevMonitor;
	static EeoExternalMediaManager *m_instance;
	long long _uid = 0;
};

class FrontCamera;
class EEOCAMERASHARED_EXPORT EeoFrontCamera : public QObject
{
    Q_OBJECT

public:
    explicit EeoFrontCamera(const char *filename, int outputWidth, int outputHeight);
    virtual ~EeoFrontCamera();

private:
    FrontCamera *camera;

public:
    virtual void changeOutput(int width, int height);

signals:
    void imageReady(const QImage &);
};


class LiveWebcam;
class IPCamera;
class ClassinCam;
class NonVirtualCam;
class EEOCAMERASHARED_EXPORT EeoLiveWebcam : public QObject
{
    Q_OBJECT

public:
    explicit EeoLiveWebcam(EeoLiveWebcamParams &params);
	explicit EeoLiveWebcam() { webcam = NULL; };

    virtual ~EeoLiveWebcam();
	LiveWebcam *getCameraInstance() { return webcam; };
	NonVirtualCam* getNonVirtualCamInstance() { return m_pNonVirtualCam; };
	void* GetInternalImpl() {
		return true == m_isNonVirtualCam ? (void*)m_pNonVirtualCam : (void*)webcam;
	}
	bool isNonVirtualCam() { return m_isNonVirtualCam; };
	EeoLiveWebcamParams& GetParams()
	{
		return m_liveWebcamParams;
	}
private:
    static const float resolutionRatio;
    static const float maxResolutionWidthSD;
    static const float maxResolutionHeightSD;
    static const float maxResolutionWidthHD;
	static const float maxResolutionWidthFHD;
    static const float maxResolutionHeightHD;
	static const float maxResolutionHeightFHD;

    LiveWebcam *webcam = NULL;
	ClassinCam* classinCam = NULL;
	NonVirtualCam* m_pNonVirtualCam = nullptr;
	bool m_isNonVirtualCam = false;
	EeoVideoResolution resolution;
	EeoLiveWebcamParams m_liveWebcamParams;
public:
    virtual void changeOutput(int width, int height);
	virtual void changeChannelOutput(int width, int height);
    virtual bool isOnLive();
    virtual void startLive();
    virtual void stopLive();
	virtual float getBitrateX();
	virtual void setBitrateX(float times);
	virtual EeoVideoResolution getResolution();
    virtual bool doesFlip();
    virtual void startFlip();
    virtual void Rotate(int angle);
    virtual void stopFlip();
	virtual void applyRemoteFlip(bool enable);
	virtual bool isApplyRemoteFlip();
	virtual void pausePreview(bool pause);
	virtual void startFullPreview();
	virtual void stopFullPreview();
	virtual int getChannelId();
	virtual void setChannelId(int channelId);
	virtual uint64_t getCId();
	virtual void setCId(uint64_t cid);
	virtual void setVolume(int val);
	virtual void setLocalAudioEnable(bool bEnable);
	virtual void setLiveAudioEnable(bool bEnable);
	virtual void changeCropArea(QRect &cropArea);	// only for VIDEO_RES_WYSIWYG
signals:
    void imageReady(const QImage &);
	void fullImageGot(const QImage&);
	void frameReady(const EeoFrameData &);
    void failed();
	void volume(float val);
	void volumeChanged(int val);
protected:
    void filterResolution(int &width, int &height, float maxWidth, float maxHeight);
    static float maxResolutionWidth(EeoVideoResolution resolution);
    static float maxResolutionHeight(EeoVideoResolution resolution);
    virtual int getWidth() const;
    virtual int getHeight() const;
};

class VideoWallCam;
class EEOCAMERASHARED_EXPORT EeoVideoWallCam : public EeoLiveWebcam
{
	Q_OBJECT

public:
	explicit EeoVideoWallCam(EeoLiveWebcam* webcam, EeoLiveWebcamParams& params);

	virtual ~EeoVideoWallCam();
	VideoWallCam* getCameraInstance() { return videoWallCam; };
private:
	VideoWallCam* videoWallCam=NULL;

public:
	virtual void changeOutput(int width, int height);
	virtual bool isOnLive();
	virtual void startLive();
	virtual void stopLive();
	virtual float getBitrateX();
	virtual void setBitrateX(float times);
	virtual EeoVideoResolution getResolution();
	virtual bool doesFlip();

private:
	virtual void startFlip();
    virtual void Rotate(int angle);
	virtual void stopFlip();
	virtual void startFullPreview();
	virtual void stopFullPreview();
	int getWidth() const;
	int getHeight() const;

	void signals_connect();

signals:
	void imageReady(const QImage&);
	void frameReady(const EeoFrameData&);
	void failed(EeoCameraError err = ERROR_UNKNOWN);
	void stop();
	void resume();
	void pause();
};

class LiveWebcamUploader;
struct AVPacket;
class EEOCAMERASHARED_EXPORT EeoLiveWebcamUploader : public QObject {
	Q_OBJECT

public:
	explicit EeoLiveWebcamUploader(uint64_t classroomId, int channelId, QObject *parent = 0);
	virtual ~EeoLiveWebcamUploader();

	void upload(const AVPacket *avpacket, int fps);
private:
	LiveWebcamUploader *uploader;
};


class LiveWebcamMonitor;
class EEOCAMERASHARED_EXPORT EeoLiveWebcamMonitor : public QObject
{
    Q_OBJECT

public:
    explicit EeoLiveWebcamMonitor(uint64_t cid, uint64_t uidToWatch, uint8_t channelId);
    virtual ~EeoLiveWebcamMonitor();

private:
    LiveWebcamMonitor *monitor;

public:
    string getReceivingStatisticsData() const;
    void changeDelay() const;

signals:
    void imageReady(const QImage &);
	void frameReady(const EeoFrameData &);
    void disconnected();
    void failed();
};


class LiveScreen;
class EEOCAMERASHARED_EXPORT EeoLiveScreen : public QObject
{
    Q_OBJECT

public:
    explicit EeoLiveScreen(uint64_t cid, int offsetX, int offsetY, int outputWidth,
        int outputHeight, float bitrateX = 1, int fps = 6);
    virtual ~EeoLiveScreen();

private:
    LiveScreen *screen;

public:
	virtual void pause();
	virtual void resume();
    virtual void changeOutput(int x, int y, int width, int height);
    virtual float getBitrateX();
    virtual void setBitrateX(float times);


signals:
    void imageReady(const QImage &);
    void failed();
};


class LiveScreenMonitor;
class EEOCAMERASHARED_EXPORT EeoLiveScreenMonitor : public QObject
{
    Q_OBJECT

public:
    explicit EeoLiveScreenMonitor(uint64_t cid, uint64_t uidToWatch);
    virtual ~EeoLiveScreenMonitor();

private:
    LiveScreenMonitor *monitor;

public:
    string getReceivingStatisticsData() const;
    void changeDelay() const;

signals:
    void imageReady(const QImage &);
	void frameReady(const EeoFrameData &);
    void disconnected();
    void failed();
};


class EEOCAMERASHARED_EXPORT EeoMediaProtocol
{
public:
    static void setGetConfigCallback(string (*callback)(string key));
	static void setSetConfigCallback(void (*callback)(string key, string value));
    static void setGetDelayCallback(int (*callback)());  /* delay in million seconds */
    static void setIsTransByUDPCallback(bool (*callback)());
    static void setUploadStatisticsDataCallback(bool (*callback)(bool flush, const char *module,
        const char *subModule, const char *subject, uint8_t mode, uint8_t version,
        uint8_t tag, const char *data, uint32_t len));

	static void setAirPlayEncryptCallback(char*(*callback)(char *buffer, quint32 bufferLength, quint32 CmdCode, quint64 SourceUID, quint32 Seq, qint16 KeyIndex));
	static void setAirPlayDecryptCallback(bool(*callback)(char* buffer, quint32 bufferLen));
	static void setAirPlayDecryptPartCallback(bool(*callback)(char* buffer, quint32 bufferLen, qint16 keyIndex));
    static void setSendWebcamVideoCallback(int(*callback)(uint64_t classroomId, uint8_t channelId, const char *data, uint32_t len));
    static void setSendScreenVideoCallback(int (*callback)(uint64_t classroomId, const char *data, uint32_t len));
    static void setUploadCamImageCallback(int(*callback)(QString path, const char* data, uint32_t len));
	static void setUploadAudioDataCallback(int(*callback)(QString type, QString end, QString qstrSec, char* data, int len));
    static void setRetransWebcamVideoCallback(bool (*callback)(uint64_t cid, uint64_t uid, uint8_t channelId, vector<uint32_t> &sequences));
    static void setRetransScreenVideoCallback(bool (*callback)(uint64_t cid, uint64_t uid, vector<uint32_t> &sequences));

    static void setSendWebcamAudioCallback(int (*callback)(uint64_t classroomId, uint8_t channelId, const char *data, uint32_t len));
    static void setSendScreenAudioCallback(int (*callback)(uint64_t classroomId, const char *data, uint32_t len));
    static void setRetransWebcamAudioCallback(bool (*callback)(uint64_t cid, uint64_t uid, uint8_t channelId, vector<uint32_t> &sequences));
    static void setRetransScreenAudioCallback(bool (*callback)(uint64_t cid, uint64_t uid, vector<uint32_t> &sequences));

    static void feedWebcamVideo(uint64_t cid, uint64_t uid, uint8_t channelId, uint32_t msgid,
        const char *data, uint32_t len);
    static void feedScreenVideo(uint64_t cid, uint64_t uid, uint32_t msgid,
        const char *data, uint32_t len);
    static void feedWebcamAudio(uint64_t cid, uint64_t uid, uint8_t channelId, uint32_t msgid,
        const char *data, uint32_t len);
    static void feedScreenAudio(uint64_t cid, uint64_t uid, uint32_t msgid,
        const char *data, uint32_t len);
};

struct EeoAirPlayerInitParams {
	quint8 channelid;
	quint64 cid;
	quint64 uid;
	quint8 groupid = 0;
	quint16 width = 0;
	quint16 height = 1080;
	QString phonenum = "";
	quint8 enAudio = false;
};
struct EeoAirPlayerOutputParams {
	quint16 width = 0;
	quint16 height = 1080;
	quint8 enAudio = false;
};
Q_DECLARE_METATYPE(EeoAirPlayerOutputParams);
class EeoAirPlayerInternal;
class EEOCAMERASHARED_EXPORT EeoAirPlayer : public QObject
{
	Q_OBJECT

public:
	EeoAirPlayer(EeoAirPlayerInitParams params);
	~EeoAirPlayer();
	
	void initialize();
	void changeOutput(EeoAirPlayerOutputParams params);
	void switchAudioPopulate(bool enable);
signals:
	void error(EeoCameraError err=ERROR_UNKNOWN);
	void serverStarted(quint16 tcp_ctrl_port, quint16 udp_data_port);
	void started();
	void stopped();
	void imageReady(const QImage &);
	void frameReady(const EeoFrameData &);
	//internal signals
	void audioPopulateSwitched(bool enable);
	void outputChanged(EeoAirPlayerOutputParams params);

private:
	EeoAirPlayerInitParams initParams;
	EeoAirPlayerInternal *airplayer;
};

class EeoAVRecorder;
class EeoAudioRecorder;
struct AVFrame;
class EEOCAMERASHARED_EXPORT PlayerAVRecorder : public QObject
{
    Q_OBJECT

private:
    int startAVRecorder(QString filename, QString rtmp_url, QString rtmp_tag,
        QString window_title,
        int screenX, int screenY, int screenWidth, int screenHeight,
        QString logo_filename, QString logo_md5_hex,
        bool mic_included,
        bool sys_audio_flag,
		bool show_watermark,
		QString output);

public:
   virtual int startAVRecorder(QString filename, QString rtmp_url, QString rtmp_tag,
        QString window_title,
		QString logo_filename, QString logo_md5_hex,
		bool mic_included,
		bool sys_audio_flag=true,
		bool show_watermark=true,
		QString output = "hd720");
	virtual void attachCamera(EeoLiveWebcam* webCamera);
	virtual int changeVideoRecordMode(QString mode);
    virtual int stopAVRecorder();
     int pauseAVRecorder();
     int resumeAVRecorder();
	virtual int terminateAVRecorder();
	//ture: record micphone
	virtual void recordMicphone(bool flag);
	/*
	0: can av record.

	other values: can not av record
	1: Do not support record on xp
	2: windows has not valid audio output device.
	3: mac device must install "iShowU Audio Capture" software.
	4. mac device must set speaker "ClassIn" to default
	5. can not record system audio, you must switch to record application audio
	*/
	virtual int canAVrecord(bool sys_audio_flag = true);

    /*0:ignore  1: installed 2:no installed*/
	virtual int audioDriverStatus();

	virtual void openScreen(int x, int y, int width, int height);
	virtual void closeScreen();
	virtual void resizeScreen(int x, int y, int width, int height);
    void stopUpdateFrame();
	void updateSrcImage(const QImage& srcImage);
    void updateSrcFrame(AVFrame* srcFrame);
	PlayerAVRecorder(QObject *parent = 0);
	~PlayerAVRecorder();
     int64_t getVideo_First_Dts();
signals:
	void requestAVRecord();
    void requestOpenScreen(int x, int y, int width, int height);
    void requestCloseScreen();
    void requestResizeScreen(int x, int y, int width, int height);
    void failed(EeoCameraError err=ERROR_UNKNOWN);
	void pushing(int ret, int size);
	void aCaptureExcept();
	void aOpenExcept();
    void aCheckOutput();
	void aMicDataExcept();
	void closed();
    void firstVideoFrameWritten();
    void recordStarted();
	void firstVideoFrameFinished(const QImage& img);
protected slots:
	virtual void processClosed();
	virtual void processDefaultOutChanged();
	virtual void processReopenAudio();

protected:
    QThread   *AVRecThread;	
	bool started = false;
    bool _system_flag = false;
	EeoLiveWebcam* webcamera = NULL;
	bool isWebcamRecord = false;
private:
	EeoAVRecorder* pEeoAVrec;
};

class EeoBoxAVRecorder;
class EEOCAMERASHARED_EXPORT PlayerAVRecorderForVideobox : public PlayerAVRecorder
{
	Q_OBJECT

public:
	virtual int startAVRecorder(QString filename, QString rtmp_url, QString rtmp_tag,
		QString window_title,
		QString logo_filename, QString logo_md5_hex,
		bool mic_included,
		bool sys_audio_flag = true,
		bool show_watermark = true,
		QString output = "hd720");
	virtual int stopAVRecorder();
	virtual int changeVideoRecordMode(QString mode);
	virtual int terminateAVRecorder();
	//ture: record micphone
	virtual void recordMicphone(bool flag);
	/*
	0: can av record.

	other values: can not av record
	1: Do not support record on xp
	2: windows has not valid audio output device.
	3: mac device must install "iShowU Audio Capture" software.
	4. mac device must set speaker "ClassIn" to default
	5. can not record system audio, you must switch to record application audio
	*/
	virtual int canAVrecord(bool sys_audio_flag = true);

	/*0:ignore  1: installed 2:no installed*/
	virtual int audioDriverStatus();

	~PlayerAVRecorderForVideobox();

protected slots:
    virtual void processClosed();
    virtual void processDefaultOutChanged();
	virtual void processReopenAudio();
private:
	EeoBoxAVRecorder *pEeoBoxAVrec;
};

class EeoAudioRecorder;
class EEOCAMERASHARED_EXPORT MicphoneRecorder : public QObject
{
	Q_OBJECT
public:
	int startRecorder(QString filename);
	int stopRecorder();
	int pauseRecorder();
	int resumeRecorder();

	MicphoneRecorder(QObject *parent = 0);
	~MicphoneRecorder();
signals:
	void requestRecord();
	void failed();
	void aCaptureExcept();
    void aOpenExcept();
    void aCheckOutput();

private:
	QThread   *AudioRecThread;
	EeoAudioRecorder *pRec;
};
//EEOCAMERASHARED_EXPORT 	MicphoneRecorder* CreateMicphoneRecorder();

/*speaker recorder */
class EEOCAMERASHARED_EXPORT SpeakerRecorder : public QObject
{
	Q_OBJECT
public:
	int startRecorder(QString filename, bool mic_included = true);
	int stopRecorder();
	int pauseRecorder();
	int resumeRecorder();

	SpeakerRecorder(QObject *parent = 0);
	~SpeakerRecorder();
signals:
	void requestRecord();
	void failed();
	void aCaptureExcept();
	void aOpenExcept();
    void aCheckOutput();
private:
	QThread   *AudioRecThread;
	EeoAudioRecorder *pRec;
};
//EEOCAMERASHARED_EXPORT 	SpeakerRecorder* CreateSpeakerRecorder();
class videobox_client;
class EEOCAMERASHARED_EXPORT VideoBoxManager : public QObject
{
	Q_OBJECT
public:
	static VideoBoxManager& Instance();

	void init();
	void uninit();

	void StartupVideoboxModule(bool bStartup);
	bool isVideoboxModuleStartup();

	videobox_client* GetVideoBoxClient();

signals: //action
	void signal_findDev();

	void signal_connectDev(QString ip, quint16 port, QString name);

	void signal_updateNetCamera(QString url);

	void signal_updateMicrophoneState(bool state);

	void signal_detectDev();

	void signal_updateDev();

	void signal_requestLogFile(QString account, long uid, QString host);

	void signal_disconnectDev();

	void finished();


signals:  //callback
	void signalFindDevOk();
	void signalFindDevError();
	void signalFindItemDev(VideoboxFindInfo& info);
	void signalDevDetectResult(QString res_json);
	void signalDevOnlineStatus(bool is_online);
	void signalDevUpdateInfoNotify(QString update);
	void signalDevUpdateStepInfo();
	void signalDevExceptionInfo(QString qsInfo);

private:
	VideoBoxManager();
	~VideoBoxManager();

	videobox_client* m_pImpl = nullptr;
	QThread* m_workThread = nullptr;
	bool m_bStartupVideoboxModule = false;
};

EEOCAMERASHARED_EXPORT bool ifCanUseDXGICapture(void);
#endif // EEOMEDIAMANAGER_H

#ifndef EEOCAMERA_GLOBAL_H
#define EEOCAMERA_GLOBAL_H

#include <QPixmap>
#include <QtCore/qglobal.h>
#include <string>

#if defined(EEOCAMERA_LIBRARY)
#  define EEOCAMERASHARED_EXPORT Q_DECL_EXPORT
#else
#  define EEOCAMERASHARED_EXPORT Q_DECL_IMPORT
#endif

/**
 * Structure describes basic parameters of the device.
 */
typedef struct EeoAVDeviceInfo {
    QString device_name;           /**< device name, format depends on device */
    QString device_description;    /**< human friendly name */
} EeoAVDeviceInfo;

typedef struct EeoVideoInfo {
    int width = 0;
    int height = 0;
}EeoVideoInfo;

typedef struct EeoVideoDevCaps {
    QList<EeoVideoInfo> vidInfos;
}EeoVideoDevCaps;

enum EeoVideoResolution : int
{
    VIDEO_RES_UNKNOWN = -1,
    VIDEO_RES_SD = 0,
    VIDEO_RES_HD,
	VIDEO_RES_FHD,
	VIDEO_RES_WYSIWYG,
	VIDEO_RES_CLASSINCAM,
};

typedef enum _EeoCameraError
{
	ERROR_UNKNOWN = 1000,
	ERROR_CREAT_CONTEXT,
	ERROR_CREAT_VIDEO_RECORD,
	ERROR_VIDIE_NOT_WELL,
	ERROR_ADD_VIDEO_STREAM,
	ERROR_ADD_AUDIO_STREAM,
	ERROR_OPNE_FILE,
	ERROR_OPEN_AUDIO_STREAM,

	ERROR_CAN_NOT_GET_FIRST_A_STAMP = 1021,
	ERROR_CAN_NOT_GET_FIRST_V_STAMP,

	ERROR_AUDIO_NOT_WELL = 1031,

	//AVRECODER
	ERROR_AVRECORDER_GET_AUDIO = 2000,
	ERROR_AVRECORDER_WRITE_VIDEO,
	ERROR_AVRECORDER_WRITE_AUDIO,

	//VIDEO ENCODER
	ERROR_VIDEO_ENCODER_OPEN_CODEC = 3000,
	ERROR_VIDEO_ENCODER_ENCODE_FRAME = 3001,
	ERROR_VIDEO_HW_ENCODER_OPEN_CODEC = 3002,
	ERROR_VIDEO_HW_ENCODER_SET_PARAM = 3003,
	ERROR_VIDEO_HW_ENCODER_START = 3004,
    //VIDEO READER
    ERROR_VIDEO_READER_READ_FRAME = 4000,
    ERROR_VIDEO_READER_OPEN,
    ERROR_VIDEO_READER_FIND_STREAM,
    ERROR_VIDEO_READER_FIND_VIDEO,
    ERROR_VIDEO_READER_NOT_ACCESS,

    //VIDEO FILTER
    ERROR_VIDEO_FILTER_BUFFER_SINK = 5000,

	//WINDOW CHANGE
	ERROR_CHANGE_VIDEO_MODE = 6000,
	
	ERROR_IO_TIMEOUT = 8000,

	//EEO Airplay
	ERROR_AIRPLAY_START_TCP = 9000,
	ERROR_AIRPLAY_UDP_SOCKET,
	ERROR_AIRPLAY_TCP_SOCKET,
	ERROR_AIRPLAY_TCP_DISCON,
	ERROR_AIRPLAY_TCP_ACCEPT,
	ERROR_AIRPLAY_TCP_READ,
	ERROR_AIRPLAY_TCP_DECRYPT,
	ERROR_AIRPLAY_SYNC_LOST,

	ERROR_MOBILE_IOS_ERROR = 9100,	// spare for 100 at least.
	ERROR_MOBILE_ANDROID_ERROR = 9200,

    ERROR_CREAT_CONTEXT_RTMP = 11001,
    ERROR_CREAT_VIDEO_RECORD_RTMP,
    ERROR_VIDIE_NOT_WELL_RTMP,
    ERROR_ADD_VIDEO_STREAM_RTMP,
    ERROR_ADD_AUDIO_STREAM_RTMP,
    ERROR_OPNE_FILE_RTMP,
    ERROR_OPEN_AUDIO_STREAM_RTMP,

    ERROR_CAN_NOT_GET_FIRST_A_STAMP_RTMP = 11021,
    ERROR_CAN_NOT_GET_FIRST_V_STAMP_RTMP,

    ERROR_AUDIO_NOT_WELL_RTMP = 11031,

    //AVRECODER
    ERROR_AVRECORDER_GET_AUDIO_RTMP = 12000,
    ERROR_AVRECORDER_WRITE_VIDEO_RTMP,
    ERROR_AVRECORDER_WRITE_AUDIO_RTMP,

}EeoCameraError;

struct EeoLiveWebcamParams {
	std::string filename;
	uint64_t cid;
	int channelId;
	int width;
	int height;
	bool upload = true;
	EeoVideoResolution resolution;
	bool flip = true;
	float bitrateX = 1;
	int force_fps = 15;
	bool localAudio = false;
	//VIDEO_RES_WYSIWYG, open with specific mode;
	int openWidth = 0;
	int openHeight = 0;
};

class VideoboxFindInfo
{
public:
	QString m_response_sever_name;
	QString m_videobox_ip;
	int m_cmd_server_port;
	QString m_cmd_server_communicator_type;
};
Q_DECLARE_METATYPE(VideoboxFindInfo)

#endif // EEOCAMERA_GLOBAL_H

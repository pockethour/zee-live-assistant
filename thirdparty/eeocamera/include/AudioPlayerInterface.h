#ifndef __AUDIO_PLAYER_INTERFACE_H__
#define __AUDIO_PLAYER_INTERFACE_H__
#include "eeocamera_global.h"
#include <QObject>

typedef enum
{
	AUDIO_MIC,
	AUDIO_DESKTOP,
	AUDIO_APP,
	AUDIO_FILE,
	AUDIO_PCM,
	AUDIO_MIC_NETWORK,
	AUDIO_DESKTOP_NETWORK,
	AUDIO_MEM,
	AUDIO_VIRTUAL_MIC,
	AUDIO_NO_SUPPORT
}AudioPlayerType;

class upload_pcm_data {
public:
	upload_pcm_data()
		: size(0),
		data(NULL) {
	}

	upload_pcm_data(const upload_pcm_data &other)
		: size(other.size),
		data(NULL) {
		if (this->size > 0) {
			data = new unsigned char[this->size];
			memcpy(this->data, other.data, this->size);
		}
	}

	upload_pcm_data(const unsigned char *buf, int len)
		: size(len),
		data(NULL) {
		if (this->size > 0) {
			data = new unsigned char[this->size];
			memcpy(this->data, buf, len);
		}
	}

	~upload_pcm_data() {
		if (data) {
			delete[]data;
			data = NULL;
		}
	}

	int Size() const {
		return this->size;
	}

	const unsigned char  *Data() const {
		return this->data;
	}

public:
	int size;
	unsigned char  *data;
};

Q_DECLARE_METATYPE(upload_pcm_data);
class EEOCAMERASHARED_EXPORT AudioPlayer :public QObject
{
	Q_OBJECT
protected:
	 virtual ~AudioPlayer();
public Q_SLOTS:
	 void deleteLater();
public:
	explicit AudioPlayer(QObject *parent = 0);
	virtual void set_player_type(int playerType);
    virtual int get_player_type();
	void destroy();

	/*set exclusive mode, true: exclusive*/
	virtual void setExclusiveMode(bool mode);
    virtual bool getExclusiveMode();

	//get file duration, ms
    virtual int getDuration() { return 0; }
    virtual float getPlayedPos() { return 0; }
	
	//for class room, call it before open_audio
    virtual int setRemoteUserID(quint64 id);
    virtual int setChannelID(quint8 channelId);

	//In default virtual micphone is not recorded, if want to be record, call setRecordFlag(true)
	//call it after open_audio
    virtual void setRecordFlag(bool flag);
    virtual bool getRecordFlag();

	//if ClassRoomId is not set, the audio data will not be sent!
	virtual int setClassRoomID(quint64 id);

	//Currently audio player support only opening one device once, so when you want to open second device, the first device must be closed.
	virtual int open_audio(const QString &guid = NULL, bool closeAecFlag = false, bool encryptedFlag = false) = 0;
    virtual int close_audio() { return -1; }
    virtual int pause_audio() { return -1; }

	//control volume notify
    virtual int start_volume_notify() { return -1; }
	virtual int stop_volume_notify() { return -1; }
    virtual int stop_send_audio() { return -1; }
    virtual int start_send_audio() { return -1; }
    virtual int stop_listen_self_audio() { return -1; }
    virtual int start_listen_self_audio() { return -1; }

	//pcm player
    virtual int set_audio_player_parameter(char channels, char sample_size, int sample_rate){Q_UNUSED(channels) Q_UNUSED(sample_size) Q_UNUSED(sample_rate) return -1;}
    virtual int set_audio_player_data(unsigned char *pcm_data, int nb_sample){Q_UNUSED(pcm_data) Q_UNUSED(nb_sample) return -1;}
    virtual void setVolumeMultiplier(float multipler) { Q_UNUSED(multipler) }

	virtual int get_mic_volume(float &volume){ Q_UNUSED(volume) return -1;}
    virtual int set_mic_volume(float volume){ Q_UNUSED(volume) return -1;}
    virtual int mic_volume_is_available(bool &available){ Q_UNUSED(available) return -1;}

	// Microphone boost control
    virtual int mic_boost_is_available(bool &available){ Q_UNUSED(available) return -1;}
    virtual int set_mic_boost(bool enable){ Q_UNUSED(enable) return -1;}
    virtual int get_mic_boost(bool &enabled){ Q_UNUSED(enabled) return -1;}

	//Microphone mut control
    virtual int mic_mute_is_available(bool &available) { Q_UNUSED(available) return -1; }
    virtual int set_mic_mute(bool enable) { Q_UNUSED(enable) return -1; }
    virtual int get_mic_mute(bool &enabled) { Q_UNUSED(enabled) return -1; }

	virtual bool getAecClosedStatus() { return false; } //true:closed
    virtual void setUseRNN(bool flag) { Q_UNUSED(flag) }
    virtual bool getRNNUsed() { return false; }
    virtual void startSeek() {}
    virtual void stopSeek() {}

	/*agc( audio automatic gain) setting */
    virtual void setAgc(bool flag) { Q_UNUSED(flag) }
    virtual bool getAgc() { return false; }

    virtual void StartListenOSMicVolume() {}
    virtual void StopListenOSMicVolume() {}

signals:
	void volumeChanged(float volume); // the volume is between 0.0 and 1.0
	void micphoneReady();
	void micphoneOpenFailed();
	void aCaptureExcept(); //sharing aduio except, restart it.
	void playFileFinished();
	void reopen();
	void uploadPcmSig(const upload_pcm_data &data);
    void OSVolume_Mic(float fValue);
    
	// (0-100,  0:no loss, .... 100: all loss) 
	// -1:local network is bad, can not get the remote lossrate, so send it
	//-2: no meaning
	void lossRateEstimateValue(int val); 
	void pcmQueueEmpty();

public:
	long long  classRoomID;
	long long remoteUserID;
    int channelID = 0;
private:
	int _playType;
	bool _recordedFlag = false;
	bool _exclusiveModeFlag = false;
};

EEOCAMERASHARED_EXPORT AudioPlayer* CreateAudioPlayer(AudioPlayerType type, QString filename = NULL, float volume = 1.0, char *pdata = 0, int datalen = 0);
EEOCAMERASHARED_EXPORT int DeleleAudioPlayer(AudioPlayer* audioPlayer);
#endif // __AUDIO_PLAYER_INTERFACE_H__

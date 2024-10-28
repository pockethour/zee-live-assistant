#pragma once
#ifdef WIN32
#if defined(EEOCAMERA_LIBRARY)
#  define EEOCAMERASHARED_EXPORT __declspec(dllexport)
#else
#  define EEOCAMERASHARED_EXPORT __declspec(dllimport)
#endif
#else
#if defined(EEOCAMERA_LIBRARY)
#  define EEOCAMERASHARED_EXPORT __attribute__((visibility("default")))
#else
#  define EEOCAMERASHARED_EXPORT __attribute__((visibility("default")))
#endif
#endif // WIN32


EEOCAMERASHARED_EXPORT void*	AS_CreateAudioPlayer();
EEOCAMERASHARED_EXPORT int	    AS_set_audio_player_parameter(void* player, int channels, int sample_size, int sample_rate);
EEOCAMERASHARED_EXPORT int	    AS_set_audio_player_data(void* player, unsigned char* pcm_data, int nb_sample);
EEOCAMERASHARED_EXPORT void	    AS_setVolumeMultiplier(void* player, float multipler);
EEOCAMERASHARED_EXPORT void	    AS_setExclusiveMode(void* player, bool bExclusive);
EEOCAMERASHARED_EXPORT int	    AS_DeleleAudioPlayer(void* player);



EEOCAMERASHARED_EXPORT void* AS_CreateVMAudioPlayer();
EEOCAMERASHARED_EXPORT int AS_set_vm_audio_player_parameter(void* player, int channels, int sample_size, int sample_rate);
EEOCAMERASHARED_EXPORT int AS_set_vm_audio_player_data(void* player, unsigned char* pcm_data, int nb_sample);
EEOCAMERASHARED_EXPORT int AS_DeleleVMAudioPlayer(void* player);
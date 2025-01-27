#ifndef IMAGE_EFFECT_LIBRARY_H
#define IMAGE_EFFECT_LIBRARY_H
#include "export.h"
#include <QImage>
#include "ParamTypes.h"
enum class ImageFilterType;
class ImgManager;
class EXPORT OpenglImageManager
{
public:
    static OpenglImageManager* instance();
    void addChannelImageFilter(int sceneId, int channelId, ImageFilterType t, void* value);
    int genSceneFilters();
    int genChannelFilters(int sceneId, bool needMerge = true);
    void setChannelFilterParas(int sceneId, int channelId, ImageFilterType t, FilterParaType para, void* value);
    void sendChannelFrame(int sceneId, int channelId, const QImage& input, bool isDefault = false, bool isChanged = true, bool isSourceError = false);
    void mergeImages(int sceneId, const QList<ChannelMergeParam>& channelList, int width, int height, const QColor& bgColor, const QImage& graffiti, const QImage& waterMark);
    bool getMergeImg(int sceneId, QImage& mergeImg);

private:
    OpenglImageManager();
    ~OpenglImageManager();
    OpenglImageManager(const OpenglImageManager&) = delete;
    OpenglImageManager& operator =(const OpenglImageManager&) = delete;
    OpenglImageManager(const OpenglImageManager&&) = delete;
    OpenglImageManager& operator =(const OpenglImageManager&&) = delete;
    ImgManager* imgManager = nullptr;
};

#endif //IMAGE_EFFECT_LIBRARY_H

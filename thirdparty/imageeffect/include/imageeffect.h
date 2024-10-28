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
    int genSceneFilters() const;
    int genChannelFilters(int sceneId, bool needMerge = true) const;
    void addChannelImageFilter(int sceneId, int channelId, ImageFilterType t, void* value) const;
    void setChannelFilterParas(int sceneId, int channelId, ImageFilterType t, FilterParaType para, void* value) const;
    void sendChannelFrame(int sceneId, int channelId, const QImage& input, bool isDefault = false, bool isChanged = true, bool isSourceError = false) const;
    void mergeImages(int sceneId, const QList<ChannelMergeParam>& channelList, int width, int height, const QColor& bgColor, const QImage& graffiti, const QImage& waterMark) const;
    bool getMergeImg(int sceneId, QImage& mergeImg) const;

    OpenglImageManager(const OpenglImageManager&) = delete;
    OpenglImageManager& operator =(const OpenglImageManager&) = delete;
    OpenglImageManager(const OpenglImageManager&&) = delete;
    OpenglImageManager& operator =(const OpenglImageManager&&) = delete;

private:
    OpenglImageManager();
    ~OpenglImageManager();
    ImgManager* imgManager = nullptr;
};

#endif //IMAGE_EFFECT_LIBRARY_H

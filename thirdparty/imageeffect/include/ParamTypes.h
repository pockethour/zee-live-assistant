#ifndef PARAMTYPES_H
#define PARAMTYPES_H
#include <string>
#include <QImage>
typedef struct _redraw_param {
    QRect videoRect;
    QImage videoImage;
    qreal qVideoRotation = 0;
}redraw_param;

typedef struct _channel_merge_param {
    int id;
    QRect rect;
    qreal rotation = 0;
}ChannelMergeParam;

typedef struct _channel_ani_param {
    int id;
    QRect rect;
    qreal rotation = 0;
    QImage img;
}ChannelAniParam;

typedef struct _beauty_basic_value
{
    double scale = 0.0;
    double brightness = 0.0;
    double contrast = 0.0;
    double saturation = 0.0;
    double alpha = 1.0;
    double xRotation = 0.0;
    double yRotation = 0.0;
    double zRotation = 0.0;
    int isMirrored = 0;
    int cornerRadius = 0;
    int isRoundCamera = 0;
    int isClip = 0;
    double channelWidth = 1280;
    double channelHeight = 720;
    double resolutionWidth = 1280;
    double resolutionHeight = 720;
    double centerXOffset = 0.5;
    double centerYOffset = 0.5;
    double firstPageW = 0;
    double firstPageH = 0;
    int fileType = 0; //0:other,1:camera,2:img/pdf/word,3:video,4: screenshare
}BeautyBasicValue;


typedef struct _beauty_smooth_value
{
    double strength = 0;
    bool isFaceDetect = false;
    std::string faceDetectModelPath = "";
}BeautySmoothValue;

typedef struct _beauty_redlips_value
{
    double strength = 0;
    bool isFaceDetect = false;
    std::string faceDetectModelPath = "";
}BeautyRedLipsValue;

typedef struct _beauty_whitening_value
{
    double strength = 0;
    bool isFaceDetect = false;
    std::string faceDetectModelPath = "";
}BeautyWhiteningValue;

typedef struct _beauty_smooth_whitening_value {
    double strength = 0;
    bool isFaceDetect = false;
    std::string faceDetectModelPath = "";
}BeautySmoothWhiteningValue;

typedef struct _beauty_slim_face_value
{
    double strength = 0;
    std::string faceDetectModelPath = "";
}BeautySlimFaceValue;

typedef struct _beauty_sticker_value
{
    std::string stickerImgPath = "";
    bool isFaceDetect = true;
    std::string faceDetectModelPath = "";
}BeautyStickerValue;

typedef struct _bg_ai_paddle_value
{
    std::string bgImagePath;
    std::string modelPath;
}BgAIPaddleValue;

typedef struct _bg_d3d_value
{
    std::string bgImagePath;
}BgD3DValue;

typedef struct _bg_ai_dnn_value
{
    std::string bgImagePath;
    bool isBgBlured = false;
    bool isBgTransparent = false;
    bool isGreenScreen = false;
    int similarity = 400;
    int smoothness = 80;
    int spill = 100;
}BgAIDnnValue;

typedef struct _bg_ai_3DImage_value
{
    std::string modelPath;
    std::string bgImagePath;
    std::string loadingImgPath;
    bool isPaused = false;
    bool isBgBlured = false;
    bool isBgTransparent = false;
}BgAI3DImageValue;

typedef struct _bg_hsv_value
{
    int hsvH;
    int hsvS;
    int hsvV;
    std::string bgImagePath;
}BgHsvValue;

typedef struct _bg_ec_value
{
    std::string bgImagePath;
}BgEcValue;

typedef struct _bg_opengl_value
{
    int rgbR;
    int rgbG;
    int rgbB;
    int similarity;
    int smoothness;
    int spill;
    std::string bgImagePath;
}BgOpenglValue;

typedef struct _face_detect_value
{
    std::string modelPath;
}FaceDetectValue;

#ifdef Q_OS_WIN
enum MergeType {
#else
enum class MergeType {
#endif
    NO_ANIMATION = -1,
    FADE_IN_FADE_OUT,
    RIGHT_IN_LEFT_OUT,
    APERTURE,
    WIPE,
    OPEN_DOOR,
    SHRED,
    FLY_OUT_FLY_IN
};

#ifdef Q_OS_WIN
enum OutFormat {
#else
enum class OutFormat {
#endif
    BGR24 = 0,
    NV12,
    UYVY,
};

typedef struct _merge_img_param
{
    QImage srcImage;
    QImage dstImage;
    MergeType mergeType;
    int progress = 0;
    QRect chanRect;
    int dstType = 0;//0:other,1:camera,2:pdf/word,3:round camera,4:pdf/word no clip
    int chanWidth = 1280;
    int chanHeight = 720;
    int moveDirection = 0;
    QList<ChannelAniParam> srcChanList;

}MergeImgParam;


enum class FilterParaType {
    //basicFilter
    BtBasicStrength = 0,

    //beauty filter
    BtSmoothStrength,
    BtRedLipsStrength,
    BtWhiteningStrength,
    BtSlimFaceStrength,
    BtSmoothWhiteningStrength,//for English version

    //stickerPath
    BtStickerImagePath,

    //background filter
    BgAIPaddleImagePath = 50,
    BgAIPaddleModelPath,
    BgAIDnnImagePath,
    BgAIDnnModelPath,
    BgAI3DImageModelPath,
    BgHsvImagePath,
    BgHsvH,
    BgHsvS,
    BgHsvV,
    BgOpenglImagePath,
    BgOpenglRgbR,
    BgOpenglRgbG,
    BgOpenglRgbB,
    BgOpenglSimilarity,
    BgOpenglSmooth,
    BgOpenglSpill,
    BgGreenScreenValue,

    //other
    BgHsvAll = 100,
    BgOpenglAll,
    FaceDetModelPath
};

enum class ImageFilterType
{
    Image_Beauty_NoFilter,
    Image_Beauty_SlimFace,
    Image_Beauty_Smooth,
    Image_Beauty_RedLips,
    Image_Beauty_Whitening,
    Image_Beauty_Sticker,
    Image_Beauty_Basic,
    Image_Bg_Green_Hsv,
    Image_Bg_Green_Opengl,
    Image_Bg_EC,
    Image_Bg_AI_Paddle,
    Image_Bg_D3D,
    Image_Bg_AI_Dnn,
    Image_Bg_AI_3DImage,
    Image_Beauty_Smooth_Whitening, //for English version
};
#endif // PARAMTYPES_H

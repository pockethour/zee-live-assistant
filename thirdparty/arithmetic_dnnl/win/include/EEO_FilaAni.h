
/*************************************************
Copyright:  EEO
Author:     zhengdao.wang
Date:       2023-11-15
Description:Read the encrypted gltf file to include the 3D avatar
        Generate face expression based on user input control parameters,
        And ultimately output a rendered image with transparent channels.
        (If you use the same size when switching between different 3D avatars, 
        you can achieve faster switching speed.)
**************************************************/

#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef EEO_FILA_ANI_EXPORT
#define CEEO_FILA_ANI_API __declspec(dllexport)
#else
#define CEEO_FILA_ANI_API __declspec(dllimport)
#endif
#else //macos
#define CEEO_FILA_ANI_API
#endif

#ifndef LOGLEVEL
#define LOGLEVEL
namespace LOG
{
    enum LEVEL{
        LOG_ANY = 0,
        LOG_ERROR = 100,
        LOG_WARNING = 200,
        LOG_INFO = 300,
        LOG_DEBUG = 400,
        LOG_NONE = 500
    };
}
#endif

class CEEO_FILA_ANI_API CEeoFilaAni {

public:

    /***************************************************************
    *  @brief       Thread waits for a certain number of milliseconds
    *  @param
    *  @note        
    *  @Sample usage:  CEeoFilaAni::sleepMs(33); //30fps
    **************************************************************/
    static void sleepMs(int iMs);

    /***************************************************************
    *  @brief       Obtain the state of class CEeoFilaAni
    *  @param
    *  @note        return 0-5
                    0:not initialized, or end deinit
                    1:start init
                    2:end init
                    3:start draw
                    4:end draw
                    5:start deinit
    *  @Sample usage:  int iState = CEeoFilaAni::getState();
    **************************************************************/
    static int getState();

    /***************************************************************
    *  @brief       Obtain the version number of the dynamic library
    *  @param       
    *  @note        return in the form of "1.1.0"
    *  @Sample usage:  cout << "CEeoFilaAni ver:" << CEeoFilaAni::getVer() << endl;
    **************************************************************/
    static const char* getVer();

    /***************************************************************
    *  @brief       >= blockLogLevel will be blocked
    *  @param       LOG_ANY LOG_ERROR LOG_WARNING LOG_INFO LOG_DEBUG LOG_NONE
    *  @note        
    *  @Sample usage:  CEeoFilaAni::setBlockLogLevel(LOG::LOG_DEBUG);
    **************************************************************/
    static void setBlockLogLevel(LOG::LEVEL iLogLevel);

    /***************************************************************
    *  @brief       <= writeLogLevel will be written into a file
    *  @param       LOG_ANY LOG_ERROR LOG_WARNING LOG_INFO LOG_DEBUG LOG_NONE
    *  @note
    *  @Sample usage:  CEeoFilaAni::setWriteLogLevel(LOG::LOG_INFO);
    **************************************************************/
    static void setWriteLogLevel(LOG::LEVEL iLogLevel);

    /***************************************************************
    *  @brief       Decrypt file and init as gltf/glb
    *  @param       gltfFileName:file path and name
                        (is file not found, return false)
                    w:set output image width (1-4096)
                    h:set output image height (1-4096)
                    iImgQuality:value meaning:0 fast; 1 balanced; 2 good; 3 perfect(slow).
    *  @note        If you need to change the Gitf file or the size of image, 
                    simply call this function again, not need to call deinit first;
                    Note that the final init will affect all subsequent getImgXXX.
                    (After changes in w or h, initEncryptedFile will slow down;
                    after deinit, initEncryptedFile will become much slower)
    *  @Sample usage:  CEeoFilaAni:initEncryptedFile("./man.bin", 640, 360);
    **************************************************************/
    static bool initEncryptedFile(const char* gltfFileName, int w = 640, int h = 360,
        int iImgQuality = 1);

    /***************************************************************
    *  @brief       Obtain rendered 3D avatars
    *  @param       f60in: Refer to the strAi[60] above
                    iFloatNum:size of f60in (Commonly used [0]-[57])
                    dstImg:destination, rendered target image (null: built-in)
                    outW:If the pointer is valid, output dstImg width
                    outH:If the pointer is valid, output dstImg height
    *  @note        Obtain images in rgba8 format (Internally set a time interval of 30ms);
                    when parameter f60in is empty, use the default emoticon;
                    the obtained image width and height refer to the function initEncryptedFile.
    *  @Sample usage:  auto imgRGBA8Data = CEeoFilaAni::getImgRGBA8();
    **************************************************************/
    static unsigned char* getImgRGBA8(float* f60in = nullptr, int iFloatNum = 58,
        unsigned char* dstImg = nullptr, int* outW = nullptr, int* outH = nullptr);

    /***************************************************************
    *  @brief       Obtain rendered 3D avatars
    *  @param       f60in: Refer to the strAi[60] above
                    iFloatNum:size of f60in (Commonly used [0]-[57])
                    backImg:background image (null: black ground)
                    backImgChannel: Channel number of backImg (rgb: 3; rgba: 4)
                    backImgW:If the value is not 0, input backImg width
                    backImgH:If the value is not 0, input backImg height
                    (If the w or h are inconsistent with the "initEncryptedFile", 
                        the background image will be black)
                    dstImg:destination, rendered target image (null: built-in)
                    outW:If the pointer is valid, output dstImg width
                    outH:If the pointer is valid, output dstImg height
    *  @note        Obtain images in rgb8 format (Internally set a time interval of 30ms);
                    when parameter f60in is empty, use the default emoticon;
                    the obtained image width and height refer to the function initEncryptedFile.
    *  @Sample usage:  auto imgRGB8Data = CEeoFilaAni::getImgRGB8withBackImg();
    **************************************************************/
    static unsigned char* getImgRGB8withBackImg(float* f60in = nullptr, int iFloatNum = 58,
        unsigned char* backImg = nullptr, int backImgChannel = 3,
        int backImgW = 0, int backImgH = 0,
        unsigned char* dstImg = nullptr, int* outW = nullptr, int* outH = nullptr);

    /***************************************************************
    *  @brief       Obtain rendered 3D avatars
    *  @param       f60in: Refer to the strAi[60] above
                    iFloatNum:size of f60in (Commonly used [0]-[57])
                    backImgRGBA8:background image (null: transparent ground)
                    backImgChannel: Channel number of backImg (currently, it must be 4)
                    backImgW:If the value is not 0, input backImg width
                    backImgH:If the value is not 0, input backImg height
                    (If the w or h are inconsistent with the "initEncryptedFile",
                        the background image will be completely transparent)
                    dstImg:destination, rendered target image (null: built-in)
                    outW:If the pointer is valid, output dstImg width
                    outH:If the pointer is valid, output dstImg height
    *  @note        Obtain images in rgba8 format (Internally set a time interval of 30ms);
                    when parameter f60in is empty, use the default emoticon;
                    the obtained image width and height refer to the function initEncryptedFile.
    *  @Sample usage:  auto imgRGBA8Data = CEeoFilaAni::getImgRGBA8withBackImg();
    **************************************************************/
    static unsigned char* getImgRGBA8withBackImg(float* f60in = nullptr, int iFloatNum = 58,
        unsigned char* backImgRGBA8 = nullptr, int backImgChannel = 4,
        int backImgW = 0, int backImgH = 0,
        unsigned char* dstImg = nullptr, int* outW = nullptr, int* outH = nullptr);

    /***************************************************************
    *  @brief       Obtain rendered 3D avatars with body
    *  @param       f60in: Refer to the strAi[60] above
                    iFloatNum:size of f60in (Commonly used [0]-[59]:[58]scale,[59]xy)
                    bodyImg:background image with body (not null)
                    bodyImgChannel: Channel number of backImg (rgb: 3; rgba: 4)
                    bodyImgW:input backImg width
                    bodyImgH:input backImg height
                    (Allow w and h to differ from "initEncryptedFile",
                        but it needs to be the same ratio as the face parameter img)
                    dstImg:destination, rendered target image (null: built-in)
                    outW:If the pointer is valid, output dstImg width
                    outH:If the pointer is valid, output dstImg height
    *  @note        Obtain images in rgb8 format (Internally set a time interval of 30ms);
                    the obtained image width and height refer to "bodyImgW" and "bodyImgH".
    **************************************************************/
    static unsigned char* getImgRGB8withBody(float* f60in, int iFloatNum,
        unsigned char* bodyImg, int bodyImgChannel = 3,
        int bodyImgW = 1280, int bodyImgH = 720,
        unsigned char* dstImg = nullptr, int* outW = nullptr, int* outH = nullptr);

    /***************************************************************
    *  @brief       Obtain rendered 3D avatars with body
    *  @param       f60in: Refer to the strAi[60] above
                    iFloatNum:size of f60in (Commonly used [0]-[59]:[58]scale,[59]xy)
                    bodyImg:background image with body
                    bodyImgChannel: Channel number of backImg (rgb: 3; rgba: 4)
                    bodyImgW:input bodyImg width
                    bodyImgH:input bodyImg height
                    (Allow w and h to differ from "initEncryptedFile",
                        but it needs to be the same ratio as the face parameter img)
                    dstImg:destination, rendered target image (null: built-in)
                    outW:If the pointer is valid, output dstImg width
                    outH:If the pointer is valid, output dstImg height
    *  @note        Obtain images in rgba8 format (Internally set a time interval of 30ms);
                    the obtained image width and height refer to "bodyImgW" and "bodyImgH".
    **************************************************************/
    static unsigned char* getImgRGBA8withBody(float* f60in, int iFloatNum,
        unsigned char* bodyImg, int bodyImgChannel = 3,
        int bodyImgW = 1280, int bodyImgH = 720,
        unsigned char* dstImg = nullptr, int* outW = nullptr, int* outH = nullptr);

    /***************************************************************
    *  @brief       Write image data to a png file (does not need init)
    *  @param       imgData:image buffer;
                    pngFileName:path and name of target png file
                    w:need to match the width of the input image
                    h:need to match the height of the input image
                    n:RGBA8 format image corresponds to 4; RGB8 corresponds to 3
                    flipV:determine whether the image needs to be flipped up and down
    *  @note        Null images will be skipped,
                    pngFileName can be passed in as a number from 0 to 999.
    *  @Sample usage:  CEeoFilaAni::writePngFile(imgRGBA8Data, "./out.png", 1280, 720, 3);
    **************************************************************/
    static bool writePngFile(unsigned char* imgData,
        const char* pngFileName = "", int w = 1280, int h = 720, int n = 4, bool flipV = false);

    /***************************************************************
    *  @brief       Deinitialization,release most resources
    *  @param
    *  @note        After calling deinit, if you want to use getImgXXX,
                    you need to call initEncryptedFile before getImgXXX;
                    it is recommended to only call this function
                    when 3D avatars are not used for a long time.
    *  @Sample usage:  CEeoFilaAni::deinit();
    **************************************************************/
    static void deinit();

    /***************************************************************
    *  @brief       Notify internal rendering thread to shut down.
    *  @param
    *  @note        Called only before the process is closed.
    *  @Sample usage:  CEeoFilaAni::myExit();
    **************************************************************/
    static void myExit();

};

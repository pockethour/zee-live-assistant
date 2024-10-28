//
//  Created by AI group on 2022/07/06.
//  Copyright (c) 2022, EEO CORPORATION System group. All rights reserved.
//

#pragma once

#ifdef Q_OS_WIN32
#ifdef CLASSDLLAPIEXPORT
#else
#define CLASSDLLAPIEXPORT _declspec(dllexport)
#endif
#else
#define CLASSDLLAPIEXPORT
#endif

#include <opencv2/opencv.hpp>
using namespace std;

#define EEO_DNNL_VER "v24.05.30.15"
CLASSDLLAPIEXPORT const char* getDnnlVer();

//This value indicates that the GPU cutout 
//test for the virtual background has been successful
#define RTV_FOR_MAIN_SUCCESS 7758521

namespace EEOAI {

/** device type for ai algorithm computation */
enum ComputeDeviceType {
  CPU0 = 0,
  /** for cpu computation. Best for strong cpu device */
  CPU = 1,
  /** will be surport */
  OPENCL = 2,
  /** for gpu computation. Best for windows system */
  VULKAN = 3,
  /** for gpu computation. Best for macos system */
  METAL = 4
};

//If the algorithm is modified in the future, 
//it may output less content to improve computational efficiency
enum VB_OUT {
    NOTHING = 0,
    COMPOSIT = 1,
    MASK = (1 << 1),
    ELSE = (1 << 2)
};

/** gpu mode type only for opencl device */
enum GpuModeType { 
    MODEPLACEHOLDER = 0,  
    //MODE_BUF = (1 << 6),
    MODE0 = (1 << 7), 
    MODE1 = (1 << 8)  //After testing, the three modes have similar speeds

    //By reading file "MNNForwardType.h", judging that the above definitions may have deviations
    //MNN_GPU_MEMORY_BUFFER = 1 << 6,/* User assign mode */
    //MNN_GPU_MEMORY_IMAGE = 1 << 7,/* User assign mode */
    // ..
    //scheduleConfig.mode = gpumode | MNN_GPU_TUNING_FAST;
};

/**
 * virtual background algorithm class.
 */

class CLASSDLLAPIEXPORT VirtualBackground {
 public:
  /**
   * @brief construct a VirtualBackground  object for replace  background.
   * @param hardwareAccSupport hardware acceleration support flag. 
   *    True->gpu suport hardware acc when set deviceType other than CPU
   * @param deviceType   compute device type.
   * @param numThread    thread number for computing.
   * @param gpuMode      gpu mode only for opencl device type.
   * @param winPreLoadPath preload diretory for opencl device type.eg. windows:
   * "C:\\Users\\${username}\\AppData\\Roaming\\${AppName}\\", 
   * macos: /User/${username}/Library/Application Surport/${AppName}.
   */
  explicit VirtualBackground( bool & hardwareAccSupport,
                              const ComputeDeviceType deviceType = VULKAN,
                              const unsigned numThread = 1,
                              GpuModeType gpuMode = GpuModeType::MODE0,
                              const char* winPreLoadPath = "");

  /**
   * @brief deconstruct a VirtualBackground  object for replace  background.
   */
  ~VirtualBackground();

 public:


    ///////////////////////////////////////////////////////////////////////
    //It is recommended to use the new interface below. 
    //The interface above will be used as a transition 
    //and will be discarded in the future

    /**
    * @brief implement virtrual background function.
    * @param inImg: input image(Recommended resolution 720p, 
    *   type only support RGB888, RGBA8888) .
    * @param backImg: back image(resolution same as inImg, type same as inImg ).
    * @param outType: does customization require outputting CompositImg and MaskImg.
    * @return composit image and portrait binary mask
    * (composit resolution same as inImg,  type is RGBA8888;
    *  mask resolution same as inImg, type is Gray).
    */
    bool Process(const cv::Mat& inImg, const cv::Mat& backImg, 
        cv::Mat& outCompositImg, cv::Mat& outMaskImg,
        int outType = (VB_OUT::COMPOSIT | VB_OUT::MASK));

    bool ProcessFast(const cv::Mat& inImg144, 
        cv::Mat& outMaskImg, const cv::Size outMaskSize);

    /**
    * @brief implement background blur function.
    * @param inImg: input image(Recommended resolution 720p, 
    *   type only support RGB888, RGBA8888) .
    * @param outType: does customization require outputting CompositImg and MaskImg.
    * @return composit image and portrait binary mask
    * (composit resolution same as inImg,  type is RGBA8888;
    *  mask resolution same as inImg, type is Gray).
    */
    bool BackgroundBlur(const cv::Mat& inImg, 
        cv::Mat& outCompositImg, cv::Mat& outMaskImg,
        int outType = (VB_OUT::COMPOSIT | VB_OUT::MASK));

    ///////////////////////////////////////////////////////////////////////

 private:
  void* m_pImpl = nullptr;
  ComputeDeviceType m_DeviceType;
  bool m_hardwareAccSupport = false;
  int m_iCatchErrorNumber = 0;

  // remove all assignment operator
  VirtualBackground(const VirtualBackground& ) = delete;
  VirtualBackground(const VirtualBackground&& ) = delete;
  VirtualBackground& operator=(const VirtualBackground&) = delete;
  VirtualBackground& operator=(const VirtualBackground&&) = delete;
};

class CLASSDLLAPIEXPORT FaceDetector {
 public:
  /**
   * @brief construct a FaceDetector object for detecting face.
   * @param deviceType: compute device type.
   * @param numThread: thread number for computing.
   */
  explicit FaceDetector(const ComputeDeviceType deviceType = CPU,
                        const unsigned numThread = 1,
                        const unsigned maxNumFace = 1);

  /**
   * @brief deconstruct a FaceDetector  object for detecting  face.
   */
  ~FaceDetector();

 public:
  typedef struct {
    std::vector<int> landMark;
    std::vector<int> box;
    std::vector<int> poseAngle;
    std::vector<float> animoji;
    std::vector<float> mesh;
    std::vector<float> expressions;
    float tongue_out;
    float confidence;
  } Face;
  typedef vector<Face> Faces;
  /**
   * @brief Detect faces in an image and optionally detect facial expressions or animoji features.
   * @param inImg: Input image  (Recommended resolution 720p, type only support RGB888) in which to detect faces .
   * @param detect_expression: If true, detect facial expression in addition to face detection. Default is false.
   * @param is_animoji: If true, detect animoji features like eyes, nose and mouth in addition to face detection. Default is false.
   * @param face_detect_interval:  interval frame for every face detection. Default is 25.
   * @param face_landmark_detect_interval: interval frame for every facial landmark detection. Default is 2.
   * @param smooth_landmark: Increasing smooth_landmark coefficient decreases speed lag while increasing slow speed jitter. Default is 1.0.
   * @return value: detected faces in inImg
   */
  Faces& Run(const cv::Mat& inImg, bool detect_expression=false, bool is_animoji=false,
             int face_detect_interval=25, int face_landmark_detect_interval=2,
             float smooth_landmark = 1.0);

 private:
  void* m_pImpl = nullptr;
  ComputeDeviceType m_DeviceType;
  Faces m_Faces;
  unsigned m_maxNumFace;

  // remove all assignment operator
  FaceDetector(const FaceDetector& ) = delete;
  FaceDetector(const FaceDetector&& ) = delete;
  FaceDetector& operator=(const FaceDetector&) = delete;
  FaceDetector& operator=(const FaceDetector&&) = delete;
};

CLASSDLLAPIEXPORT
void getGpuDevices(vector<string>& gpuDevices);

}  // namespace EEOAI

namespace OptSimdOrCV {
/*Optimize simd when conditions are suitable,
  all method suport in-place mode*/
CLASSDLLAPIEXPORT
void copy(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void resizeBilinear(const cv::Mat& src, cv::Mat& dst, cv::Size size);
CLASSDLLAPIEXPORT
void resizeB_144(const cv::Mat& src_img, cv::Mat& out_img,
  int outW = 256, int outH = 144);

CLASSDLLAPIEXPORT
void RgbaToRgb(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void RgbaToBgr(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void RgbaToBgra(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void RgbToRgba(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void RgbToBgra(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void RgbToBgr(const cv::Mat& src, cv::Mat& dst);

CLASSDLLAPIEXPORT
void BgraToBgr(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void BgraToRgb(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void BgraToRgba(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void BgrToBgra(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void BgrToRgba(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void BgrToRgb(const cv::Mat& src, cv::Mat& dst);
}  // namespace OptSimdOrCV

namespace AILog {
/** priority definition*/
enum {
  /**
   * Use if there's a problem that can potentially affect the program,
   * but isn't enough to require termination of the program.
   *
   * Use in creation functions and core subsystem functions.  Places that
   * should definitely not fail.
   */
  LOG_ERROR = 100,

  /**
   * Use if a problem occurs that doesn't affect the program and is
   * recoverable.
   *
   * Use in places where failure isn't entirely unexpected, and can
   * be handled safely.
   */
  LOG_WARNING = 200,

  /**
   * Informative message to be displayed in the log.
   */
  LOG_INFO = 300,

  /**
   * Debug message to be used mostly by developers.
   */
  LOG_DEBUG = 400
};
/** log callback*/
typedef void (*LogCallBack)(int priority, const char* msg);
extern LogCallBack g_logCallBack;
CLASSDLLAPIEXPORT void setLogCallBack(LogCallBack logCallBack);
CLASSDLLAPIEXPORT void log(int level, const char *format, ...);
}  // namespace AILog

namespace ECryptAlg
{
//////////////////////////////////////////////////////////////////////////
//
enum ECrypt {
    CRYPT_TYPE_NONE = 0,    // no encypt
    CRYPT_TYPE_1 = 1,       // method 1
    CRYPT_TYPE_2 = 2        // method 2
};

//////////////////////////////////////////////////////////////////////////
CLASSDLLAPIEXPORT 
short GetPubKeysNum();
CLASSDLLAPIEXPORT
short GetPubKeyIdx();
CLASSDLLAPIEXPORT
bool EncryptByPubKey(char* buf, unsigned int len, short pubKeyIdx, ECrypt type = CRYPT_TYPE_NONE);
CLASSDLLAPIEXPORT
bool DecryptByPubKey(char* buf, unsigned int len, short pubKeyIdx, ECrypt type = CRYPT_TYPE_NONE);

//////////////////////////////////////////////////////////////////////////

} // ECryptAlg

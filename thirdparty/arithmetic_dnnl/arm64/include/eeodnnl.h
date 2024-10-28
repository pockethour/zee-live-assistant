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

namespace EEOAI {

/** device type for ai algorithm computation */
enum ComputeDeviceType {
  CPU0 = 0,
  /** for cpu computation. Best for strong cpu device */
  CPU = 1,
  /** will be surport */
  OPENCL,
  /** for gpu computation. Best for windows system */
  VULKAN,
  /** for gpu computation. Best for macos system */
  METAL
};
/** gpu mode type only for opencl device */
enum GpuModeType { MODEPLACEHOLDER=0,  MODE0 = 1 << 7, MODE1 = 1 << 8 };

/**
 * virtual background algorithm class.
 */

class CLASSDLLAPIEXPORT VirtualBackground {
 public:
  /**
   * @brief construct a VirtualBackground  object for replace  background.
   * @param hardwareAccSupport hardware acceleration support flag. True->gpu suport hardware acc when set deviceType other than CPU
   * @param deviceType   compute device type.
   * @param numThread    thread number for computing.
   * @param gpuMode      gpu mode only for opencl device type.
   * @param winPreLoadPath preload diretory for opencl device type.eg. windows:
   * "C:\\Users\\${username}\\AppData\\Roaming\\${AppName}\\", macos: /User/${username}/Library/Application Surport/${AppName}.
   */
  explicit VirtualBackground( bool & hardwareAccSupport,
      const ComputeDeviceType deviceType = VULKAN, const unsigned numThread = 1,
      GpuModeType gpuMode = GpuModeType::MODE0,
                             const char* winPreLoadPath = "");

  /**
   * @brief deconstruct a VirtualBackground  object for replace  background.
   */
  ~VirtualBackground();

  typedef struct {
    cv::Mat composit;
    cv::Mat mask;
  } VirtualBackgroundRes;

 public:
  /**
   * @brief implement virtrual background function.
   * @param inImg     input image(Recommended resolution 720p, type only support
   * RGB888, RGBA8888) .
   * @param backImg   back image(resolution same as inImg, type same as inImg )
   * .
   * @return          VirtualBackgroundRes include composit image and  portrait
   * binary mask( composit resolution same as inImg,  type is RGBA8888; mask
   * resolution same as inImg, type is Gray).
   */
  VirtualBackgroundRes Process(const cv::Mat& inImg, const cv::Mat& backImg);

  /**
   * @brief implement  background blur function.
   * @param inImg     input image(Recommended resolution 720p, type only support
   * RGB888, RGBA8888) .
   * @return          VirtualBackgroundRes include composit image and  portrait
   * binary mask( composit resolution same as inImg,  type is RGBA8888; mask
   * resolution same as inImg, type is Gray).
   */
  VirtualBackgroundRes BackgroundBlur(const cv::Mat& inImg);

 private:
  void* m_pImpl = nullptr;
  ComputeDeviceType m_DeviceType;
  bool m_hardwareAccSupport;

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
   * @param deviceType   compute device type.
   * @param numThread    thread number for computing.
   */
  explicit FaceDetector(const ComputeDeviceType deviceType = CPU,
                        const unsigned numThread = 1,
                        const unsigned maxNumFace = 3);

  /**
   * @brief deconstruct a FaceDetector  object for detecting  face.
   */
  ~FaceDetector();

 public:
  typedef struct {
    std::vector<int> landMark;
    std::vector<int> box;
    std::vector<int> poseAngle;
    float confidence;
  } Face;
  typedef vector<Face> Faces;
  /**
   * @brief implement face detection function.
   * @param inImg     input image(Recommended resolution 720p, type only support
   * BGR888) .
   * @return value    detected faces in inImg
   */
  Faces& Run(const cv::Mat& inImg);

  /**
  * @brief `Stop` function responsible for stopping the face detection process.
  * @param
  * @return
  */
  void Stop();

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

}  // namespace EEOAI

namespace OptimizeCV {
/*all method not suport in-place mode*/
CLASSDLLAPIEXPORT
void resizeBilinear(const cv::Mat& src, cv::Mat& dst,                     cv::Size size);
CLASSDLLAPIEXPORT
void BgrToRgb(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void RgbaToRgb(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void RgbaToBgr(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void RgbToRgba(const cv::Mat& src, cv::Mat& dst);
CLASSDLLAPIEXPORT
void copy(const cv::Mat& src, cv::Mat& dst);
}  // namespace OptimizeCV

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
    CRYPT_TYPE_1,           // method 1
    CRYPT_TYPE_2            // method 2
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

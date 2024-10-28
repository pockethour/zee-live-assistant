#ifndef RGBRENDERER_H
#define RGBRENDERER_H

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOffscreenSurface>


class RgbRenderer: public QOpenGLFunctions
{
public:
    RgbRenderer();
    ~RgbRenderer();
    void drawFrame(const unsigned char* srcImage,int srcWidth,int srcHeight);
    void drawMergeFrame(const unsigned char* srcImage,const unsigned char* srcImage2,int srcWidth,int srcHeight);

protected:

    void initializeGL();
    void initPBO(int width,int height,int scaleX,int scaleY);
    bool initShaders();
    void initGeometry();
    void drawGeometry();
    void initTexture();
    void rotateMatrix(float angle, float x, float y, float z );
    void initMatrix();
private:

    bool isInit=false;
    //材质内存空间
    unsigned char *datas[3];
    unsigned char *alldata;
    QOpenGLShaderProgram program;

    //shader中yuv变量地址
    GLuint unis[3];
    //opengl的 texture地址
    GLuint texs[3];
    GLuint mvpMatrix;
    QMatrix4x4 mvp_matrix;
    QMatrix4x4 projection;

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    QOpenGLBuffer pixelBuf;
    GLuint texture;
    GLuint texture2;
};

#endif // RGBRENDERER_H

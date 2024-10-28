#include "rgbrenderer.h"

#include <QOffscreenSurface>
#include <QGLFramebufferObject>
#include <QImage>
#include <QScreen>
#include <QMatrix4x4>
struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};
#define GET_STR(x) #x

#define A_VER 3

#define T_VER 4

//顶点shader
const char *vertString = GET_STR(
    attribute vec4 a_position;
attribute vec2 a_texcoord;
varying vec2 tex_coord;
uniform mat4 mvpMatrix;
void main(void)
{
    gl_Position = mvpMatrix * a_position;
    tex_coord = a_texcoord;
}
);

//片元shader

const char *fragString = GET_STR(
    varying vec2 tex_coord;
uniform sampler2D texture1;
void main(void)
{
    gl_FragColor = texture2D(texture1, tex_coord);
}
);

const char *fragString_merge = GET_STR(
    varying vec2 tex_coord;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main(void)
{
    vec4 f = texture2D(texture1, tex_coord);
    vec4 g = texture2D(texture2, tex_coord);

    if(f.r<0.5 || f.g<0.5 || f.b<0.5)
    {
        vec4 dstColor = f * (1.0 - g.a) + g * g.a;
        gl_FragColor = dstColor;
    }
    else
    {
        vec4 dstColor = f * (1.0 - g.a) + g;
        gl_FragColor = dstColor;
    }
}
);

RgbRenderer::RgbRenderer():
    arrayBuf(QOpenGLBuffer::VertexBuffer),
    indexBuf(QOpenGLBuffer::IndexBuffer),
    pixelBuf(QOpenGLBuffer::PixelPackBuffer)
{
}

RgbRenderer::~RgbRenderer()
{
    arrayBuf.destroy();
    indexBuf.destroy();
    pixelBuf.destroy();
}

void RgbRenderer::drawMergeFrame(const unsigned char* srcImage,const unsigned char* srcImage2,int srcWidth,int srcHeight)
{
    if(0==srcWidth || 0==srcHeight)
        return;
    if(!isInit)
    {
        initializeGL();
        initMatrix();
        rotateMatrix(180,1,0,0);
        initPBO(srcWidth,srcHeight,1,1);
        isInit=true;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    //修改纹理内容(复制内存内容)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, srcWidth, srcHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,srcImage);
    //与shader 关联
    glUniform1i(program.uniformLocation("texture1"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    //修改纹理内容(复制内存内容)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, srcWidth, srcHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,srcImage2);
    //与shader 关联
    glUniform1i(program.uniformLocation("texture2"), 1);

    drawGeometry();
}

void RgbRenderer::drawFrame(const unsigned char* srcImage,int srcWidth,int srcHeight)
{
    if(0==srcWidth || 0==srcHeight)
        return;
    if(!isInit)
    {
        initializeGL();
        initMatrix();
        rotateMatrix(180,1,0,0);
        initPBO(srcWidth,srcHeight,1,1);
        isInit=true;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    //修改纹理内容(复制内存内容)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, srcWidth, srcHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,srcImage);
    //与shader 关联
    glUniform1i(program.uniformLocation("texture1"), 0);

    drawGeometry();
}

void RgbRenderer::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    initShaders();
    initGeometry();
    initTexture();
}

void RgbRenderer::initTexture()
{
    glGenTextures(1, &texture);
    //只需要一个texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    //放大过滤，线性插值   GL_NEAREST(效率高，但马赛克严重)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(2, &texture2);
    //只需要一个texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    //放大过滤，线性插值   GL_NEAREST(效率高，但马赛克严重)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void RgbRenderer::initPBO(int width,int height,int scaleX,int scaleY)
{
    pixelBuf.create();
    pixelBuf.bind();
    pixelBuf.allocate(width*scaleX*height*scaleY*4);
    pixelBuf.release();
}

bool RgbRenderer::initShaders()
{
    //顶点shader
    if(!program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertString))
    {
        qDebug()<<"init vertex shader fail!";
        return false;
    }
    //片元shader
    if(!program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragString_merge))
    //if(!program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragString))
    {
        qDebug()<<"init fragment shader fail!";
        return false;
    }

    //编译shader
    if(!program.link())
    {
        qDebug()<<"link shader fail!";
        return false;
    }

    if(!program.bind())
    {
        qDebug()<<"bin shader fail!";
        return false;
    }
    return true;
}

void RgbRenderer::initGeometry()
{
    arrayBuf.create();
    indexBuf.create();
    arrayBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    VertexData vertices[] = {
        {QVector3D(-1.0f, -1.0f,  0.0f), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 1.0f, -1.0f,  0.0f), QVector2D(1.0f, 0.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  0.0f), QVector2D(0.0f, 1.0f)},  // v2
        {QVector3D( 1.0f,  1.0f,  0.0f), QVector2D(1.0f,1.0f)} // v3
    };

    GLushort indices[] = {
         0,  1,  2,  3
    };

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, 4 * sizeof(VertexData));

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, 4 * sizeof(GLushort));
}

void RgbRenderer::drawGeometry()
{
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program.attributeLocation("a_position");
    program.enableAttributeArray(vertexLocation);
    program.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program.attributeLocation("a_texcoord");
    program.enableAttributeArray(texcoordLocation);
    program.setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Draw geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
    arrayBuf.release();
    indexBuf.release();
}

void RgbRenderer::initMatrix()
{
    projection.setToIdentity();
    mvp_matrix = projection;
    program.setUniformValue("mvpMatrix", mvp_matrix);
}

void RgbRenderer::rotateMatrix(float angle, float x, float y, float z )
{
    QMatrix4x4 matrix;
    matrix.rotate(angle,x,y,z);
    mvp_matrix=mvp_matrix*matrix;
    program.setUniformValue("mvpMatrix", mvp_matrix);
}

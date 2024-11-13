#include "httpserver.h"
#include <QTcpSocket>
#include <QDebug>
#include <QThreadPool>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QImage>
HttpServer::HttpServer(QObject *parent)
    : QTcpServer{parent}
{
    QThreadPool::globalInstance()->setMaxThreadCount(10);
    if (listen(QHostAddress::Any, 8081)) {
        qDebug() << "Server started on port 8081";
    }
    else {
        qDebug() << "Failed to start server";
    }
}

void HttpServer::incomingConnection(qintptr socketDescriptor)
{
    static int rnum = 0;
    qDebug() << "request number:" << ++rnum;
    HttpTask* task = new HttpTask(socketDescriptor);
    QThreadPool::globalInstance()->start(task);
}

void HttpTask::run() {
    QTcpSocket socket;
    if (!socket.setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Failed to set socket descriptor";
        return;
    }

    if (!socket.waitForReadyRead(3000)) {
        qDebug() << "Timeout waiting for data";
        return;
    }

    QByteArray request = socket.readAll();
    qDebug() << "Request received:\n" << request;

    // 检查请求路径是否为 `/process`
    if (request.startsWith("POST /process")) {
        handleProcessRequest(&socket, request);
    }
    else {
        // 返回 404 错误
        QByteArray response = generateHttpResponse("404 Not Found");
        socket.write(response);
        socket.flush();
    }

    socket.disconnectFromHost();
}

void HttpTask::handleProcessRequest(QTcpSocket* socket, const QByteArray& request) {
    qDebug() << "Processing image request...";

    // 查找 HTTP 请求正文的起始位置
    int jsonStart = request.indexOf("\r\n\r\n") + 4;
    if (jsonStart <= 4) {
        qDebug() << "Failed to find JSON data in request";
        QByteArray response = generateHttpResponse("Invalid HTTP request format");
        socket->write(response);
        socket->flush();
        return;
    }

    // 提取 JSON 数据部分
    QByteArray jsonData = request.mid(jsonStart);
    qDebug() << "Extracted JSON data:" << jsonData;

    // 解析 JSON 数据
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QString filePath = doc.object().value("filePath").toString();

    if (filePath.isEmpty()) {
        qDebug() << "Invalid file path";
        QByteArray response = generateHttpResponse("Invalid file path");
        socket->write(response);
        socket->flush();
        return;
    }

    qDebug() << "Received file path:" << filePath;

    // 继续处理文件路径
    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QImage image;
        if (image.loadFromData(file.readAll())) {
            // 示例：将图片缩小为 100x100 像素
            //image = image.scaled(100, 100, Qt::KeepAspectRatio);
            QFileInfo fileInfo(filePath);
            QString fileName = fileInfo.fileName();  // 获取文件名
            // 保存处理后的图片
            QString outputFilePath = "../image-uploader/output/"+ fileName;
            if (image.save(outputFilePath)) {
                qDebug() << "Image processed and saved to:" << outputFilePath;

                // 返回处理结果（文件路径）
                QByteArray response = generateHttpResponse(fileName);
                socket->write(response);
                socket->flush();
            }
            else {
                QByteArray response = generateHttpResponse("Failed to save processed image");
                socket->write(response);
                socket->flush();
            }
        }
        else {
            QByteArray response = generateHttpResponse("Failed to load image");
            socket->write(response);
            socket->flush();
        }
    }
    else {
        QByteArray response = generateHttpResponse("File not found or failed to open");
        socket->write(response);
        socket->flush();
    }
}

QByteArray HttpTask::generateHttpResponse(const QString& outputName) {
    QByteArray response = "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"  // 更改为 JSON 格式
        "Connection: close\r\n\r\n";

    // 构建包含处理后的文件名的 JSON 数据
    QByteArray jsonResponse = "{ \"processedFileName\": \"" + outputName.toUtf8() + "\" }";
    response += jsonResponse;

    return response;
}
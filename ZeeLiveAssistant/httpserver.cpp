#include "httpserver.h"
#include <QTcpSocket>
#include <QDebug>
#include <QThreadPool>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
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

    // �������·���Ƿ�Ϊ `/process`
    if (request.startsWith("POST /process")) {
        handleProcessRequest(&socket, request);
    }
    else {
        // ���� 404 ����
        QByteArray response = generateHttpResponse("404 Not Found");
        socket.write(response);
        socket.flush();
    }

    socket.disconnectFromHost();
}

void HttpTask::handleProcessRequest(QTcpSocket* socket, const QByteArray& request) {
    qDebug() << "Processing image request...";

    // ���� HTTP �������ĵ���ʼλ��
    int jsonStart = request.indexOf("\r\n\r\n") + 4;
    if (jsonStart <= 4) {
        qDebug() << "Failed to find JSON data in request";
        QByteArray response = generateHttpResponse("Invalid HTTP request format");
        socket->write(response);
        socket->flush();
        return;
    }

    // ��ȡ JSON ���ݲ���
    QByteArray jsonData = request.mid(jsonStart);
    qDebug() << "Extracted JSON data:" << jsonData;

    // ���� JSON ����
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

    // ���������ļ�·��
    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        qDebug() << "File size:" << fileData.size();

        QByteArray response = generateHttpResponse("File processed successfully");
        socket->write(response);
        socket->flush();
    }
    else {
        qDebug() << "File not found or failed to open";
        QByteArray response = generateHttpResponse("File not found or failed to open");
        socket->write(response);
        socket->flush();
    }
}

QByteArray HttpTask::generateHttpResponse(const QString& message) {
    QByteArray response = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n\r\n";
    response += message.toUtf8();
    return response;
}
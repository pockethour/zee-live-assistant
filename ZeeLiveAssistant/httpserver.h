#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QRunnable>
#include <QTcpSocket>

class HttpTask : public QRunnable
{
public:
    HttpTask(qintptr socketDescriptor) : socketDescriptor(socketDescriptor) {}
    ~HttpTask() {
        qDebug() << "HttpTask destroyed";
    }

    void run() override;

private:
    qintptr socketDescriptor;
    void handleProcessRequest(QTcpSocket* socket, const QByteArray& request);
    QByteArray generateHttpResponse(const QString& message);
};

class HttpServer : public QTcpServer
{
public:
    explicit HttpServer(QObject *parent = nullptr);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif // HTTPSERVER_H

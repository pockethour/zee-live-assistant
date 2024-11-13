#include <QCoreApplication>
#include "httpserver.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    HttpServer server;
    return app.exec();
}
//#include "player.h"
//#include <QApplication>
//
//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    Player w;
//    w.show();
//    return a.exec();
//}

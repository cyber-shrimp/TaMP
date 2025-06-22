#include <QApplication>
#include "gui/mainwindow.h"
#include "../client/client.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Client* client = Client::getInstance();
    MainWindow w(client);
    w.resize(400, 300);
    w.show();

    // Асинхронное подключение после показа окна
    client->connectToServer("127.0.0.1", 33333);

    int result = a.exec();
    Client::destroyInstance();
    return result;
}
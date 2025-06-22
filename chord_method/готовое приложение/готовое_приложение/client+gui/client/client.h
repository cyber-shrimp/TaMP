#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

class Client : public QObject
{
    Q_OBJECT
private:
//  Приватный конструктор - запрещает создание объекта извне
    explicit Client(QObject *parent = nullptr);
//  Запрет копирования и присваивания
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    static Client* p_instance;
    QTcpSocket *socket;

public:
    static Client* getInstance(QObject *parent = nullptr) {
        if (!p_instance) {
            p_instance = new Client(parent);
        }
        return p_instance;
    }

    static void destroyInstance() {
        if (p_instance) {
            delete p_instance;
            p_instance = nullptr;
        }
    }

    void connectToServer(const QString &ip, quint16 port);
    void sendRequest(const QJsonObject &request);
    bool isConnected() const;

signals:
    void connected();
    void responseReceived(const QJsonObject &response);
    void connectionError(const QString &error);

private slots:
    void onReadyRead();
    void onSocketConnected();
    void onSocketError(QAbstractSocket::SocketError error);
};

#endif // CLIENT_H
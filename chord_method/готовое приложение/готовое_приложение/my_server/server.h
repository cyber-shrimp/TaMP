#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QHash>
#include <QJsonObject>
#include <QCryptographicHash>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    bool startServer(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);
    void stopServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QSqlDatabase m_db;
    QHash<QTcpSocket*, QByteArray> m_buffers;

    void handleRequest(QTcpSocket *clientSocket, const QByteArray &jsonData);
    QByteArray processAuth(const QJsonObject &request);
    QByteArray processRegister(const QJsonObject &request);
    QByteArray processAnswer(const QJsonObject &request);
    QByteArray getStats(const QJsonObject &request);
    bool initDatabase();
    QString hashPassword(const QString &password);
    QByteArray errorResponse(const QString &message);
};

#endif // SERVER_H
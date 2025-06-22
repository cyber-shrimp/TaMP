#include "client.h"
#include <QDebug>
#include <QJsonParseError>
#include <QMessageBox>

//  Реализация в формате Singleton
Client* Client::p_instance = nullptr;

//  Реализация приватного конструктора
Client::Client(QObject *parent) : QObject(parent)
{
    // Создание сокета и подключение сигналов к слотам
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &Client::onSocketConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &Client::onSocketError);
}

// Метод для подключения к серверу по указанному IP и порту
void Client::connectToServer(const QString &ip, quint16 port)
{
    // Если сокет уже подключен, разрываем соединение перед новым подключением
    if (socket->state() != QAbstractSocket::UnconnectedState) {
        socket->abort();
    }
    socket->connectToHost(ip, port);
}

// Слот, вызываемый при успешном подключении к серверу
void Client::onSocketConnected()
{
    emit connected();
}

// Слот для обработки ошибок сокета
void Client::onSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    emit connectionError(socket->errorString());
}

// Метод для отправки JSON-запроса на сервер
void Client::sendRequest(const QJsonObject &request)
{
    // Проверка состояния подключения
    if (socket->state() != QAbstractSocket::ConnectedState) {
        emit connectionError("Нет подключения к серверу");
        return;
    }

    // Преобразование JSON в байтовый массив и отправка
    QJsonDocument doc(request);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + '\n';
    
    if (socket->write(data) == -1) {
        emit connectionError("Ошибка отправки данных");
    }
}

// Метод проверки подключения к серверу
bool Client::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

// Слот для обработки входящих данных от сервера
void Client::onReadyRead()
{
    // Чтение всех доступных строк
    while (socket->canReadLine()) {
        QByteArray data = socket->readLine().trimmed();
        
        if (data.isEmpty()) continue;
        
        // Парсинг JSON данных
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        
        if (error.error != QJsonParseError::NoError) {
            emit connectionError("Ошибка формата ответа сервера");
            continue;
        }
        
        // Если получен JSON-объект, испускаем сигнал
        if (doc.isObject()) {
            emit responseReceived(doc.object());
        }
    }
}
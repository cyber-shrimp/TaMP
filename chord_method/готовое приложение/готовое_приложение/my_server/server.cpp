#include "server.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QDebug>
#include <QRegularExpression>
#include <QDir>
#include <QCryptographicHash>
#include <QDateTime>
#include <QSqlError>

Server::Server(QObject *parent) : QTcpServer(parent)
{
    if (!initDatabase()) {
        qCritical() << "Database initialization error!";
    }
}

bool Server::startServer(const QHostAddress &address, quint16 port)
{
    if (!listen(address, port)) {
        qCritical() << "Failed to start server:" << errorString();
        return false;
    }
    qInfo() << "Server is running on" << address.toString() << "port" << port;
    return true;
}

void Server::stopServer()
{
    close();
    m_db.close();
    qInfo() << "Server has stopped";
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
    
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        qWarning() << "Socket error:" << socket->errorString();
        delete socket;
        return;
    }
    
    m_buffers.insert(socket, QByteArray());
    qInfo() << "New connection from:" << socket->peerAddress().toString();
}

void Server::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_buffers[socket].append(socket->readAll());

    while (m_buffers[socket].contains('\n')) {
        QByteArray data = m_buffers[socket].left(m_buffers[socket].indexOf('\n')).trimmed();
        m_buffers[socket].remove(0, m_buffers[socket].indexOf('\n') + 1);
        
        if (!data.isEmpty()) {
            handleRequest(socket, data);
        }
    }
}

void Server::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_buffers.remove(socket);
        socket->deleteLater();
        qInfo() << "Client disconnected:" << socket->peerAddress().toString();
    }
}

bool Server::initDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QDir::currentPath() + "/task_manager.db";
    qDebug() << "Database path:" << dbPath;
    m_db.setDatabaseName(dbPath);
    
    if (!m_db.open()) {
        qCritical() << "Error opening database:" << m_db.lastError().text();
        return false;
    }
    
    QSqlQuery query;
    
    // Users table
    QString createUsersTable = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "login TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL, "
        "email TEXT UNIQUE NOT NULL, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)";
    
    if (!query.exec(createUsersTable)) {
        qCritical() << "Error creating users table:" << query.lastError().text();
        return false;
    }
    
    // Statistics table
    QString createStatsTable = 
        "CREATE TABLE IF NOT EXISTS user_stats ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL, "
        "question_id INTEGER NOT NULL, "
        "is_correct BOOLEAN NOT NULL, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY(user_id) REFERENCES users(id), "
        "UNIQUE(user_id, question_id))";

    if (!query.exec(createStatsTable)) {
        qCritical() << "Error creating stats table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QString Server::hashPassword(const QString &password)
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

QByteArray Server::errorResponse(const QString &message)
{
    QJsonObject response;
    response["type"] = "error";
    response["message"] = message;
    return QJsonDocument(response).toJson(QJsonDocument::Compact);
}

void Server::handleRequest(QTcpSocket *clientSocket, const QByteArray &jsonData)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    
    if (error.error != QJsonParseError::NoError) {
        clientSocket->write(errorResponse("Invalid JSON format"));
        return;
    }
    
    QJsonObject request = doc.object();
    QString type = request["type"].toString();
    
    QByteArray response;
    if (type == "register") {
        response = processRegister(request);
    } else if (type == "auth") {
        response = processAuth(request);
    } else if (type == "submit_answer") {
        response = processAnswer(request);
    } else if (type == "get_stats") {
        response = getStats(request);
    } else {
        clientSocket->write(errorResponse("Unknown request type"));
        return;
    }
    
    clientSocket->write(response + '\n');
}

QByteArray Server::processRegister(const QJsonObject &request)
{
    QStringList requiredFields = {"login", "password", "email"};
    for (const QString &field : requiredFields) {
        if (!request.contains(field) || request[field].toString().isEmpty()) {
            return errorResponse("Field is required: " + field);
        }
    }

    QString login = request["login"].toString().trimmed();
    QString password = request["password"].toString();
    QString email = request["email"].toString().trimmed().toLower();

    // Validation
    if (login.length() < 4) {
        return errorResponse("Login must be at least 4 characters");
    }

    if (password.length() < 6) {
        return errorResponse("Password must be at least 6 characters");
    }

    if (!QRegularExpression("^\\S+@\\S+\\.\\S+$").match(email).hasMatch()) {
        return errorResponse("Invalid email format");
    }

    // Check existing user
    QSqlQuery checkQuery(m_db);
    checkQuery.prepare("SELECT id FROM users WHERE login = :login OR email = :email");
    checkQuery.bindValue(":login", login);
    checkQuery.bindValue(":email", email);
    
    if (!checkQuery.exec()) {
        return errorResponse("Database error: " + checkQuery.lastError().text());
    }
    
    if (checkQuery.next()) {
        return errorResponse("User with this login or email already exists");
    }

    // Hash password
    QString hashedPassword = hashPassword(password);

    // Add user
    QSqlQuery insertQuery(m_db);
    insertQuery.prepare(
        "INSERT INTO users (login, password, email) "
        "VALUES (:login, :password, :email)"
    );
    insertQuery.bindValue(":login", login);
    insertQuery.bindValue(":password", hashedPassword);
    insertQuery.bindValue(":email", email);
    
    QJsonObject response;
    response["type"] = "register_response";
    
    if (insertQuery.exec()) {
        response["success"] = true;
        response["message"] = "Registration successful";
        response["user_id"] = insertQuery.lastInsertId().toInt();
    } else {
        response["success"] = false;
        response["message"] = "Registration failed: " + insertQuery.lastError().text();
    }
    
    return QJsonDocument(response).toJson(QJsonDocument::Compact);
}

QByteArray Server::processAuth(const QJsonObject &request)
{
    if (!request.contains("login") || !request.contains("password")) {
        return errorResponse("Login and password are required");
    }

    QString login = request["login"].toString();
    QString hashedPassword = hashPassword(request["password"].toString());

    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM users WHERE login = :login AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", hashedPassword);
    
    QJsonObject response;
    response["type"] = "auth_response";
    
    if (!query.exec()) {
        response["success"] = false;
        response["message"] = "Database error: " + query.lastError().text();
    } else if (query.next()) {
        response["success"] = true;
        response["message"] = "Authentication successful";
        response["user_id"] = query.value("id").toInt();
    } else {
        response["success"] = false;
        response["message"] = "Invalid login or password";
    }
    
    return QJsonDocument(response).toJson(QJsonDocument::Compact);
}

QByteArray Server::processAnswer(const QJsonObject &request)
{
    if (!request.contains("user_id") || !request.contains("question_id") || !request.contains("answer")) {
        return errorResponse("Missing required fields");
    }

    int userId = request["user_id"].toInt();
    int questionId = request["question_id"].toInt();
    QString answer = request["answer"].toString();

    bool isCorrect = false;
    switch (questionId) {
        case 1: isCorrect = (answer == "4"); break;
        case 2: isCorrect = (answer == "15"); break;
        case 3: isCorrect = (answer == "5"); break;
        case 4: isCorrect = (answer == "3"); break;
        default: return errorResponse("Invalid question ID");
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT OR REPLACE INTO user_stats (user_id, question_id, is_correct) "
        "VALUES (:user_id, :question_id, :is_correct)"
    );
    query.bindValue(":user_id", userId);
    query.bindValue(":question_id", questionId);
    query.bindValue(":is_correct", isCorrect);

    QJsonObject response;
    response["type"] = "answer_response";
    
    if (query.exec()) {
        response["success"] = true;
        response["is_correct"] = isCorrect;
    } else {
        response["success"] = false;
        response["message"] = "Database error: " + query.lastError().text();
    }
    
    return QJsonDocument(response).toJson(QJsonDocument::Compact);
}

QByteArray Server::getStats(const QJsonObject &request)
{
    if (!request.contains("user_id")) {
        return errorResponse("User ID is required");
    }

    int userId = request["user_id"].toInt();
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT question_id, is_correct FROM user_stats "
        "WHERE user_id = :user_id ORDER BY question_id"
    );
    query.bindValue(":user_id", userId);

    QJsonObject response;
    response["type"] = "stats_response";
    QJsonArray statsArray;

    if (query.exec()) {
        while (query.next()) {
            QJsonObject stat;
            stat["question_id"] = query.value("question_id").toInt();
            stat["is_correct"] = query.value("is_correct").toBool();
            statsArray.append(stat);
        }
        response["success"] = true;
        response["stats"] = statsArray;
    } else {
        response["success"] = false;
        response["message"] = "Database error: " + query.lastError().text();
    }

    return QJsonDocument(response).toJson(QJsonDocument::Compact);
}
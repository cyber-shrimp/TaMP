#include "loginwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QDebug>

LoginWindow::LoginWindow(Client *client, QWidget *parent) 
    : QWidget(parent), client(client), userId(-1)
{
    this->setStyleSheet("QWidget#LoginWindow { background-color: #aa00ff; }");
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);

    loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Введите логин");
    loginEdit->setMinimumHeight(40);
    layout->addWidget(loginEdit);

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Введите пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(40);
    layout->addWidget(passwordEdit);

    QPushButton *loginButton = new QPushButton("Войти", this);
    loginButton->setMinimumSize(100, 45);
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    layout->addWidget(loginButton);

    QPushButton *registerButton = new QPushButton("Регистрация", this);
    registerButton->setMinimumSize(100, 45);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    layout->addWidget(registerButton);

    connect(client, &Client::responseReceived, this, &LoginWindow::handleServerResponse);
    connect(client, &Client::connectionError, this, [this](const QString &error) {
        QMessageBox::critical(this, "Ошибка соединения", error);
    });

    this->setObjectName("LoginWindow");
}

int LoginWindow::getUserId() const {
    return userId;
}

void LoginWindow::onLoginClicked() {
    QString login = loginEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены");
        return;
    }

    if (login.length() < 4 || password.length() < 6) {
        QMessageBox::warning(this, "Ошибка", "Логин должен быть не менее 4 символов\nПароль - не менее 6 символов");
        return;
    }

    QJsonObject request;
    request["type"] = "auth";
    request["login"] = login;
    request["password"] = password;

    client->sendRequest(request);
}

void LoginWindow::onRegisterClicked() {
    emit openRegister();
}

void LoginWindow::handleServerResponse(const QJsonObject &response) {
    if (!response.contains("type")) {
        QMessageBox::critical(this, "Ошибка", "Некорректный ответ сервера");
        return;
    }

    QString type = response["type"].toString();
    
    if (type == "auth_response") {
        if (!response.contains("success")) {
            QMessageBox::critical(this, "Ошибка", "Некорректный формат ответа");
            return;
        }

        bool success = response["success"].toBool();
        QString message = response.contains("message") 
                        ? response["message"].toString() 
                        : "Неизвестная ошибка сервера";
        
        if (success) {
            userId = response["user_id"].toInt();
            QMessageBox::information(this, "Успех", "Авторизация прошла успешно");
            emit loginSuccess();
        } else {
            QMessageBox::warning(this, "Ошибка авторизации", message);
        }
    }
    else if (type == "error") {
        QString errorMsg = response.contains("message") 
                         ? response["message"].toString() 
                         : "Неизвестная ошибка";
        QMessageBox::critical(this, "Ошибка сервера", errorMsg);
    }
    //else {
    //    QMessageBox::warning(this, "Ошибка", "Получен ответ неизвестного типа: " + type);
    //}
}
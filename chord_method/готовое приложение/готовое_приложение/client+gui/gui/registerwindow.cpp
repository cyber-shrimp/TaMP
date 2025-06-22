#include "registerwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRegularExpression>

RegisterWindow::RegisterWindow(Client *client, QWidget *parent) 
    : QWidget(parent), client(client)
{
    // Установка фиолетового фона для окна регистрации
    this->setStyleSheet("RegisterWindow { background-color: #aa00ff; }");

    // Создаем основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Создаем поля ввода
    loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Введите логин (мин. 4 символа)");
    
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Введите пароль (мин. 6 символов)");
    passwordEdit->setEchoMode(QLineEdit::Password);
    
    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("Введите email");

    // Создаем кнопки
    QPushButton *registerButton = new QPushButton("Зарегистрироваться", this);
    QPushButton *backButton = new QPushButton("Назад", this);

    // Добавляем элементы на layout
    mainLayout->addWidget(loginEdit);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(emailEdit);
    mainLayout->addWidget(registerButton);
    mainLayout->addWidget(backButton);

    // Устанавливаем layout для окна
    setLayout(mainLayout);

    // Подключаем сигналы и слоты
    connect(registerButton, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    connect(backButton, &QPushButton::clicked, this, &RegisterWindow::backToLogin);
    connect(client, &Client::responseReceived, this, &RegisterWindow::handleServerResponse);

    // Настройки окна
    setWindowTitle("Регистрация");
    resize(300, 200);
}

void RegisterWindow::onRegisterClicked()
{
    QString login = loginEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString email = emailEdit->text().trimmed();

    // Валидация данных
    if (login.isEmpty() || password.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены");
        return;
    }

    if (login.length() < 4) {
        QMessageBox::warning(this, "Ошибка", "Логин должен быть не менее 4 символов");
        return;
    }

    if (password.length() < 6) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен быть не менее 6 символов");
        return;
    }

    if (!QRegularExpression("^\\S+@\\S+\\.\\S+$").match(email).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Некорректный email");
        return;
    }

    QJsonObject request;
    request["type"] = "register";
    request["login"] = login;
    request["password"] = password;
    request["email"] = email;

    client->sendRequest(request);
}

void RegisterWindow::handleServerResponse(const QJsonObject &response)
{
    if (response["type"].toString() == "register_response") {
        if (response["success"].toBool()) {
            QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");
            emit registrationSuccess();
        } else {
            QMessageBox::warning(this, "Ошибка", response["message"].toString());
        }
    }
    // Просто игнорируем другие типы ответов
}
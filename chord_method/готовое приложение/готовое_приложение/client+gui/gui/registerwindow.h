#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include "../client/client.h"

class RegisterWindow : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterWindow(Client *client, QWidget *parent = nullptr);

signals:
    void registrationSuccess();
    void backToLogin();

private slots:
    void onRegisterClicked();
    void handleServerResponse(const QJsonObject &response);

private:
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *emailEdit;
    Client *client;
};

#endif // REGISTERWINDOW_H
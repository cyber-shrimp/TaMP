#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include "../client/client.h"

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    explicit LoginWindow(Client *client, QWidget *parent = nullptr);
    int getUserId() const;

signals:
    void loginSuccess();
    void openRegister();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void handleServerResponse(const QJsonObject &response);

private:
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    Client *client;
    int userId = -1;
};

#endif // LOGINWINDOW_H
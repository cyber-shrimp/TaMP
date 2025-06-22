#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStackedWidget>
#include <QMessageBox>
#include "loginwindow.h"
#include "registerwindow.h"
#include "taskwindow.h"
#include "questionwindow.h"
#include "client.h"

class MainWindow : public QStackedWidget {
    Q_OBJECT
public:
    explicit MainWindow(Client *client, QWidget *parent = nullptr);

private slots:
    void onLoginSuccess();
    void onOpenRegister();
    void onBackToLogin();
    void onShowQuestion(int id);
    void onExit();
    void onClientConnected();
    void onClientError(const QString &error);
    void onResponseReceived(const QJsonObject &response);

private:
    void updateStats();
    
    Client *client;
    LoginWindow *loginWindow;
    RegisterWindow *registerWindow;
    TaskWindow *taskWindow;
    QuestionWindow *questionWindow;
    int currentUserId;
};

#endif // MAINWINDOW_H
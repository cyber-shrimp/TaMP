#include "mainwindow.h"
#include <QJsonArray>

MainWindow::MainWindow(Client *client, QWidget *parent) 
    : QStackedWidget(parent), client(client), currentUserId(-1)
{
    this->setStyleSheet("MainWindow { background-color: #aa00ff; }");
    
    loginWindow = new LoginWindow(client);
    registerWindow = new RegisterWindow(client);
    taskWindow = new TaskWindow(client);
    questionWindow = nullptr;

    addWidget(loginWindow);
    addWidget(registerWindow);
    addWidget(taskWindow);

    connect(loginWindow, &LoginWindow::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(loginWindow, &LoginWindow::openRegister, this, &MainWindow::onOpenRegister);
    connect(registerWindow, &RegisterWindow::backToLogin, this, &MainWindow::onBackToLogin);
    connect(taskWindow, &TaskWindow::showQuestion, this, &MainWindow::onShowQuestion);
    connect(taskWindow, &TaskWindow::requestStats, this, &MainWindow::updateStats);  // Исправленное соединение
    connect(taskWindow, &TaskWindow::exit, this, &MainWindow::onExit);
    connect(client, &Client::connected, this, &MainWindow::onClientConnected);
    connect(client, &Client::connectionError, this, &MainWindow::onClientError);
    connect(client, &Client::responseReceived, this, &MainWindow::onResponseReceived);
}

void MainWindow::onClientConnected() {
    qDebug() << "Successfully connected to server";
}

void MainWindow::onClientError(const QString &error) {
    QMessageBox::warning(this, "Connection Error", error);
}

void MainWindow::onLoginSuccess() {
    currentUserId = loginWindow->getUserId();
    updateStats();
    setCurrentWidget(taskWindow);
}

void MainWindow::onOpenRegister() {
    setCurrentWidget(registerWindow);
}

void MainWindow::onBackToLogin() {
    setCurrentWidget(loginWindow);
}

void MainWindow::onShowQuestion(int id) {
    if (questionWindow) {
        removeWidget(questionWindow);
        delete questionWindow;
    }
    questionWindow = new QuestionWindow(client, currentUserId, id);
    addWidget(questionWindow);
    setCurrentWidget(questionWindow);
    
    connect(questionWindow, &QuestionWindow::backToTasks, this, [this]() {
        setCurrentWidget(taskWindow);
        updateStats();
    });
}

void MainWindow::onExit() {
    close();
}

void MainWindow::onResponseReceived(const QJsonObject &response) {
    if (response["type"] == "stats_response") {
        if (response["success"].toBool()) {
            QJsonArray stats = response["stats"].toArray();
            QString statsText = "<h3>Ваша статистика:</h3><ul>";
            
            QMap<int, QString> questions;
            questions[1] = "2 + 2 = ?";
            questions[2] = "3 * 5 = ?";
            questions[3] = "10 / 2 = ?";
            questions[4] = "7 - 4 = ?";
            
            QSet<int> answeredQuestions;
            for (const QJsonValue &stat : stats) {
                QJsonObject s = stat.toObject();
                int qId = s["question_id"].toInt();
                answeredQuestions.insert(qId);
                QString result = s["is_correct"].toBool() ? "✓" : "✗";
                statsText += QString("<li>%1: %2</li>").arg(questions[qId]).arg(result);
            }
            
            for (auto it = questions.begin(); it != questions.end(); ++it) {
                if (!answeredQuestions.contains(it.key())) {
                    statsText += QString("<li>%1: Не отвечен</li>").arg(it.value());
                }
            }
            
            statsText += "</ul>";
            taskWindow->setStats(statsText);
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить статистику");
        }
    }
}

void MainWindow::updateStats() {
    if (currentUserId == -1) return;
    
    QJsonObject request;
    request["type"] = "get_stats";
    request["user_id"] = currentUserId;
    client->sendRequest(request);
}
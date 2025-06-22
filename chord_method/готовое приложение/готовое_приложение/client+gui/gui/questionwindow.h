#ifndef QUESTIONWINDOW_H
#define QUESTIONWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include "client.h"

class QuestionWindow : public QWidget {
    Q_OBJECT
public:
    explicit QuestionWindow(Client *client, int userId, int questionId, QWidget *parent = nullptr);

signals:
    void backToTasks();

private slots:
    void onSubmitClicked();
    void onResponseReceived(const QJsonObject &response);

private:
    Client *client;
    int userId;
    int questionId;
    QLineEdit *answerEdit;
};

#endif // QUESTIONWINDOW_H
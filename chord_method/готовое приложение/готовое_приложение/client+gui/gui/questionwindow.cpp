#include "questionwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

QuestionWindow::QuestionWindow(Client *client, int userId, int questionId, QWidget *parent)
    : QWidget(parent), client(client), userId(userId), questionId(questionId) {
    this->setStyleSheet("QuestionWindow { background-color: #aa00ff; }");
    
    QVBoxLayout *layout = new QVBoxLayout(this);

    QString questionText;
    switch (questionId) {
        case 1: questionText = "2 + 2 = ?"; break;
        case 2: questionText = "3 * 5 = ?"; break;
        case 3: questionText = "10 / 2 = ?"; break;
        case 4: questionText = "7 - 4 = ?"; break;
    }

    QLabel *questionLabel = new QLabel(questionText, this);
    layout->addWidget(questionLabel);

    answerEdit = new QLineEdit(this);
    answerEdit->setPlaceholderText("Ответ");
    layout->addWidget(answerEdit);

    QPushButton *submitButton = new QPushButton("OK", this);
    connect(submitButton, &QPushButton::clicked, this, &QuestionWindow::onSubmitClicked);
    layout->addWidget(submitButton);

    connect(client, &Client::responseReceived, this, &QuestionWindow::onResponseReceived);
}

void QuestionWindow::onSubmitClicked() {
    QJsonObject request;
    request["type"] = "submit_answer";
    request["user_id"] = userId;
    request["question_id"] = questionId;
    request["answer"] = answerEdit->text();

    client->sendRequest(request);
}

void QuestionWindow::onResponseReceived(const QJsonObject &response) {
    if (response["type"] == "answer_response") {
        bool isCorrect = response["is_correct"].toBool();
        QMessageBox::information(this, "Результат", 
            isCorrect ? "Правильный ответ!" : "Неправильный ответ!");
        emit backToTasks();
    }
}
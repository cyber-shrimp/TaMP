#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include "client.h"

class TaskWindow : public QWidget {
    Q_OBJECT
public:
    explicit TaskWindow(Client *client, QWidget *parent = nullptr);
    void setStats(const QString &statsText);

signals:
    void showQuestion(int questionId);
    void requestStats();  // Исправленный сигнал
    void exit();

private slots:
    void onQuestionClicked(int id);
    void onStatsClicked();
    void onExitClicked();

private:
    Client *client;
    QLabel *statsLabel;
    QScrollArea *statsScrollArea;
    QWidget *statsContainer;
};

#endif // TASKWINDOW_H
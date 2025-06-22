#include "taskwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>

TaskWindow::TaskWindow(Client *client, QWidget *parent) 
    : QWidget(parent), client(client)
{
    this->setStyleSheet(
        "TaskWindow { background-color: #aa00ff; }"
        "QPushButton {"
        "   background-color: #6a0dad;"
        "   color: white;"
        "   border: none;"
        "   padding: 10px;"
        "   font-size: 16px;"
        "   border-radius: 5px;"
        "   min-width: 200px;"
        "}"
        "QPushButton:hover { background-color: #7b1fa2; }"
        "QLabel { color: white; font-size: 14px; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    statsScrollArea = new QScrollArea(this);
    statsScrollArea->setWidgetResizable(true);
    statsScrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    statsScrollArea->setVisible(false);

    statsContainer = new QWidget();
    statsContainer->setStyleSheet("background: transparent;");
    QVBoxLayout *statsLayout = new QVBoxLayout(statsContainer);
    
    statsLabel = new QLabel("Статистика загружается...", statsContainer);
    statsLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    statsLabel->setWordWrap(true);
    statsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    statsLayout->addWidget(statsLabel);
    
    statsScrollArea->setWidget(statsContainer);

    QVBoxLayout *buttonsLayout = new QVBoxLayout();
    buttonsLayout->setSpacing(10);

    for (int i = 1; i <= 4; ++i) {
        QPushButton *button = new QPushButton("Вопрос " + QString::number(i), this);
        connect(button, &QPushButton::clicked, [this, i]() { onQuestionClicked(i); });
        buttonsLayout->addWidget(button);
    }

    QPushButton *statsButton = new QPushButton("Статистика", this);
    connect(statsButton, &QPushButton::clicked, this, &TaskWindow::onStatsClicked);
    buttonsLayout->addWidget(statsButton);

    QPushButton *exitButton = new QPushButton("Выход", this);
    connect(exitButton, &QPushButton::clicked, this, &TaskWindow::onExitClicked);
    buttonsLayout->addWidget(exitButton);

    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(statsScrollArea);
}

void TaskWindow::setStats(const QString &statsText) {
    statsLabel->setText(statsText);
    statsLabel->setTextFormat(Qt::RichText);
    statsLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 14px;"
        "   padding: 10px;"
        "   background-color: rgba(0, 0, 0, 0.2);"
        "   border-radius: 5px;"
        "}"
    );
}

void TaskWindow::onQuestionClicked(int id) {
    statsScrollArea->setVisible(false);
    emit showQuestion(id);
}

void TaskWindow::onStatsClicked() {
    bool showStats = !statsScrollArea->isVisible();
    statsScrollArea->setVisible(showStats);
    
    if (showStats) {
        emit requestStats();  // Исправленный вызов сигнала
    }
}

void TaskWindow::onExitClicked() {
    emit exit();
}
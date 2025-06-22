QT += widgets network
QT += core gui network sql

TARGET = qt_task_app
DESTDIR = bin

# Указываем, что нужно создавать MOC-файлы в отдельной директории
MOC_DIR = moc
OBJECTS_DIR = obj

# Исходные файлы
SOURCES += \
    main.cpp \
    client/client.cpp \
    gui/mainwindow.cpp \
    gui/loginwindow.cpp \
    gui/registerwindow.cpp \
    gui/taskwindow.cpp \
    gui/questionwindow.cpp

# Заголовочные файлы
HEADERS += \
    client/client.h \
    gui/mainwindow.h \
    gui/loginwindow.h \
    gui/registerwindow.h \
    gui/taskwindow.h \
    gui/questionwindow.h

# Пути к заголовкам (абсолютные)
INCLUDEPATH += $$PWD \
               $$PWD/client \
               $$PWD/gui \
               $$[QT_INSTALL_HEADERS]

# Для Windows: явно указываем путь к Qt
win32 {
    QT_INSTALL_PREFIX = C:/Qt/6.9.0/mingw_64
    INCLUDEPATH += $$QT_INSTALL_PREFIX/include
}
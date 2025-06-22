QT += core network sql
QT += core gui network sql
TARGET = server
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

SOURCES += main.cpp \
           server.cpp

HEADERS += server.h
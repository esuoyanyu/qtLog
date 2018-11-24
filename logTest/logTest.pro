QT += core
QT -= gui

CONFIG += c++11

TARGET = logTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    logthread.cpp

HEADERS += \
    logthread.h \
    tool.h

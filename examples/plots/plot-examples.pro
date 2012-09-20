#
#  QCustomPlot Plot Examples
#

QT       += core gui
TARGET = plot-examples
TEMPLATE = app

SOURCES += main.cpp\
           mainwindow.cpp \
         ../../qcustomplot.cpp

HEADERS  += mainwindow.h \
         ../../qcustomplot.h

LIBS += ../../../../libs/timer/src/Timer.o

FORMS    += mainwindow.ui


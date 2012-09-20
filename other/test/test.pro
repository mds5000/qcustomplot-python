#
# Project to test various use cases
#


QT += core gui

TARGET = test
TEMPLATE = app

CONFIG(debug, debug|release) {
  qcplib.commands = cd ../../src; make debug
} else {
  qcplib.commands = cd ../../src; make release
}
QMAKE_EXTRA_TARGETS += qcplib
PRE_TARGETDEPS += qcplib

SOURCES += main.cpp\
mainwindow.cpp
HEADERS  += mainwindow.h \
../../src/*.h
FORMS    += mainwindow.ui
OTHER_FILES += \
../../todo.txt \
../../changenotes.txt

LIBS += -L../../src
CONFIG(debug, debug|release) {
  LIBS += -lqcustomplotd
} else {
  LIBS += -lqcustomplot
}




QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    entry.cpp \
    lexicon.cpp \
    listmodel.cpp \
    main.cpp \
    mainwindow.cpp \
    tablemodel.cpp \
    wordHistory.cpp \
    wordbreaker.cpp

HEADERS += \
    entry.h \
    lexicon.h \
    listmodel.h \
    mainwindow.h \
    tablemodel.h \
    wordHistory.h \
    wordbreaker.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

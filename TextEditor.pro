# 工程文件：告诉 qmake 用哪些模块、编译哪些源文件
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = TextEditor
TEMPLATE = app

# 源码
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    finddialog.cpp \
    replacedialog.cpp \
    codeeditor.cpp

HEADERS += \
    mainwindow.h \
    finddialog.h \
    replacedialog.h \
    codeeditor.h

# 界面文件（UI 设计器生成的 .ui）
FORMS += \
    mainwindow.ui \
    finddialog.ui \
    replacedialog.ui

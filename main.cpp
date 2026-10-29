#include "mainwindow.h"
#include <QApplication>

// 程序入口：创建 QApplication，显示主窗口
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}

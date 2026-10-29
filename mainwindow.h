#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;

namespace Ui {
class MainWindow;
}

// 主窗口：文本编辑器的主体
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 关闭窗口时触发：用于「未保存则提示」
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件
    void newFile();        // 新建
    void openFile();       // 打开
    bool saveFile();       // 保存
    bool saveFileAs();     // 另存为
    // 编辑
    void find();           // 查找
    void replace();        // 替换
    // 格式
    void setFont();        // 设置字体
    void setFontColor();   // 字体颜色
    void setBackgroundColor(); // 背景色
    // 视图
    void toggleLineNumbers(bool on); // 显示/隐藏行号
    // 帮助
    void about();

    // 状态同步
    void updateStatusBar(); // 更新状态栏（光标位置等）
    void updateActions();   // 更新剪切/复制/撤销等按钮的使能状态

private:
    bool maybeSave();               // 若有未保存修改，弹提示；返回是否继续
    void loadFile(const QString &fileName); // 从文件读入
    void saveToFile(const QString &fileName); // 写入文件
    void updateTitle();             // 根据「是否修改」更新窗口标题
    void setupConnections();        // 集中做信号槽连接
    void setupStatusBar();          // 初始化状态栏
    void setupIcons();              // 给菜单/工具栏动作设置标准图标

    Ui::MainWindow *ui;
    QString currentFile;   // 当前文件完整路径
    bool isUntitled;       // 是否为「未命名」的新文件
};

#endif // MAINWINDOW_H

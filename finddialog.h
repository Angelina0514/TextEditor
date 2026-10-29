#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

namespace Ui {
class FindDialog;
}

// 查找对话框：输入关键词，点「查找下一个」发出信号，由主窗口执行查找
class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

signals:
    // text: 查找内容  cs: 区分大小写  whole: 全词匹配
    void findNext(const QString &text, bool cs, bool whole);

private slots:
    void onFindClicked();

private:
    Ui::FindDialog *ui;
};

#endif // FINDDIALOG_H

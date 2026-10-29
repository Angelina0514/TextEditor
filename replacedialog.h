#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

namespace Ui {
class ReplaceDialog;
}

// 替换对话框：输入「查找内容」和「替换为」，可单个替换或全部替换
class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog();

signals:
    void replaceOne(const QString &findText, const QString &replaceText, bool cs);
    void replaceAll(const QString &findText, const QString &replaceText, bool cs);

private slots:
    void onReplaceClicked();
    void onReplaceAllClicked();

private:
    Ui::ReplaceDialog *ui;
};

#endif // REPLACEDIALOG_H

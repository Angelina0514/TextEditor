#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class QPaintEvent;
class QResizeEvent;

class LineNumberArea;

// 从 QPlainTextEdit 派生出带行号显示的编辑器（选做功能，参考 Qt 官方 CodeEditor 示例）
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();

public slots:
    // 显示/隐藏行号（主窗口的「视图→行号」菜单调用）
    void setLineNumberAreaVisible(bool visible);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    bool lineNumbersVisible;
};

// 行号边栏：一个小控件，紧贴在编辑器左侧绘制行号
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override { return QSize(codeEditor->lineNumberAreaWidth(), 0); }

protected:
    void paintEvent(QPaintEvent *event) override { codeEditor->lineNumberAreaPaintEvent(event); }

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H

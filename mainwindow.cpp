#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "codeeditor.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QColorDialog>
#include <QFontDialog>
#include <QLabel>
#include <QCloseEvent>
#include <QClipboard>
#include <QMimeData>
#include <QGuiApplication>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isUntitled(true)
{
    ui->setupUi(this);

    setupStatusBar();
    setupConnections();
    setupIcons();

    // 初始状态：新建一个空白文档，并清掉「修改」标记
    ui->textEdit->setPlainText("");
    ui->textEdit->document()->setModified(false);
    updateTitle();
    updateActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ---------------------------------------------------------------------------
// 集中做信号槽连接
// ---------------------------------------------------------------------------
void MainWindow::setupConnections()
{
    // 文件
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    // 编辑：直接接到 QPlainTextEdit 自带的槽
    connect(ui->actionUndo, &QAction::triggered, ui->textEdit, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->textEdit, &QPlainTextEdit::redo);
    connect(ui->actionCut, &QAction::triggered, ui->textEdit, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->textEdit, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->textEdit, &QPlainTextEdit::paste);
    connect(ui->actionSelectAll, &QAction::triggered, ui->textEdit, &QPlainTextEdit::selectAll);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::find);
    connect(ui->actionReplace, &QAction::triggered, this, &MainWindow::replace);

    // 格式
    connect(ui->actionFont, &QAction::triggered, this, &MainWindow::setFont);
    connect(ui->actionFontColor, &QAction::triggered, this, &MainWindow::setFontColor);
    connect(ui->actionBackgroundColor, &QAction::triggered, this, &MainWindow::setBackgroundColor);
    connect(ui->actionWordWrap, &QAction::toggled, this, [this](bool on) {
        ui->textEdit->setLineWrapMode(on ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    });

    // 视图
    connect(ui->actionToolbar, &QAction::toggled, this, [this](bool on) {
        ui->toolBar->setVisible(on);
    });
    connect(ui->actionStatusbar, &QAction::toggled, this, [this](bool on) {
        ui->statusbar->setVisible(on);
    });
    connect(ui->actionLineNumbers, &QAction::toggled, this, &MainWindow::toggleLineNumbers);

    // 帮助
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);

    // 状态同步
    connect(ui->textEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateStatusBar);
    connect(ui->textEdit, &QPlainTextEdit::copyAvailable, this, [this](bool yes) {
        ui->actionCut->setEnabled(yes);
        ui->actionCopy->setEnabled(yes);
    });
    connect(ui->textEdit, &QPlainTextEdit::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(ui->textEdit, &QPlainTextEdit::redoAvailable, ui->actionRedo, &QAction::setEnabled);
    connect(ui->textEdit->document(), &QTextDocument::modificationChanged, this, &MainWindow::updateTitle);

    // 剪贴板变化 → 更新粘贴按钮
    connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::updateActions);
}

// ---------------------------------------------------------------------------
// 图标：用 Qt 自带的标准图标，不用额外图片资源
// ---------------------------------------------------------------------------
void MainWindow::setupIcons()
{
    QStyle *style = this->style();
    ui->actionNew->setIcon(style->standardIcon(QStyle::SP_FileIcon));
    ui->actionOpen->setIcon(style->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionSave->setIcon(style->standardIcon(QStyle::SP_DialogSaveButton));
    ui->actionUndo->setIcon(style->standardIcon(QStyle::SP_ArrowBack));
    ui->actionRedo->setIcon(style->standardIcon(QStyle::SP_ArrowForward));
    ui->actionCut->setIcon(style->standardIcon(QStyle::SP_DialogDiscardButton));
    ui->actionCopy->setIcon(style->standardIcon(QStyle::SP_DialogApplyButton));
    ui->actionPaste->setIcon(style->standardIcon(QStyle::SP_DialogResetButton));
    ui->actionFind->setIcon(style->standardIcon(QStyle::SP_FileDialogContentsView));
    ui->actionAbout->setIcon(style->standardIcon(QStyle::SP_MessageBoxInformation));
    // 让工具栏显示「图标+文字」
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

// ---------------------------------------------------------------------------
// 状态栏
// ---------------------------------------------------------------------------
void MainWindow::setupStatusBar()
{
    // 状态栏左侧显示一个「行 x，列 y」的标签（代码里动态更新）
    QLabel *posLabel = new QLabel("就绪", this);
    posLabel->setObjectName("posLabel");
    ui->statusbar->addWidget(posLabel);
}

void MainWindow::updateStatusBar()
{
    QLabel *label = findChild<QLabel *>("posLabel");
    if (!label)
        return;
    QTextCursor cursor = ui->textEdit->textCursor();
    int line = cursor.blockNumber() + 1;          // 行号从 1 开始
    int col  = cursor.positionInBlock() + 1;      // 列号从 1 开始
    label->setText(QString("行 %1, 列 %2").arg(line).arg(col));
}

// ---------------------------------------------------------------------------
// 按钮使能状态
// ---------------------------------------------------------------------------
void MainWindow::updateActions()
{
    // 粘贴：剪贴板里有文本才能用
    const QMimeData *mime = QGuiApplication::clipboard()->mimeData();
    ui->actionPaste->setEnabled(mime && mime->hasText());
    // 剪切/复制：有选中内容才可用（copyAvailable 信号也会维护，这里兜底）
    bool hasSelection = ui->textEdit->textCursor().hasSelection();
    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);
    // 撤销/重做
    ui->actionUndo->setEnabled(ui->textEdit->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->textEdit->document()->isRedoAvailable());
}

// ---------------------------------------------------------------------------
// 文件操作
// ---------------------------------------------------------------------------
void MainWindow::newFile()
{
    if (!maybeSave())
        return;
    ui->textEdit->setPlainText("");
    ui->textEdit->document()->setModified(false);
    isUntitled = true;
    currentFile.clear();
    updateTitle();
}

void MainWindow::openFile()
{
    if (!maybeSave())
        return;
    QString fileName = QFileDialog::getOpenFileName(
        this, "打开文件", QString(), "文本文件 (*.txt *.cpp *.h *.pro);;所有文件 (*)");
    if (fileName.isEmpty())
        return;
    loadFile(fileName);
}

bool MainWindow::saveFile()
{
    if (isUntitled)
        return saveFileAs();
    saveToFile(currentFile);
    return true;
}

bool MainWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, "另存为", currentFile.isEmpty() ? "未命名.txt" : currentFile,
        "文本文件 (*.txt);;所有文件 (*)");
    if (fileName.isEmpty())
        return false;
    saveToFile(fileName);
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    // 打开失败（不存在/权限不足/被占用）要合理处理
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "打开失败",
                             QString("无法打开文件：\n%1\n\n%2").arg(fileName, file.errorString()));
        return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");                 // 按 UTF-8 读取，中文不乱码
    ui->textEdit->setPlainText(in.readAll());
    file.close();

    ui->textEdit->document()->setModified(false);
    isUntitled = false;
    currentFile = fileName;
    updateTitle();
}

void MainWindow::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "保存失败",
                             QString("无法保存文件：\n%1\n\n%2").arg(fileName, file.errorString()));
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << ui->textEdit->toPlainText();
    file.close();

    ui->textEdit->document()->setModified(false);
    isUntitled = false;
    currentFile = fileName;
    updateTitle();
}

// 若有未保存修改，弹提示；返回 true 表示可以继续（用户选了保存或丢弃）
bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified())
        return true;

    QMessageBox::StandardButton ret = QMessageBox::warning(
        this, "未保存的修改",
        "文档已被修改，是否保存？",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        return saveFile();       // 保存成功才继续；取消保存则返回 false
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;                 // Discard
}

// 根据「是否修改」更新窗口标题
void MainWindow::updateTitle()
{
    QString name = isUntitled ? "未命名" : currentFile;
    bool modified = ui->textEdit->document()->isModified();
    setWindowTitle(QString("%1%2 - 简单文本编辑器").arg(name, modified ? " *" : ""));
}

// ---------------------------------------------------------------------------
// 关闭窗口：未保存则提示
// ---------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

// ---------------------------------------------------------------------------
// 格式
// ---------------------------------------------------------------------------
void MainWindow::setFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->textEdit->document()->defaultFont(), this, "选择字体");
    if (ok) {
        ui->textEdit->setFont(font);                        // 控件字体
        ui->textEdit->document()->setDefaultFont(font);     // 文档默认字体
    }
}

void MainWindow::setFontColor()
{
    QColor color = QColorDialog::getColor(ui->textEdit->palette().text().color(), this, "字体颜色");
    if (!color.isValid())
        return;

    // QPlainTextEdit 没有 setTextColor()，用 QTextCharFormat 作用于整个文档
    QTextCursor cursor(ui->textEdit->document());
    cursor.select(QTextCursor::Document);
    QTextCharFormat fmt;
    fmt.setForeground(color);
    cursor.mergeCharFormat(fmt);
}

void MainWindow::setBackgroundColor()
{
    QColor color = QColorDialog::getColor(ui->textEdit->palette().base().color(), this, "背景颜色");
    if (color.isValid()) {
        QPalette p = ui->textEdit->palette();
        p.setColor(QPalette::Base, color);
        ui->textEdit->setPalette(p);
    }
}

void MainWindow::toggleLineNumbers(bool on)
{
    // ui->textEdit 在 .ui 里已 promote 成 CodeEditor，直接调用其接口
    ui->textEdit->setLineNumberAreaVisible(on);
}

// ---------------------------------------------------------------------------
// 查找 / 替换
// ---------------------------------------------------------------------------
void MainWindow::find()
{
    FindDialog dialog(this);
    // 把对话框的「查找下一个」信号接到实际搜索逻辑上
    connect(&dialog, &FindDialog::findNext, this, [this](const QString &text, bool cs, bool whole) {
        QTextDocument::FindFlags flags;
        if (cs) flags |= QTextDocument::FindCaseSensitively;
        if (whole) flags |= QTextDocument::FindWholeWords;

        if (!ui->textEdit->find(text, flags)) {
            // 从头再找一次（循环查找）
            QTextCursor cursor = ui->textEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            ui->textEdit->setTextCursor(cursor);
            if (!ui->textEdit->find(text, flags)) {
                ui->statusbar->showMessage("未找到：" + text, 3000);
            }
        }
    });
    dialog.exec();
}

void MainWindow::replace()
{
    ReplaceDialog dialog(this);
    connect(&dialog, &ReplaceDialog::replaceOne,
            this, [this](const QString &findText, const QString &replaceText, bool cs) {
        QTextDocument::FindFlags flags;
        if (cs) flags |= QTextDocument::FindCaseSensitively;

        QTextCursor cursor = ui->textEdit->textCursor();
        if (cursor.hasSelection() && cursor.selectedText() == findText)
            cursor.insertText(replaceText);

        if (!ui->textEdit->find(findText, flags)) {
            ui->statusbar->showMessage("未找到：" + findText, 3000);
        }
    });
    connect(&dialog, &ReplaceDialog::replaceAll,
            this, [this](const QString &findText, const QString &replaceText, bool cs) {
        QTextDocument::FindFlags flags;
        if (cs) flags |= QTextDocument::FindCaseSensitively;

        // 光标移到开头，循环替换全部
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        ui->textEdit->setTextCursor(cursor);

        int count = 0;
        while (ui->textEdit->find(findText, flags)) {
            ui->textEdit->textCursor().insertText(replaceText);
            ++count;
        }
        ui->statusbar->showMessage(QString("共替换 %1 处").arg(count), 3000);
    });
    dialog.exec();
}

// ---------------------------------------------------------------------------
// 关于
// ---------------------------------------------------------------------------
void MainWindow::about()
{
    QMessageBox::about(this, "关于",
        "<h3>简单文本编辑器</h3>"
        "<p>Qt 应用程序开发 实验二</p>"
        "<p>功能：文件读写、编辑操作、字体颜色、查找替换、行号等</p>"
        "<p>基于 Qt 5.12.11 / MinGW 7.3.0</p>");
}

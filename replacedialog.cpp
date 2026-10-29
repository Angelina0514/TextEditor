#include "replacedialog.h"
#include "ui_replacedialog.h"

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);
    connect(ui->replaceButton, &QPushButton::clicked, this, &ReplaceDialog::onReplaceClicked);
    connect(ui->replaceAllButton, &QPushButton::clicked, this, &ReplaceDialog::onReplaceAllClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

void ReplaceDialog::onReplaceClicked()
{
    QString findText = ui->findEdit->text();
    if (findText.isEmpty())
        return;
    emit replaceOne(findText, ui->replaceEdit->text(), ui->caseCheck->isChecked());
}

void ReplaceDialog::onReplaceAllClicked()
{
    QString findText = ui->findEdit->text();
    if (findText.isEmpty())
        return;
    emit replaceAll(findText, ui->replaceEdit->text(), ui->caseCheck->isChecked());
}

#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    connect(ui->findNextButton, &QPushButton::clicked, this, &FindDialog::onFindClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::onFindClicked()
{
    QString text = ui->findEdit->text();
    if (text.isEmpty())
        return;
    emit findNext(text,
                  ui->caseCheck->isChecked(),
                  ui->wholeWordCheck->isChecked());
}

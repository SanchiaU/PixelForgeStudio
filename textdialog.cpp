#include "textdialog.h"


// 文本对话框
TextDialog::TextDialog(QString text, ImageArea *parent):
    QDialog(parent)
{
    initializeGui();

    if (!text.isEmpty())
    {
        mTextEdit->setText(text);
        QTextCursor cursor(mTextEdit->textCursor());
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        mTextEdit->setTextCursor(cursor);
    }

    layout()->setSizeConstraint(QLayout::SetFixedSize);
    setWindowTitle(tr("Text"));

}

void TextDialog::initializeGui()
{
    QPushButton *mFontButton = new QPushButton(tr("Select Font..."));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(mFontButton, SIGNAL(clicked()), this, SLOT(selectFont()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));

    QHBoxLayout *hBox = new QHBoxLayout();
    hBox->addWidget(mFontButton);
    hBox->addWidget(buttonBox);

    mTextEdit = new QTextEdit();
    mTextEdit->setLineWrapMode(QTextEdit::NoWrap);  // 禁用自动换行

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(mTextEdit);
    mainLayout->addLayout(hBox);

    setLayout(mainLayout);

    connect(mTextEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));

}

void TextDialog::textChanged()
{
    // 将父窗口转换为ImageArea*类型，并发射带当前文本的信号
    emit textChanged(qobject_cast<ImageArea*>(this->parent()),mTextEdit->toPlainText());

}

void TextDialog::selectFont()
{
    bool ok;

    QFont font = DataSingleton::Instance()->getTextFont();

    // 打开字体选择对话框
    font = QFontDialog::getFont(&ok, font, this);

    if (ok)
    {
        DataSingleton::Instance()->setTextFont(font);

        textChanged();

        mTextEdit->setFocus();
    }

}

// 取消操作槽
void TextDialog::cancel()
{
    emit canceled(qobject_cast<ImageArea*>(this->parent()));

    // 调用基类reject()关闭对话框
    QDialog::reject();

}

// 重写对话框拒绝操作
void TextDialog::reject()
{
    if (mTextEdit->toPlainText().isEmpty() ||
        QMessageBox::question(this,
                              tr("Question"),
                              tr("Clear text?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) == QMessageBox::Yes)
    {
        // 通知父窗口取消操作
        emit canceled(qobject_cast<ImageArea*>(this->parent()));
    }
    QDialog::reject();
}


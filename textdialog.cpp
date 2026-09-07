#include "textdialog.h"


// 2025/08/09--代码已经审核通过



// 文本对话框构造函数
TextDialog::TextDialog(QString text, ImageArea *parent):
    QDialog(parent)     // 调用基类构造函数
{
    initializeGui();  // 初始化界面

    // 如果传入非空文本，设置到文本编辑框并移动光标到末尾
    if (!text.isEmpty())
    {
        mTextEdit->setText(text);
        QTextCursor cursor(mTextEdit->textCursor());
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        mTextEdit->setTextCursor(cursor);
    }

    // 设置布局大小策略为固定大小
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    setWindowTitle(tr("Text"));  // 设置窗口标题

}

// 初始化图形用户界面
void TextDialog::initializeGui()
{
    // 创建字体选择按钮
    QPushButton *mFontButton = new QPushButton(tr("Select Font..."));

    // 创建标准确定/取消按钮组
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // 连接按钮信号到槽函数
    connect(mFontButton, SIGNAL(clicked()), this, SLOT(selectFont()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));

    // 创建水平布局（字体按钮 + 按钮组）
    QHBoxLayout *hBox = new QHBoxLayout();
    hBox->addWidget(mFontButton);
    hBox->addWidget(buttonBox);

    // 创建文本编辑区域
    mTextEdit = new QTextEdit();
    mTextEdit->setLineWrapMode(QTextEdit::NoWrap);  // 禁用自动换行

    // 创建主垂直布局（文本编辑区 + 水平按钮布局）
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(mTextEdit);
    mainLayout->addLayout(hBox);

    setLayout(mainLayout);  // 设置对话框主布局

    // 连接文本变化信号到处理槽
    connect(mTextEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));

}

// 文本变化处理槽
void TextDialog::textChanged()
{
    // 将父窗口转换为ImageArea*类型，并发射带当前文本的信号
    emit textChanged(qobject_cast<ImageArea*>(this->parent()),mTextEdit->toPlainText());

}

// 字体选择槽
void TextDialog::selectFont()
{
    bool ok;  // 用于接收用户是否确认选择

    // 从单例获取当前字体设置
    QFont font = DataSingleton::Instance()->getTextFont();

    // 打开字体选择对话框
    font = QFontDialog::getFont(&ok, font, this);

    if (ok)  // 如果用户确认选择
    {
        // 更新全局字体设置
        DataSingleton::Instance()->setTextFont(font);

        // 触发文本变化信号更新预览
        textChanged();

        // 将焦点设置回文本编辑框
        mTextEdit->setFocus();
    }

}

// 取消操作槽
void TextDialog::cancel()
{
    // 通知父窗口取消操作
    emit canceled(qobject_cast<ImageArea*>(this->parent()));

    // 调用基类reject()关闭对话框
    QDialog::reject();

}

// 重写对话框拒绝操作
void TextDialog::reject()
{
    // 如果文本为空，或用户确认清除文本
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

    // 调用基类reject()关闭对话框
    QDialog::reject();
}


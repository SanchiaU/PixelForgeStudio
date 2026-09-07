#include "shortcutedit.h"



// 2025/07/22--代码已经审核通过



// 构造函数：初始化快捷键编辑框
ShortcutEdit::ShortcutEdit(QWidget *parent) :
    QLineEdit(parent)  // 调用基类构造函数
{
    // 设置为只读模式，防止用户直接输入文本
    setReadOnly(true);

    // 创建并配置清除按钮
    mClearButton = new QToolButton(this);
    mClearButton->setCursor(Qt::ArrowCursor);                // 鼠标指针样式
    mClearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }"); // 无边框样式
    mClearButton->hide();                                    // 初始隐藏

    // 计算并设置右侧内边距（为清除按钮预留空间）
    int fr = style()->pixelMetric(QStyle::PM_DefaultFrameWidth); // 获取边框宽度
    setStyleSheet(QString("QLineEdit { padding-right: %1px; }")
                      .arg(mClearButton->sizeHint().width() + fr + 1)); // 动态计算内边距

    // 设置控件最小尺寸（保证能容纳清除按钮）
    QSize msz = minimumSizeHint();
    setMinimumSize(
        qMax(msz.width(), mClearButton->sizeHint().height() + fr * 2 + 2),   // 宽度
        qMax(msz.height(), mClearButton->sizeHint().height() + fr * 2 + 2)   // 高度
        );

    // 连接信号与槽
    connect(mClearButton, SIGNAL(clicked()), this, SLOT(clear()));            // 点击清除按钮清空内容
    connect(this, SIGNAL(textChanged(QString)), SLOT(updateClearButton(QString))); // 文本变化更新按钮状态

}

// 键盘按下事件处理函数
void ShortcutEdit::keyPressEvent(QKeyEvent *event)
{
    // 忽略按键自动重复事件和纯修饰键事件
    if(event->isAutoRepeat() || isModifier(event->key()))
        return;

    // 组合修饰键(Shift/Ctrl/Alt等)和实际按键
    QKeySequence keySeq(event->modifiers() + event->key());

    // 设置文本显示（自动转换为"Ctrl+A"等格式）
    setText(keySeq.toString());
}

// 控件大小调整事件处理函数
// 用于重新定位清除按钮，使其始终在编辑框右侧居中显示
void ShortcutEdit::resizeEvent(QResizeEvent *)
{
    // 获取系统边框宽度
    int fr = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    // 计算清除按钮新位置：
    // X坐标：控件右边界 - 边框宽度 - 按钮宽度
    // Y坐标：垂直居中（(底部坐标+1 - 按钮高度)/2）
    mClearButton->move(
        rect().right() - fr - mClearButton->sizeHint().width(),
        (rect().bottom() + 1 - mClearButton->sizeHint().height()) / 2
        );
}

// 判断按钮是否为修饰键
bool ShortcutEdit::isModifier(int key)
{
    return (key == Qt::Key_Control ||
            key == Qt::Key_Meta ||
            key == Qt::Key_Alt ||
            key == Qt::Key_AltGr ||
            key == Qt::Key_Shift);
}

// 根据文本内容更新清除按钮状态
void ShortcutEdit::updateClearButton(const QString &text)
{
    mClearButton->setVisible(!text.isEmpty());
}

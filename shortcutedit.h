#ifndef SHORTCUTEDIT_H
#define SHORTCUTEDIT_H


// 2025/07/22--代码已经审核通过


#include <QObject>

#include <QLineEdit>
#include <QKeyEvent>            // 键盘事件处理
#include <QKeySequence>         // 键盘序列处理
#include <QToolButton>
#include <QStyle>               // 控件样式处理

// 声明命名空间中的 QToolButton（前置声明，减少头文件依赖）
QT_BEGIN_NAMESPACE
class QToolButton;
QT_END_NAMESPACE


// 用于编辑键盘序列的自定义控件
// 基于QLineEdit修改，专门用于捕获和显示键盘快捷键组合

class ShortcutEdit : public QLineEdit
{
    Q_OBJECT
public:
    // 构造函数
    explicit ShortcutEdit(QWidget *parent = 0);

    // 键盘按下事件处理函数
    virtual void keyPressEvent(QKeyEvent *event);

    // 控件大小调整事件处理函数
    // 用于重新定位清除按钮，使其始终在编辑框右侧居中显示
    virtual void resizeEvent(QResizeEvent *);

private:
    // 判断按钮是否为修饰键
    bool isModifier(int key);

    // 清除按钮指针（点击时清空当前内容）
    QToolButton *mClearButton;

private slots:
    // 根据文本内容更新清除按钮状态
    void updateClearButton(const QString& text);
};

#endif // SHORTCUTEDIT_H

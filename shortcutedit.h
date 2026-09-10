#ifndef SHORTCUTEDIT_H
#define SHORTCUTEDIT_H


#include <QObject>

#include <QLineEdit>
#include <QKeyEvent>            // 键盘事件处理
#include <QKeySequence>         // 键盘序列处理
#include <QToolButton>
#include <QStyle>               // 控件样式处理

QT_BEGIN_NAMESPACE
class QToolButton;
QT_END_NAMESPACE

class ShortcutEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit ShortcutEdit(QWidget *parent = 0);

    virtual void keyPressEvent(QKeyEvent *event);
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

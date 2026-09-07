#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H


// 2025/08/09--代码已经审核通过



#include <QDialog>      // Qt对话框模块
#include <QTextEdit>        // Qt文本编辑框组件

#include "imagearea.h"      // 图像区域类
#include "datasingleton.h"  // 包含单例数据管理类

// 包含必要的Qt组件
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QFontDialog>
#include <QDialogButtonBox>
#include <QMessageBox>


// 文本工具对话框类
// 继承自QDialog，提供文本输入和字体选择功能
// 用于在图像区域上添加/编辑文本内容
class TextDialog : public QDialog
{
    Q_OBJECT    // 启用Qt的元对象系统（信号槽机制）
public:
    // 构造函数
    // text 初始文本内容  parent 指向父窗口（图像区域）的指针
    explicit TextDialog(QString text,ImageArea *parent);

private:
    // 初始化图形用户界面
    void initializeGui();

    QTextEdit *mTextEdit;   // 文本编辑框组件指针

signals:
    // 文本内容变更信号
    void textChanged(ImageArea *,const QString);
    // 取消操作信号
    void canceled(ImageArea*);

private slots:
    // 处理文本编辑框内容变化的槽函数
    void textChanged();
    // 字体选择槽函数
    void selectFont();
    // 取消操作槽函数
    void cancel();
    // 重写基类的拒绝操作，当用户按ESC或点击关闭按钮时调用cancel()
    void reject();

};

#endif // TEXTDIALOG_H

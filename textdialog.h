#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H


#include <QDialog>      // Qt对话框模块
#include <QTextEdit>        // Qt文本编辑框组件

#include "imagearea.h"      // 图像区域类
#include "datasingleton.h"  // 包含单例数据管理类

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QFontDialog>
#include <QDialogButtonBox>
#include <QMessageBox>



class TextDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TextDialog(QString text,ImageArea *parent);

private:
    void initializeGui();

    QTextEdit *mTextEdit;   // 文本编辑框组件指针

signals:
    void textChanged(ImageArea *,const QString);
    void canceled(ImageArea*);

private slots:
    void textChanged();
    void selectFont();
    void cancel();
    void reject();

};

#endif // TEXTDIALOG_H

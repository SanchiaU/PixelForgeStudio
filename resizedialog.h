#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include <QDialog>
#include <QObject>

#include<QLabel>
#include<QGridLayout>
#include<QRadioButton>
#include<QSpinBox>
#include<QtCore/QDebug>
#include<QDialogButtonBox>
#include<QCheckBox>
#include<QGroupBox>
#include<QVBoxLayout>

QT_BEGIN_NAMESPACE
class QLabel;
class QSpinBox;
class QCheckBox;
QT_END_NAMESPACE

/*
主要功能：图像尺寸调整对话框
*/

class ResizeDialog : public QDialog
{
    Q_OBJECT
public:
    //构造函数
    explicit ResizeDialog(const QSize &size,QWidget *parent);
    //获取用户设置的新尺寸 QSize新尺寸对象（宽、度值）
    inline QSize getNewSize(){return QSize(mWidth,mHeight);}
private:
    //初始化GUI组件布局；
    void initializeGui();

    int mWidth;//计算后的宽度
    int mHeight;
    int mOrigWidth;//原始宽度
    int mOrigHeight;

    QLabel *mNewSizeLabel;  //显示新尺寸信息的标签
    QSpinBox *mPixelWButton;
    QSpinBox *mPixelHButton;  //像素模式-高度输入框
    QSpinBox *mPercentWButton;
    QSpinBox *mPercentHButton;  //百分比模式-高度输入框
    QCheckBox *mPreserveAspectBox;  //保持高度比 复选框

private slots:
    //模式切换槽函数
    void pixelsButtonClicked(bool flag);
    void percentButtonClicked(bool flag);
    //数据变化槽函数
    void pixelsWValueChanged(const int &value);//像素宽度变化
    void pixelsHValueChanged(const int &value);
    void percentWValueChanged(const int &value);//百分比宽度变化
    void percentHValueChanged(const int &value);


};

#endif // RESIZEDIALOG_H

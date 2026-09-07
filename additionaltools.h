#ifndef ADDITIONALTOOLS_H
#define ADDITIONALTOOLS_H



// 2025/07/28--代码已经审核通过



#include <QObject>              // 引入Qt核心对象基类
#include <QImage>               // 引入Qt图像处理类
#include <QPainter>             // Qt绘图工具
#include <QLabel>               // Qt标签组件
#include <QTransform>           // 几何变换（旋转/缩放）
#include <QSize>                // 尺寸对象
#include <QClipboard>           // 剪贴板功能
#include <QApplication>         // Qt应用核心

// 开始Qt命名空间声明（兼容性处理）
QT_BEGIN_NAMESPACE
class ImageArea;  // 前置声明ImageArea类
class QSize;      // 前置声明Qt尺寸类
QT_END_NAMESPACE

// 实现额外图像处理工具的类，用于改变图像状态
// 此类提供画布调整、图像旋转、缩放等功能，通过信号/槽机制与界面交互
class AdditionalTools : public QObject
{
    Q_OBJECT            // 启用Qt元对象系统（信号/槽/属性）
public:
    // 构造函数
    // pImageArea 指向图像显示区域的指针
    // parent 父对象指针（用于内存管理）
    explicit AdditionalTools(ImageArea *pImageArea, QObject *parent);

    // 调整画布尺寸
    void resizeCanvas(int width, int height, bool flag = false);

    // 调整图像尺寸（可能保持宽度比或适应画布）
    void resizeImage();

    // 旋转图像
    // flag 旋转方向标识（true表示顺时针，false表示逆时针）
    void rotateImage(bool flag);

    // 缩放图像
    // factor 缩放因子（>1放大  <1缩小）
    bool zoomImage(qreal factor);

private:
    ImageArea *mPImageArea; // 指向图像显示区域的指针（操作核心图像数据）
    qreal mZoomedFactor;    // 当前缩放因子（相对于原始图像的缩放比例）

signals:
    // 发送新图像尺寸信号
    void sendNewImageSize(const QSize&);

};

#endif // ADDITIONALTOOLS_H

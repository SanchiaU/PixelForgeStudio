#ifndef ABSTRACTINSTRUMENT_H
#define ABSTRACTINSTRUMENT_H

// 包含必要Qt头文件
#include <QtCore/QObject>   // Qt核心对象基类
#include <QMouseEvent>      // 鼠标事件处理
#include <QImage>           // 图像处理类

// 声明前置引用（避免循环依赖）
QT_BEGIN_NAMESPACE
class ImageArea; // 声明ImageArea类
QT_END_NAMESPACE


// 抽象绘图工具基类
// 定义所有绘图工具（比如：画笔、橡皮檫、直线等）必须实现的通用接口和行为
class AbstractInstrument : public QObject
{
    Q_OBJECT  // 启用Qt的元对象系统（信号槽机制）
public:
    explicit AbstractInstrument(QObject *parent = 0);
    virtual ~AbstractInstrument(){}
    // ======= 纯虚函数（接口契约）========子类必须实现这些鼠标事件处理函数
    virtual void mousePressEvent(QMouseEvent *event, ImageArea &imageArea) = 0;
    virtual void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea) = 0;

protected:
    // 工具使用的坐标点（记录起始点和结束点）
    QPoint mStartPoint, mEndPoint;
    // 图像副本（用于临时保存编辑前的图像状态）
    QImage mImageCopy;
    // 核心绘图方法（纯虚函数）
    virtual void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false) = 0;
    // 创建撤销命令（默认实现）
    // 基类实现：保存整个图像状态到撤销栈
    // 子类可重写此方法实现更高效的局部撤销
    virtual void makeUndoCommand(ImageArea &imageArea);

};

#endif // ABSTRACTINSTRUMENT_H

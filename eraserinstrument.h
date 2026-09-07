#ifndef ERASERINSTRUMENT_H
#define ERASERINSTRUMENT_H



// 2025/08/02--代码已经审核通过




#include <QObject>

// 包含Qt核心功能模块
#include <QtCore/QObject>


#include "abstractinstrument.h"     // 包含基类头文件
#include "imagearea.h"              // 图像区域管理类
#include "datasingleton.h"          // 单例数据管理（存储画笔大小等设置）


#include <QPen>                   // Qt画笔类
#include <QPainter>               // Qt绘图类


// 橡皮擦工具类
// 继承自AbstractInstrument类，实现图像擦除功能
// 通过覆盖鼠标事件和绘制方法，实现在图像区域上面擦除像素（通常用背景色覆盖）
class EraserInstrument : public AbstractInstrument
{
    Q_OBJECT            // 启用Qt的元对象系统（信号/槽机制）
public:
    // 构造函数
    explicit EraserInstrument(QObject *parent = 0);  // 初始化父对象

    // 鼠标事件处理函数
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea) override;    // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea) override;     // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea) override;  // 鼠标释放事件

protected:
    // 执行实际绘制/擦除操作
    // imageArea 目标图像区域
    // isSecondaryColor 是否使用次要颜色（橡皮擦通常忽略此参数）
    // additionalFlag 附加标志位（用于特殊绘制模式）
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false) override;
};

#endif // ERASERINSTRUMENT_H









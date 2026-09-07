#ifndef RECTANGLEINSTRUMENT_H
#define RECTANGLEINSTRUMENT_H



// 2025/08/07--代码已经审核通过



#include "abstractinstrument.h"  // 包含基础工具类的定义
#include <QtCore/QObject>       // 包含Qt核心对象模块

#include "imagearea.h"          // 包含绘图区域类
#include "datasingleton.h"      // 包含全局数据单例

#include <QPen>                     // Qt画笔类
#include <QPainter>                 // Qt绘图工具


// 矩形绘制工具类
// 实现矩形绘制功能：
// 支持绘制空心矩形（边框）
// 支持绘制实心矩形（填充）
// 支持主色/次色选择
// 支持拖拽调整矩形大小
class RectangleInstrument : public AbstractInstrument  // 继承自抽象工具基类
{
    Q_OBJECT  // Qt宏，启用信号槽机制和元对象系统

public:
    // 构造函数
    explicit RectangleInstrument(QObject *parent = 0);

    // 鼠标按下事件处理
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标移动事件处理
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标释放事件处理
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea);

protected:
    // 实际绘制方法
    // 核心功能：根据起始点和终点计算矩形区域；使用QPainter绘制矩形；支持边框和填充模式
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false);

};

#endif // RECTANGLEINSTRUMENT_H



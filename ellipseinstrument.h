#ifndef ELLIPSEINSTRUMENT_H
#define ELLIPSEINSTRUMENT_H



// 2025/08/07--代码已经审核通过



#include "abstractinstrument.h"  // 基类声明，提供绘图工具的基础接口
#include <QtCore/QObject>       // Qt 核心对象模块，支持信号槽机制

#include "imagearea.h"          // 图像区域类
#include "datasingleton.h"      // 单例数据类，存储全局设置（颜色、笔刷大小等）

#include <QPen>                  // Qt 画笔类
#include <QPainter>              // Qt 绘图工具类

// 椭圆绘制工具类
// 通过鼠标拖拽确定椭圆的外接矩形，支持实时预览绘制效果
class EllipseInstrument : public AbstractInstrument
{
    Q_OBJECT  // 启用Qt的元对象系统（信号槽、属性等）

public:
    // 构造函数
    explicit EllipseInstrument(QObject *parent = 0);

    // 鼠标按下事件处理
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标移动事件处理
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标释放事件处理
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea);

protected:
    // 执行椭圆绘制操作
    // 实现细节：根据记录的起点和终点计算椭圆外接矩形；使用QPainter绘制椭圆（可带边框/填充）
    // 根据参数选择画笔/画刷颜色; 更新图像区域显示
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false);

};

#endif // ELLIPSEINSTRUMENT_H

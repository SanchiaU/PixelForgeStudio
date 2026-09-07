#ifndef LINEINSTRUMENT_H
#define LINEINSTRUMENT_H



// 2025/08/06--代码已经审核通过




// 声明线绘制工具类，用于在图像区域绘制直线


#include "abstractinstrument.h"  // 包含抽象工具基类
#include <QtCore/QObject>        // Qt核心对象类

#include "imagearea.h"              // 图像区域管理类
#include "datasingleton.h"          // 全局数据单例（颜色等）

#include <QPen>                    // Qt画笔类
#include <QPainter>                // Qt绘图工具
#include <QImage>                  // Qt图像类

// 线绘制工具类，继承自抽象工具基类
// 此类实现在图像区域通过鼠标事件绘制直线的功能
// 通过捕捉鼠标的按下、移动和释放事件来确定直线的起点和终点
// 并且复用双缓冲机制在临时图层上实现预览直线
class LineInstrument : public AbstractInstrument
{
    Q_OBJECT        // 启用Qt的元对象系统（信号/槽等）
public:

    // 构造函数
    explicit LineInstrument(QObject *parent = 0);

    // 鼠标按下事件处理
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标移动事件处理
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标释放事件处理
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea);

protected:
    // 执行实际绘制操作
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false);

};

#endif // LINEINSTRUMENT_H

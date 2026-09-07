#ifndef CURVELINEINSTRUMENT_H
#define CURVELINEINSTRUMENT_H


// 2025/08/09--代码已经审核通过


#include "abstractinstrument.h"
#include <QtCore/QObject>
#include <QtCore/QPoint>

#include "imagearea.h"      // 图像区域管理类
#include "datasingleton.h"  // 全局数据单例

#include <QPen>     // Qt画笔工具
#include <QPainter> // Qt绘图工具
#include <QImage>   // Qt图像处理
#include <QPainterPath>

// 曲线绘制工具类（继承自抽象绘图工具基类）
class CurveLineInstrument : public AbstractInstrument
{
    Q_OBJECT // Qt元对象系统宏，支持信号槽机制
public:
    // 构造函数
    explicit CurveLineInstrument(QObject *parent=0);

    // 鼠标事件处理函数（重写基类虚函数）
    virtual void mousePressEvent(QMouseEvent *event, ImageArea &imageArea);
    virtual void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea);
    virtual void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea);


protected:
    // 实际绘图曲线的方法
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false);

private:
    // 贝塞尔曲线控制点
    QPoint mFirstControlPoint; // 第一控制点（曲线起点）
    QPoint mSecondControlPoint; // 第二控制占（曲线终点）

    // 记录已采集的控制点数量
    // 0-无点，1=起点，2=终点
    unsigned int mPointsCount:2;
};

#endif // CURVELINEINSTRUMENT_H

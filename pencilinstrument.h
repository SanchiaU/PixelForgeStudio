#ifndef PENCILINSTRUMENT_H
#define PENCILINSTRUMENT_H




// 2025/08/05--代码已经审核通过




#include "abstractinstrument.h"         // 基类：抽象绘图工具
#include <QtCore/QObject>               // Qt核心对象

#include <QPen>     // Qt画笔工具
#include <QPainter> // Qt绘图工具

#include "imagearea.h"      // 图像区域管理类
#include "datasingleton.h"  // 全局数据单例（颜色、画笔大小等）


// 铅笔绘图工具类，继承自AbstractInstrument
// 实现基础自由绘制功能：
// 单像素级精度绘制、支持鼠标拖拽连续绘制、使用主/副颜色系统
class PencilInstrument : public AbstractInstrument
{
    Q_OBJECT  // 启用Qt元对象系统（信号槽等）

public:
    // 构造函数
    explicit PencilInstrument(QObject *parent = 0);

    // 鼠标按下事件处理
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea) override;

    // 鼠标移动事件处理
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea) override;

    // 鼠标释放事件处理
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea) override;


protected:
    // 核心绘制方法
    // 在鼠标轨迹上绘制单像素点
    // 使用QPainter进行抗锯齿绘制
    // 支持主/副颜色切换
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false) override;
};

#endif // PENCILINSTRUMENT_H

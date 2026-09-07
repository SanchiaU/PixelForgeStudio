#ifndef SPRAYINSTRUMENT_H
#define SPRAYINSTRUMENT_H



// 2025/08/06--代码已经审核通过



#include "abstractinstrument.h"  // 包含基础工具类的定义
#include <QtCore/QObject>       // 包含Qt核心对象模块

#include "imagearea.h"      // 包含绘制区域类
#include "datasingleton.h"  // 包含全局数据单例

#include <QPen>                     // Qt画笔类
#include <QPainter>                 // Qt绘图工具
#include <math.h>                   // 数学函数库


// 喷枪工具类（喷雾效果
// 实现类似真实喷枪的绘图效果：
// 鼠标按下时在随机位置生成小点
// 鼠标移动时持续喷射粒子
// 粒子分布具有随机性的扩散特性
class SprayInstrument : public AbstractInstrument  // 继承自抽象工具基类
{
    Q_OBJECT  // Qt宏，启用信号槽机制和元对象系统

public:
    // 构造函数
    explicit SprayInstrument(QObject *parent = 0);

    // 鼠标按下事件处理
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标移动事件处理
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标释放事件处理
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea);

protected:
    // 实际绘制方法
    // 以鼠标位置 为中心生成随机点，粒子密度和半径可配置，使用QPainter进行点绘制
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false);

};

#endif // SPRAYINSTRUMENT_H

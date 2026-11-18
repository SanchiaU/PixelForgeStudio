#ifndef RECTANGLEINSTRUMENT_H
#define RECTANGLEINSTRUMENT_H


#include "abstractinstrument.h"  // 包含基础工具类的定义
#include <QtCore/QObject>       // 包含Qt核心对象模块

#include "imagearea.h"          // 包含绘图区域类
#include "datasingleton.h"      // 包含全局数据单例

#include <QPen>                     // Qt画笔类
#include <QPainter>                 // Qt绘图工具

class RectangleInstrument : public AbstractInstrument
{
    Q_OBJECT

public:
    explicit RectangleInstrument(QObject *parent = 0);

    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea);

    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea);

    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea);

protected:
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false);

};

#endif // RECTANGLEINSTRUMENT_H



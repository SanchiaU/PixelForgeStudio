#include "lineinstrument.h"


LineInstrument::LineInstrument(QObject *parent) :
    AbstractInstrument(parent)  // 调用基类构造函数
{

}

void LineInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        mStartPoint = mEndPoint = event->pos();

        imageArea.setIsPaint(true);

        mImageCopy = *imageArea.getImage();

        makeUndoCommand(imageArea);
    }
}


void LineInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        mEndPoint = event->pos();

        imageArea.setImage(mImageCopy);

        if(event->buttons() & Qt::LeftButton)
        {
            paint(imageArea, false);
        }
        else if(event->buttons() & Qt::RightButton)
        {
            paint(imageArea, true);
        }
    }
}


void LineInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        imageArea.setImage(mImageCopy);

        if(event->button() == Qt::LeftButton)
        {
            paint(imageArea, false);
        }
        else if(event->button() == Qt::RightButton)
        {
            paint(imageArea, true);
        }

        imageArea.setIsPaint(false);
    }
}


void LineInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool)
{
    QPainter painter(imageArea.getImage());

    if(isSecondaryColor)
    {
        painter.setPen(QPen(
            DataSingleton::Instance()->getSecondaryColor(),  // 获取次色
            DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),  // 计算缩放后的笔刷大小
            Qt::SolidLine,   // 实线样式
            Qt::RoundCap,    // 圆形线帽
            Qt::RoundJoin    // 圆形连接点
            ));
    }
    else
    {
        painter.setPen(QPen(
            DataSingleton::Instance()->getPrimaryColor(),    // 获取主色
            DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),  // 缩放笔刷
            Qt::SolidLine,
            Qt::RoundCap,
            Qt::RoundJoin
            ));
    }

    if(mStartPoint != mEndPoint)
    {
        painter.drawLine(mStartPoint, mEndPoint);
    }
    else if(mStartPoint == mEndPoint)
    {
        painter.drawPoint(mStartPoint);
    }

    imageArea.setEdited(true);

    painter.end();

    imageArea.update();
}

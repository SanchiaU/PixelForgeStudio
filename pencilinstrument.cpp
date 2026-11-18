#include "pencilinstrument.h"


PencilInstrument::PencilInstrument(QObject *parent) :
    AbstractInstrument(parent)   // 调用基类构造函数
{

}
void PencilInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        mStartPoint = mEndPoint = event->pos();
        imageArea.setIsPaint(true);
        makeUndoCommand(imageArea);
    }
}
void PencilInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea){
    if(imageArea.isPaint()){
        mEndPoint = event->pos();
        if(event->buttons() & Qt::LeftButton)
        {
            paint(imageArea, false); // false表示使用主颜色
        }
        else if(event->buttons() & Qt::RightButton)
        {
            paint(imageArea, true);  // true表示使用副颜色
        }
        mStartPoint = event->pos();
    }
}
void PencilInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        mEndPoint = event->pos();
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


// 配置画笔属性（颜色、大小、样式）
void PencilInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool)
{
    QPainter painter(imageArea.getImage());

    if(isSecondaryColor)
    {
        painter.setPen(QPen(DataSingleton::Instance()->getSecondaryColor(),
                            DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                            Qt::SolidLine,       // 实线
                            Qt::RoundCap,        // 圆角端点
                            Qt::RoundJoin));     // 圆角连接
    }else{
        painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor(),
                            DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                            Qt::SolidLine,
                            Qt::RoundCap,
                            Qt::RoundJoin));
    }
    if(mStartPoint != mEndPoint){
        painter.drawLine(mStartPoint, mEndPoint);
    }else{
        painter.drawPoint(mStartPoint);
    }
    imageArea.setEdited(true);
    painter.end();
    imageArea.update();
}

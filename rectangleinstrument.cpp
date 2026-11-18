#include "rectangleinstrument.h"

RectangleInstrument::RectangleInstrument(QObject *parent) :
    AbstractInstrument(parent)
{}
void RectangleInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        mStartPoint = mEndPoint = event->pos();
        imageArea.setIsPaint(true);
        mImageCopy = *imageArea.getImage();
        makeUndoCommand(imageArea);
    }
}

void RectangleInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        mEndPoint = event->pos();
        imageArea.setImage(mImageCopy);
        if(event->buttons() & Qt::LeftButton)
        {
            paint(imageArea, false); // 左键：仅边框
        }
        else if(event->buttons() & Qt::RightButton)
        {
            paint(imageArea, true); // 右键：边框+填充
        }
    }
}
void RectangleInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
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

void RectangleInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool)
{
    QPainter painter(imageArea.getImage());
    painter.setPen(QPen(
        DataSingleton::Instance()->getPrimaryColor(),
        DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    if(isSecondaryColor){
        painter.setBrush(QBrush(DataSingleton::Instance()->getSecondaryColor()));
    }
    if(mStartPoint != mEndPoint){
        painter.drawRect(QRect(mStartPoint, mEndPoint));
    }
    imageArea.setEdited(true);
    painter.end();
    imageArea.update();
}

#include "eraserinstrument.h"


EraserInstrument::EraserInstrument(QObject *parent):
    AbstractInstrument(parent)
{

}

void EraserInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        mStartPoint = mEndPoint = event->pos();

        imageArea.setIsPaint(true);

        makeUndoCommand(imageArea);
    }
}

void EraserInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        mEndPoint = event->pos();

        paint(imageArea, false);

        mStartPoint = event->pos();
    }
}

void EraserInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        mEndPoint = event->pos();

        paint(imageArea);

        imageArea.setIsPaint(false);
    }
}

void EraserInstrument::paint(ImageArea &imageArea, bool /* isSecondaryColor */, bool /* additionalFlag */)
{
    QPainter painter(imageArea.getImage());

    painter.setPen(QPen(Qt::white,
                        DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

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



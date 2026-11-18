#include "ellipseinstrument.h"

EllipseInstrument::EllipseInstrument(QObject *parent) :
    AbstractInstrument(parent)  // 调用基类构造函数
{

}

void EllipseInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        mStartPoint = mEndPoint = event->pos();

        imageArea.setIsPaint(true);

        mImageCopy = *imageArea.getImage();

        makeUndoCommand(imageArea);
    }
}
void EllipseInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())  // 检查是否处于绘图状态
    {
        mEndPoint = event->pos();
        imageArea.setImage(mImageCopy);
        if(event->buttons() & Qt::LeftButton)
        {
            paint(imageArea, false);  // isSecondaryColor = false
        }
        else if(event->buttons() & Qt::RightButton)
        {
            paint(imageArea, true);   // isSecondaryColor = true
        }
    }
}

void EllipseInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
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
void EllipseInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool /*additionalFlag*/)
{
    QPainter painter(imageArea.getImage());

    // 配置画笔属性：
    // - 颜色：主色（isSecondaryColor=false）或辅色（true）
    // - 大小：全局笔刷大小 × 当前缩放因子（保证不同缩放级别下视觉大小一致）
    // - 样式：实线/圆角端点/圆角连接
    painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor(),
                        DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if(isSecondaryColor)
    {
        painter.setBrush(QBrush(DataSingleton::Instance()->getSecondaryColor()));
    }

    if(mStartPoint != mEndPoint)
    {
        painter.drawEllipse(QRect(mStartPoint, mEndPoint));
    }
    imageArea.setEdited(true);
    painter.end();
    imageArea.update();
}





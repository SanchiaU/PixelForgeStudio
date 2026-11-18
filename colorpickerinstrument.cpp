#include "colorpickerinstrument.h"


ColorpickerInstrument::ColorpickerInstrument(QObject *parent):
    AbstractInstrument(parent)      // 调用基类构造函数
{

}


void ColorpickerInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        imageArea.setIsPaint(true);
    }
}


void ColorpickerInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    QRgb pixel(imageArea.getImage()->pixel(event->pos()));

    QColor getColor(pixel);

    imageArea.emitColor(getColor);
}


void ColorpickerInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        mStartPoint = mEndPoint = event->pos();

        if(event->button() == Qt::LeftButton)
        {
            paint(imageArea, false);
            // 发送主颜色更新信号（更新界面显示）
            imageArea.emitPrimaryColorView();
        }
        else if(event->button() == Qt::RightButton)
        {
            paint(imageArea, true);
            imageArea.emitSecondaryColorView();
        }

        imageArea.setIsPaint(false);

        // 发送信号恢复之前的工具（颜色拾取是临时工具）
        imageArea.emitRestorePreviousInstrument();
    }
}

// 实际颜色设置实现函数
// 根据坐标位置获取像素颜色，并更新到全局颜色设置：
// 1.检查坐标是否在图像范围内
// 2.获取指定位置像素颜色
// 3.更新单例中的颜色值
void ColorpickerInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool /*additionalFlag*/)
{
    bool inArea(true);

    if(mStartPoint.x() < 0 || mStartPoint.y() < 0
        || mStartPoint.x() > imageArea.getImage()->width()
        || mStartPoint.y() > imageArea.getImage()->height())
    {
        inArea = false;
    }

    if(inArea && isSecondaryColor)
    {
        QRgb pixel(imageArea.getImage()->pixel(mStartPoint));
        QColor getColor(pixel);

        DataSingleton::Instance()->setSecondaryColor(getColor);
    }

    if(inArea)
    {
        QRgb pixel(imageArea.getImage()->pixel(mStartPoint));
        QColor getColor(pixel);

        DataSingleton::Instance()->setPrimaryColor(getColor);
    }
}

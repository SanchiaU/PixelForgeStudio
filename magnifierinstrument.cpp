#include "magnifierinstrument.h"


// 构造函数
MagnifierInstrument::MagnifierInstrument(QObject *parent) :
    AbstractInstrument(parent)  // 调用基类构造函数
{

}
// 鼠标按下事件处理
void MagnifierInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅处理左键和右键事件
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        imageArea.setIsPaint(true);  // 设置绘图状态标志
    }
}
void MagnifierInstrument::mouseMoveEvent(QMouseEvent *, ImageArea &)
{

}
// 鼠标释放事件处理
void MagnifierInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅在有效绘图状态下处理
    if(imageArea.isPaint())
    {
        // 左键释放 - 放大操作
        if(event->button() == Qt::LeftButton)
        {
            // 尝试放大图像（2倍）
            if(imageArea.zoomImage(2.0))
            {
                imageArea.setZoomFactor(2.0);  // 更新缩放因子
            }
        }
        // 右键释放 - 缩小操作
        else if(event->button() == Qt::RightButton)
        {
            // 尝试缩小图像（0.5倍）
            if(imageArea.zoomImage(0.5))
            {
                imageArea.setZoomFactor(0.5);  // 更新缩放因子
            }
        }
        imageArea.setIsPaint(false);  // 重置绘图状态标志
    }
}
// 绘制函数，未使用到
void MagnifierInstrument::paint(ImageArea &, bool , bool )
{}

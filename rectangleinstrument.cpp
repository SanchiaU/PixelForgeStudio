#include "rectangleinstrument.h"

// 矩形工具构造函数
RectangleInstrument::RectangleInstrument(QObject *parent) :
    AbstractInstrument(parent)  // 调用基类构造函数
{

}


// 鼠标按下事件处理
void RectangleInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅响应左键或右键按下
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // 记录起始点和结束点为当前鼠标位置
        mStartPoint = mEndPoint = event->pos();
        // 设置绘图区域为"正在绘制"状态
        imageArea.setIsPaint(true);
        // 保存当前图像状态用于实时预览
        mImageCopy = *imageArea.getImage();
        // 创建撤销命令快照
        makeUndoCommand(imageArea);
    }
}

// 鼠标移动事件处理
void RectangleInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 检查是否处于绘图状态
    if(imageArea.isPaint())
    {
        // 更新结束点为当前鼠标位置
        mEndPoint = event->pos();
        // 恢复原始图像（擦除之前绘制的预览矩形）
        imageArea.setImage(mImageCopy);

        // 根据鼠标按键选择绘制模式
        if(event->buttons() & Qt::LeftButton)
        {
            // 左键：仅边框
            paint(imageArea, false);
        }
        else if(event->buttons() & Qt::RightButton)
        {
            // 右键：边框+填充
            paint(imageArea, true);
        }
    }
}
// 鼠标释放事件处理
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

        // 结束绘图状态
        imageArea.setIsPaint(false);
    }
}

// 矩形绘制核心方法
// 实现原理：设置画笔属性（主色边框）；左键时设置填充画刷（次色填充）；绘制矩形（从起始点到结束点）
void RectangleInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool)
{
    QPainter painter(imageArea.getImage());
    painter.setPen(QPen(
        DataSingleton::Instance()->getPrimaryColor(),  // 边框颜色：主色
        DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),  // 边框宽度（考虑缩放因子）
        Qt::SolidLine,   // 实线
        Qt::RoundCap,    // 圆形端点
        Qt::RoundJoin    // 圆形连接
        ));
    if(isSecondaryColor)
    {
        painter.setBrush(QBrush(DataSingleton::Instance()->getSecondaryColor()));
    }
    if(mStartPoint != mEndPoint)
    {
        painter.drawRect(QRect(mStartPoint, mEndPoint));
    }
    imageArea.setEdited(true);
    painter.end();
    imageArea.update();
}

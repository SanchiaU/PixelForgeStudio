#include "lineinstrument.h"



// 2025/08/06--代码已经审核通过



// 构造函数
LineInstrument::LineInstrument(QObject *parent) :
    AbstractInstrument(parent)  // 调用基类构造函数
{

}

// 鼠标按下事件处理
void LineInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅处理左键或右键点击
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // 记录起点和终点（初始位置相同）
        mStartPoint = mEndPoint = event->pos();

        // 设置绘制状态标志（表示开始绘制）
        imageArea.setIsPaint(true);

        // 备份当前图像状态（用于实时预览）
        mImageCopy = *imageArea.getImage();

        // 创建撤销命令（用于实现撤销/重做功能）
        makeUndoCommand(imageArea);
    }
}


// 鼠标移动事件处理
void LineInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅在绘制状态下处理移动事件
    if(imageArea.isPaint())
    {
        // 更新终点位置
        mEndPoint = event->pos();

        // 恢复原始图像（清除上一帧的预览）
        imageArea.setImage(mImageCopy);

        // 根据鼠标按键选择绘制模式
        if(event->buttons() & Qt::LeftButton)
        {
            // 使用主色绘制预览
            paint(imageArea, false);
        }
        else if(event->buttons() & Qt::RightButton)
        {
            // 使用次色（如橡皮擦）绘制预览
            paint(imageArea, true);
        }
    }
}


// 鼠标释放事件处理
void LineInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅在绘制状态下处理释放事件
    if(imageArea.isPaint())
    {
        // 恢复原始图像状态
        imageArea.setImage(mImageCopy);

        // 根据鼠标按键确定最终绘制颜色
        if(event->button() == Qt::LeftButton)
        {
            paint(imageArea, false);  // 使用主色绘制
        }
        else if(event->button() == Qt::RightButton)
        {
            paint(imageArea, true);   // 使用次色绘制
        }

        // 清除绘制状态标志
        imageArea.setIsPaint(false);
    }
}


// 执行实际绘制操作
void LineInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool)
{
    // 创建绘图工具，绑定到图像区域的图像
    QPainter painter(imageArea.getImage());

    // 设置画笔属性
    if(isSecondaryColor)
    {
        // 使用次要颜色（如背景色/橡皮擦）
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
        // 使用主要颜色（前景色）
        painter.setPen(QPen(
            DataSingleton::Instance()->getPrimaryColor(),    // 获取主色
            DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),  // 缩放笔刷
            Qt::SolidLine,
            Qt::RoundCap,
            Qt::RoundJoin
            ));
    }

    // 绘制直线（当起点和终点不同时）
    if(mStartPoint != mEndPoint)
    {
        painter.drawLine(mStartPoint, mEndPoint);
    }
    // 绘制单点（当点击未移动时）
    else if(mStartPoint == mEndPoint)
    {
        painter.drawPoint(mStartPoint);
    }

    // 标记图像已被编辑
    imageArea.setEdited(true);

    // 结束绘制
    painter.end();

    // 请求更新整个图像区域
    imageArea.update();
}



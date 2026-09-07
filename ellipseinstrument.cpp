#include "ellipseinstrument.h"




// 2025/08/07--代码已经审核通过




// 椭圆工具构造函数
EllipseInstrument::EllipseInstrument(QObject *parent) :
    AbstractInstrument(parent)  // 调用基类构造函数
{

}

// 鼠标按下事件处理
void EllipseInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅处理左右键事件
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // 记录起始点和终点（初始为同一点）
        mStartPoint = mEndPoint = event->pos();

        // 设置绘图状态标志
        imageArea.setIsPaint(true);

        // 备份当前图像状态（用于绘制过程中的实时预览）
        mImageCopy = *imageArea.getImage();

        // 创建撤销点（允许用户撤销此次绘制）
        makeUndoCommand(imageArea);
    }
}


// 鼠标移动事件处理
void EllipseInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())  // 检查是否处于绘图状态
    {
        // 更新终点坐标
        mEndPoint = event->pos();

        // 恢复按下时的原始图像（清除前一帧的临时绘制）
        imageArea.setImage(mImageCopy);

        // 左键绘制（主色）
        if(event->buttons() & Qt::LeftButton)
        {
            paint(imageArea, false);  // isSecondaryColor = false
        }
        // 右键绘制（辅色）
        else if(event->buttons() & Qt::RightButton)
        {
            paint(imageArea, true);   // isSecondaryColor = true
        }
    }
}

// 鼠标释放事件处理
void EllipseInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        // 恢复原始图像
        imageArea.setImage(mImageCopy);

        // 根据按键状态执行最终绘制
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


// 实际执行椭圆绘制
void EllipseInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool /*additionalFlag*/)
{
    // 创建绘图工具，目标为图像区域的底层图像
    QPainter painter(imageArea.getImage());

    // 配置画笔属性：
    // - 颜色：主色（isSecondaryColor=false）或辅色（true）
    // - 大小：全局笔刷大小 × 当前缩放因子（保证不同缩放级别下视觉大小一致）
    // - 样式：实线/圆角端点/圆角连接
    painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor(),
                        DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    // 设置画刷（填充颜色）
    if(isSecondaryColor)
    {
        // 使用辅色填充椭圆内部
        painter.setBrush(QBrush(DataSingleton::Instance()->getSecondaryColor()));
    }

    // 仅当起点和终点不同时绘制（避免绘制点）
    if(mStartPoint != mEndPoint)
    {
        // 根据起点和终点计算外接矩形并绘制椭圆
        painter.drawEllipse(QRect(mStartPoint, mEndPoint));
    }

    // 标记图像已被编辑
    imageArea.setEdited(true);


    // 结束绘图（释放资源）
    painter.end();

    // 更新整个图像区域（实际应用中可优化为局部更新）
    imageArea.update();
}





#include "eraserinstrument.h"




// 2025/08/02--代码已经审核通过




// 构造函数
EraserInstrument::EraserInstrument(QObject *parent):
    AbstractInstrument(parent)
{

}

// 鼠标按下事件处理
void EraserInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅响应左键或右键点击（支持左右键擦除）
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // 记录鼠标按下的起始位置
        mStartPoint = mEndPoint = event->pos();

        // 设置绘制标志，表示开始擦除操作
        imageArea.setIsPaint(true);

        // 创建撤销命令（用于支持撤销操作）
        makeUndoCommand(imageArea);
    }
}

// 鼠标移动事件处理
void EraserInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 检查是否处于绘制状态
    if(imageArea.isPaint())
    {
        // 更新当前鼠标位置
        mEndPoint = event->pos();

        // 执行绘制/擦除操作（第二个参数false表示不使用次要颜色）
        paint(imageArea, false);

        // 将当前点设为新的起点（为连续绘制做准备）
        mStartPoint = event->pos();
    }
}

// 鼠标释放事件处理
void EraserInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        // 记录鼠标释放位置
        mEndPoint = event->pos();

        // 执行最终的绘制/擦除操作
        paint(imageArea);

        // 清除绘制状态标志
        imageArea.setIsPaint(false);
    }
}

// 执行实际绘制/擦除操作
void EraserInstrument::paint(ImageArea &imageArea, bool /* isSecondaryColor */, bool /* additionalFlag */)
{
    // 在图像区域上创建绘制工具
    QPainter painter(imageArea.getImage());

    painter.setPen(QPen(Qt::white,
                        DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    // 处理移动擦除（线段）
    if(mStartPoint != mEndPoint)
    {
        // 在起始点和终点之间绘制直线（形成连续擦除轨迹）
        painter.drawLine(mStartPoint, mEndPoint);
    }
    // 处理点击擦除（单点）
    else if(mStartPoint == mEndPoint)
    {
        // 在单点位置绘制一个点
        painter.drawPoint(mStartPoint);
    }

    // 标记图像已被修改
    imageArea.setEdited(true);


    // 结束绘制（自动释放资源）
    painter.end();

    // 请求重绘整个图像区域（实际应用中可优化为局部更新）
    imageArea.update();
}



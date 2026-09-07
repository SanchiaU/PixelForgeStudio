#include "pencilinstrument.h"




// 2025/08/05--代码已经审核通过




// 铅笔工具构造函数
PencilInstrument::PencilInstrument(QObject *parent) :
    AbstractInstrument(parent)   // 调用基类构造函数
{

}

// 鼠标按下事件处理
void PencilInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 只处理左右键点击
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // 记录起始点和结束点（初始为同一点）
        mStartPoint = mEndPoint = event->pos();

        // 设置绘图状态标志
        imageArea.setIsPaint(true);

        // 创建撤销命令快照
        makeUndoCommand(imageArea);
    }
}


// 鼠标移动事件处理
void PencilInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 检查是否处于绘图状态
    if(imageArea.isPaint())
    {
        // 更新结束点为当前鼠标位置
        mEndPoint = event->pos();

        // 左键绘制（主颜色）
        if(event->buttons() & Qt::LeftButton)
        {
            paint(imageArea, false); // false表示使用主颜色
        }
        // 右键绘制（副颜色）
        else if(event->buttons() & Qt::RightButton)
        {
            paint(imageArea, true);  // true表示使用副颜色
        }

        // 更新起始点为当前位置（为下个移动事件准备）
        mStartPoint = event->pos();
    }
}


// 鼠标释放事件处理
void PencilInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        // 设置结束点为释放位置
        mEndPoint = event->pos();

        // 根据释放的按键选择颜色
        if(event->button() == Qt::LeftButton)
        {
            paint(imageArea, false);
        }
        else if(event->button() == Qt::RightButton)
        {
            paint(imageArea, true);
        }

        // 清除绘图状态标志
        imageArea.setIsPaint(false);
    }
}


// 核心绘制方法：
// 配置画笔属性（颜色、大小、样式）
// 绘制直线或点、更新图像区域状态
void PencilInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool)
{
    // 创建画家对象，操作图像区域的底层图像
    QPainter painter(imageArea.getImage());

    // 根据颜色选择配置画笔
    if(isSecondaryColor)
    {
        // 使用副颜色：考虑缩放因子调整画笔大小
        painter.setPen(QPen(DataSingleton::Instance()->getSecondaryColor(),
                            DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                            Qt::SolidLine,       // 实线
                            Qt::RoundCap,        // 圆角端点
                            Qt::RoundJoin));     // 圆角连接
    }
    else
    {
        // 使用主颜色
        painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor(),
                            DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                            Qt::SolidLine,
                            Qt::RoundCap,
                            Qt::RoundJoin));
    }

    // 绘制逻辑
    if(mStartPoint != mEndPoint)
    {
        // 两点不同时绘制线段（连续轨迹）
        painter.drawLine(mStartPoint, mEndPoint);
    }
    else
    {
        // 单点绘制（点击时）
        painter.drawPoint(mStartPoint);
    }

    // 标记图像已被编辑
    imageArea.setEdited(true);

    // 结束绘制操作
    painter.end();

    // 请求重绘整个图像区域（简单实现）
    imageArea.update();
}

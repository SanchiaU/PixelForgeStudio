#include "pencilinstrument.h"


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
void PencilInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea){
    if(imageArea.isPaint()){
        mEndPoint = event->pos();
        if(event->buttons() & Qt::LeftButton)
        {
            paint(imageArea, false); // false表示使用主颜色
        }
        else if(event->buttons() & Qt::RightButton)
        {
            paint(imageArea, true);  // true表示使用副颜色
        }
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
    QPainter painter(imageArea.getImage());

    if(isSecondaryColor)
    {
        painter.setPen(QPen(DataSingleton::Instance()->getSecondaryColor(),
                            DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                            Qt::SolidLine,       // 实线
                            Qt::RoundCap,        // 圆角端点
                            Qt::RoundJoin));     // 圆角连接
    }else{
        painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor(),
                            DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),
                            Qt::SolidLine,
                            Qt::RoundCap,
                            Qt::RoundJoin));
    }
    if(mStartPoint != mEndPoint){
        painter.drawLine(mStartPoint, mEndPoint);
    }else{
        painter.drawPoint(mStartPoint);
    }
    imageArea.setEdited(true);
    painter.end();
    imageArea.update();
}

#include "curvelineinstrument.h"


// 2025/08/09--代码已经审核通过



// 构造函数
CurveLineInstrument::CurveLineInstrument(QObject *parent):
    AbstractInstrument(parent)      // 调用基类构造函数
{
    mPointsCount=0;      // 初始化控制点计数为0（无控制点）
}

// 鼠标按下事件处理
void CurveLineInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅处理左键或右键点击
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // 检查是否需要重置曲线状态（来自全局单例）
        if(DataSingleton::Instance()->isResetCurve())
        {
            mPointsCount = 0;  // 重置控制点计数
            DataSingleton::Instance()->setResetCurve(false);  // 清除重置标志
        }

        // 根据当前控制点数量执行不同操作
        switch(mPointsCount)
        {
        case 0: // 初始状态：开始新曲线
            mImageCopy = *imageArea.getImage();  // 保存当前图像副本用于重绘

            // 初始化所有点为当前点击位置
            mStartPoint = mEndPoint = mFirstControlPoint = mSecondControlPoint = event->pos();
            ++mPointsCount;  // 进入第一个控制点状态
            break;

        case 1: // 已有起点：设置第一个控制点
            mFirstControlPoint = mSecondControlPoint = event->pos();  // 初始化为同一位置
            ++mPointsCount;  // 进入第二个控制点状态
            break;

        case 2: // 已有两个点：设置第二个控制点
            mSecondControlPoint = event->pos();
            mPointsCount = 0;  // 重置状态（曲线完成）
            break;
        }

        imageArea.setIsPaint(true);  // 标记开始绘制
        makeUndoCommand(imageArea);   // 创建撤销命令（保存当前状态）
    }

}

// 鼠标移动事件处理（实时绘制预览）
void CurveLineInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅在绘制状态下处理
    if(imageArea.isPaint())
    {
        // 根据控制点数量更新不同点
        switch(mPointsCount)
        {
        case 1: // 仅起点：更新终点位置（直线预览）
            mEndPoint = event->pos();
            break;

        case 2: // 有起点和第一个控制点：同时更新两个控制点（二次贝塞尔预览）
            mFirstControlPoint = mSecondControlPoint = event->pos();
            break;

        case 0: // 所有点已确定：仅更新第二个控制点（三次贝塞尔调整）
            mSecondControlPoint = event->pos();
            break;
        }

        // 重置图像到初始状态（避免重影）
        imageArea.setImage(mImageCopy);

        // 根据鼠标按键选择颜色
        if(event->buttons() & Qt::LeftButton)
            paint(imageArea, false);   // 使用主色绘制
        else if(event->buttons() & Qt::RightButton)
            paint(imageArea, true);    // 使用辅色绘制
    }

}


// 鼠标释放事件处理（完成绘制）
void CurveLineInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    if(imageArea.isPaint())
    {
        // 恢复初始图像状态
        imageArea.setImage(mImageCopy);

        // 根据按键确定颜色
        if(event->button() == Qt::LeftButton)
            paint(imageArea, false);
        else if(event->button() == Qt::RightButton)
            paint(imageArea, true);

        imageArea.setIsPaint(false);  // 结束绘制状态
    }
}

// 实际绘制曲线方法
void CurveLineInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool)
{
    QPainter painter(imageArea.getImage());  // 在图像上创建绘图工具

    // 创建贝塞尔曲线路径
    QPainterPath path;
    path.moveTo(mStartPoint);  // 移动到起点
    // 三次贝塞尔曲线：起点→控制点1→控制点2→终点
    path.cubicTo(mFirstControlPoint, mSecondControlPoint, mEndPoint);

    // 配置画笔属性
    painter.setPen(QPen(
        isSecondaryColor ? DataSingleton::Instance()->getSecondaryColor() :  // 选择颜色
            DataSingleton::Instance()->getPrimaryColor(),
        DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor(),  // 计算缩放后笔触大小
        Qt::SolidLine,     // 实线样式
        Qt::RoundCap,      // 圆形线帽
        Qt::RoundJoin      // 圆形连接点
        ));

    // 绘制路径
    painter.strokePath(path, painter.pen());

    // 更新状态
    imageArea.setEdited(true);  // 标记图像已被编辑
    painter.end();             // 结束绘制
    imageArea.update();        // 请求界面重绘
}





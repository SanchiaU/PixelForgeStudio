#include "sprayinstrument.h"



// 2025/08/06--代码已经审核通过



// 喷枪工具构造函数
SprayInstrument::SprayInstrument(QObject *parent) :
    AbstractInstrument(parent)  // 调用基类构造函数
{

}


// 鼠标按下事件处理
void SprayInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅响应左键或右键按下
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // 记录起始点和结束点为当前鼠标位置
        mStartPoint = mEndPoint = event->pos();

        // 设置绘图区域为"正在绘制"状态
        imageArea.setIsPaint(true);

        // 创建撤销命令快照
        makeUndoCommand(imageArea);
    }
}


// 鼠标移动事件处理
void SprayInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 检查是否处于绘图状态
    if(imageArea.isPaint())
    {
        // 更新结束点为当前鼠标位置
        mEndPoint = event->pos();

        // 根据鼠标按键选择颜色
        if(event->buttons() & Qt::LeftButton)
        {
            // 使用主色绘制
            paint(imageArea, false);
        }
        else if(event->buttons() & Qt::RightButton)
        {
            // 使用次色绘制
            paint(imageArea, true);
        }

        // 更新起始点为当前位置，为下一次移动做准备
        mStartPoint = event->pos();
    }
}
// 鼠标释放事件处理
void SprayInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 检查是否处于绘图状态
    if(imageArea.isPaint())
    {
        // 根据鼠标按键选择颜色
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


// 喷枪核心绘制函数
// 实现原理：在鼠标位置周围生成随机点，使用三层不同半径的粒子分布，粒子密度内密外疏
void SprayInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool)
{
    // 创建画家对象，操作绘图区域的图像
    QPainter painter(imageArea.getImage());

    // 设置画笔属性
    if(isSecondaryColor)
    {
        // 使用次色
        painter.setPen(QPen(DataSingleton::Instance()->getSecondaryColor(),
                            // 笔刷大小 = sqrt(基础大小 * 缩放因子)
                            // 这样缩放时保持视觉效果一致
                            sqrt(DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor()),
                            Qt::SolidLine,       // 实线
                            Qt::RoundCap,        // 圆形端点
                            Qt::RoundJoin));      // 圆形连接
    }
    else
    {
        // 使用主色
        painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor(),
                            sqrt(DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor()),
                            Qt::SolidLine,
                            Qt::RoundCap,
                            Qt::RoundJoin));
    }

    int x, y; // 随机偏移量

    // 循环生成12个粒子点（三层分布）
    for(int i(0); i < 12; i++)
    {
        // 根据索引决定粒子分布范围
        switch(i) {
        // 内层粒子（索引0-3）：小范围[-2,2]
        case 0: case 1: case 2: case 3:
            x = (qrand() % 5 - 2) * sqrt(DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor());
            y = (qrand() % 5 - 2) * sqrt(DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor());
            break;

        // 中层粒子（索引4-7）：中范围[-4,4]
        case 4: case 5: case 6: case 7:
            x = (qrand() % 10 - 4) * sqrt(DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor());
            y = (qrand() % 10 - 4) * sqrt(DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor());
            break;

        // 外层粒子（索引8-11）：大范围[-7,7]
        case 8: case 9: case 10: case 11:
            x = (qrand() % 15 - 7) * sqrt(DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor());
            y = (qrand() % 15 - 7) * sqrt(DataSingleton::Instance()->getPenSize() * imageArea.getZoomFactor());
            break;
        }

        // 在鼠标位置加上随机偏移绘制点
        painter.drawPoint(mEndPoint.x() + x,
                          mEndPoint.y() + y);
    }

    // 标记图像已被修改
    imageArea.setEdited(true);
    // 结束绘制
    painter.end();
    // 更新绘图区域显示
    imageArea.update();
}





#include "colorpickerinstrument.h"



// 2025/08/02--代码已经审核通过




// 构造函数
ColorpickerInstrument::ColorpickerInstrument(QObject *parent):
    AbstractInstrument(parent)      // 调用基类构造函数
{

}

// 鼠标按下事件
// 当鼠标按下时拾取当前像素颜色，并设置为当前主颜色
void ColorpickerInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 仅处理左键或右键按下事件
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        // 设置标志位表示开始拾取操作
        imageArea.setIsPaint(true);
    }
}

// 鼠标移动事件
// 通常用于实现拖动时连续拾取颜色
void ColorpickerInstrument::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 获取鼠标当前位置的像素值
    QRgb pixel(imageArea.getImage()->pixel(event->pos()));

    // 将像素值转换为QColor对象
    QColor getColor(pixel);

    // 发送颜色预览信号（用于界面实时显示颜色值）
    imageArea.emitColor(getColor);
}

// 鼠标释放事件
// 执行最后的清理操作
void ColorpickerInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 检查是否处于有效拾取状态
    if(imageArea.isPaint())
    {
        // 记录起始点和结束点（此处使用相同点，因为只需单点采样）
        mStartPoint = mEndPoint = event->pos();

        // 左键释放：设置主颜色
        if(event->button() == Qt::LeftButton)
        {
            // 调用paint函数实际设置颜色（false表示设置主颜色）
            paint(imageArea, false);
            // 发送主颜色更新信号（更新界面显示）
            imageArea.emitPrimaryColorView();
        }
        // 右键释放：设置次颜色
        else if(event->button() == Qt::RightButton)
        {
            // 调用paint函数实际设置颜色（true表示设置次颜色）
            paint(imageArea, true);
            // 发送次颜色更新信号
            imageArea.emitSecondaryColorView();
        }

        // 重置绘制状态标志
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
    // 默认假设坐标在有效范围内
    bool inArea(true);

    // 检查坐标是否超出图像边界
    if(mStartPoint.x() < 0 || mStartPoint.y() < 0
        || mStartPoint.x() > imageArea.getImage()->width()
        || mStartPoint.y() > imageArea.getImage()->height())
    {
        inArea = false;  // 坐标无效
    }

    // 处理次颜色设置
    if(inArea && isSecondaryColor)
    {
        // 获取起始点位置的像素值
        QRgb pixel(imageArea.getImage()->pixel(mStartPoint));
        QColor getColor(pixel);

        // 更新全局次颜色（通过单例模式）
        DataSingleton::Instance()->setSecondaryColor(getColor);
    }

    // 处理主颜色设置
    if(inArea)
    {
        // 再次获取像素颜色（避免重复使用变量）
        QRgb pixel(imageArea.getImage()->pixel(mStartPoint));
        QColor getColor(pixel);

        // 更新全局主颜色
        DataSingleton::Instance()->setPrimaryColor(getColor);
    }
}

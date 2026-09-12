#include "fillinstrument.h"

// 填充工具构造函数
FillInstrument::FillInstrument(QObject *parent) :
    AbstractInstrument(parent)  // 调用基类构造函数
{

}

// 鼠标按下事件处理
void FillInstrument::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
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
void FillInstrument::mouseMoveEvent(QMouseEvent *, ImageArea &)
{
    // 填充工具在移动时不需要任何操作
}

// 鼠标释放事件处理
void FillInstrument::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
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

// 填充核心方法
void FillInstrument::paint(ImageArea &imageArea, bool isSecondaryColor, bool)
{
    QColor switchColor; // 要填充的目标颜色

    // 确定使用主色还是次色
    if(!isSecondaryColor)
        switchColor = DataSingleton::Instance()->getPrimaryColor();
    else
        switchColor = DataSingleton::Instance()->getSecondaryColor();

    // 获取点击位置的像素颜色
    QRgb pixel(imageArea.getImage()->pixel(mStartPoint));
    QColor oldColor(pixel);

    // 只有当目标颜色与原始颜色不同时才进行填充
    if(switchColor != oldColor)
    {
        // 调用递归填充算法
        fillRecurs(mStartPoint.x(), mStartPoint.y(),
                   switchColor.rgb(),   // 目标颜色（QRgb格式）
                   oldColor.rgb(),      // 原始颜色（QRgb格式）
                   *imageArea.getImage()); // 直接操作图像
    }

    // 标记图像已被修改
    imageArea.setEdited(true);

    // 更新绘图区域显示
    imageArea.update();
}

// 扫描线递归填充算法
// 算法流程：向左扫描填充直到边界，向右扫描填充直到边界，在当前扫描线的上下行中寻找新种子点
// 使用扫描线优化，减少递归深度
void FillInstrument::fillRecurs(int x, int y, QRgb switchColor, QRgb oldColor, QImage &tempImage)
{
    int temp_x(x); // 临时X坐标
    int left_x(0); // 左边界

    // 向左扫描填充
    while(true)
    {
        if(tempImage.pixel(temp_x, y) != oldColor)break;
        tempImage.setPixel(temp_x, y, switchColor);
        if(temp_x > 0){
            --temp_x;
            left_x = temp_x; // 更新左边界
        }else
            break; // 到达左边界
    }
    int right_x(0); // 右边界
    temp_x = x + 1; // 从原始位置右侧开始
    // 向右扫描填充
    while(true){
        // 检查当前像素
        if(tempImage.pixel(temp_x, y) != oldColor)break;

        // 填充当前像素
        tempImage.setPixel(temp_x, y, switchColor);

        // 继续向右移动
        if(temp_x < tempImage.width() - 1)
        {
            temp_x++;
            right_x = temp_x; // 更新右边界
        }
        else
            break; // 到达右边界
    }

    // 在当前扫描线的上下行中寻找新的种子点
    for(int x_(left_x+1); x_ < right_x; ++x_)
    {
        // 边界检查
        if(y < 1 || y >= tempImage.height() - 1)
            break;
        if(right_x > tempImage.width())
            break;

        // 检查上方像素
        QRgb currentColor = tempImage.pixel(x_, y - 1);
        if(currentColor == oldColor && currentColor != switchColor)
        {
            // 递归填充上方行
            fillRecurs(x_, y - 1, switchColor, oldColor, tempImage);
        }

        // 检查下方像素
        currentColor = tempImage.pixel(x_, y + 1);
        if(currentColor == oldColor && currentColor != switchColor)
        {
            // 递归填充下方行
            fillRecurs(x_, y + 1, switchColor, oldColor, tempImage);
        }
    }
}





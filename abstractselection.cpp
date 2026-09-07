#include "abstractselection.h"



// 2025/07/31--代码已经审核通过



#include "math.h"  // 数学函数



// 构造函数：初始化所有选择状态标志为false;
AbstractSelection::AbstractSelection(QObject *parent) :
    AbstractInstrument(parent)
{
    mIsSelectionExists = mIsSelectionMoving = mIsSelectionResizing
        = mIsPaint = mIsImageSelected = mIsMouseMoved
        = mIsSelectionAdjusting = false;
}

// 鼠标按下事件处理
void AbstractSelection::mousePressEvent(QMouseEvent *event, ImageArea &imageArea)
{
    mButton = event->button();  // 记录按下的鼠标按钮
    mIsMouseMoved = false;      // 重置鼠标移动标志

    // 如果已存在选择区域
    if (mIsSelectionExists)
    {
        // 恢复原始图像状态
        imageArea.setImage(mImageCopy);
        paint(imageArea);  // 绘制当前选择内容

        // 右键点击：开始调整操作
        if (mButton == Qt::RightButton)
        {
            mIsSelectionAdjusting = true;
            startAdjusting(imageArea);  // 调用子类调整逻辑
        }

        // 点击在选区内部
        if (event->pos().x() > mTopLeftPoint.x() &&
            event->pos().x() < mBottomRightPoint.x() &&
            event->pos().y() > mTopLeftPoint.y() &&
            event->pos().y() < mBottomRightPoint.y())
        {
            if (!mIsSelectionAdjusting)  // 非调整状态下创建撤销点
            {
                makeUndoCommand(imageArea);
            }

            if (!mIsImageSelected)  // 首次选择图像内容
            {
                startMoving(imageArea);  // 调用子类移动初始化
                if (!mIsSelectionAdjusting)
                {
                    mIsImageSelected = true;  // 标记图像已被选中
                }
            }
            else  // 已选中状态只需绘制边框
            {
                drawBorder(imageArea);
            }

            mIsSelectionMoving = true;  // 设置移动标志
            // 计算移动偏移量（鼠标位置到右下角的差值）
            mMoveDiffPoint = mBottomRightPoint - event->pos();
            return;
        }
        // 点击在右下角调整手柄区域（6x6像素）
        else if (event->pos().x() >= mBottomRightPoint.x() &&
                 event->pos().x() <= mBottomRightPoint.x() + 6 &&
                 event->pos().y() >= mBottomRightPoint.y() &&
                 event->pos().y() <= mBottomRightPoint.y() + 6)
        {
            if (!mIsSelectionAdjusting)  // 非调整状态下创建撤销点
            {
                makeUndoCommand(imageArea);
            }
            startResizing(imageArea);  // 调用子类调整大小初始化
            mIsSelectionResizing = true;  // 设置调整大小标志
            return;
        }
        else  // 点击在选区外部
        {
            clearSelection(imageArea);  // 清除当前选区
        }
    }

    // 首次创建选区（左键按下）
    if (event->button() == Qt::LeftButton)
    {
        // 初始化选区起点和终点
        mBottomRightPoint = mTopLeftPoint = event->pos();
        mHeight = mWidth = 0;  // 初始高度宽度为0

        // 保存当前图像状态副本
        mImageCopy = *imageArea.getImage();

        startSelection(imageArea);  // 调用子类选择初始化
        mIsPaint = true;  // 设置绘制标志
    }
}

// 鼠标移动事件处理
void AbstractSelection::mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea)
{
    mIsMouseMoved = true;  // 标记鼠标已移动

    // 存在选区时的操作
    if (mIsSelectionExists)
    {
        // 移动选区
        if (mIsSelectionMoving)
        {
            // 计算新右下角位置
            mBottomRightPoint = event->pos() + mMoveDiffPoint;
            // 计算新左上角位置（保持选区尺寸不变）
            mTopLeftPoint = event->pos() + mMoveDiffPoint -
                            QPoint(mWidth - 1, mHeight - 1);

            // 恢复原始图像并执行移动操作
            imageArea.setImage(mImageCopy);
            move(imageArea);        // 调用子类移动逻辑
            drawBorder(imageArea);  // 绘制新边框
            mIsPaint = false;       // 结束绘制状态
        }
        // 调整选区大小
        else if (mIsSelectionResizing)
        {
            // 更新右下角为当前鼠标位置
            mBottomRightPoint = event->pos();
            // 计算新高度（绝对值+1防止为0）
            mHeight = fabs(mTopLeftPoint.y() - mBottomRightPoint.y()) + 1;
            // 计算新宽度
            mWidth = fabs(mTopLeftPoint.x() - mBottomRightPoint.x()) + 1;

            // 恢复原始图像并执行调整操作
            imageArea.setImage(mImageCopy);
            resize(imageArea);      // 调用子类调整大小逻辑
            drawBorder(imageArea);  // 绘制新边框
            mIsPaint = false;       // 结束绘制状态
        }
    }

    // 首次创建选区（绘制中）
    if (mIsPaint)
    {
        // 更新终点为当前鼠标位置
        mBottomRightPoint = event->pos();
        // 计算高度
        mHeight = fabs(mTopLeftPoint.y() - mBottomRightPoint.y()) + 1;
        // 计算宽度
        mWidth = fabs(mTopLeftPoint.x() - mBottomRightPoint.x()) + 1;

        // 恢复原始图像
        imageArea.setImage(mImageCopy);
        drawBorder(imageArea);  // 绘制临时边框
        select(imageArea);       // 调用子类选择逻辑
    }

    // 更新鼠标光标形状
    updateCursor(event, imageArea);
}

// 鼠标释放事件处理
void AbstractSelection::mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea)
{
    // 确保左上角在左上方，右下角在右下方
    int right = mTopLeftPoint.x() > mBottomRightPoint.x() ?
                    mTopLeftPoint.x() : mBottomRightPoint.x();
    int bottom = mTopLeftPoint.y() > mBottomRightPoint.y() ?
                     mTopLeftPoint.y() : mBottomRightPoint.y();
    int left = mTopLeftPoint.x() < mBottomRightPoint.x() ?
                   mTopLeftPoint.x() : mBottomRightPoint.x();
    int top = mTopLeftPoint.y() < mBottomRightPoint.y() ?
                  mTopLeftPoint.y() : mBottomRightPoint.y();

    mBottomRightPoint = QPoint(right, bottom);
    mTopLeftPoint = QPoint(left, top);

    // 存在选区时的操作
    if (mIsSelectionExists)
    {
        updateCursor(event, imageArea);  // 更新光标

        // 右键点击且未移动：显示上下文菜单
        if (mButton == Qt::RightButton && !mIsMouseMoved)
        {
            showMenu(imageArea);     // 调用子类菜单显示
            paint(imageArea);         // 绘制选区内容
            drawBorder(imageArea);    // 绘制边框
            mIsPaint = false;         // 结束绘制
            mIsSelectionMoving = mIsImageSelected = false; // 重置状态
        }
        // 完成移动操作
        else if (mIsSelectionMoving)
        {
            imageArea.setImage(mImageCopy);  // 恢复原始图像
            completeMoving(imageArea);       // 调用子类移动完成逻辑
            paint(imageArea);                // 绘制移动后内容
            drawBorder(imageArea);           // 绘制边框
            mIsPaint = false;                // 结束绘制
            mIsSelectionMoving = false;       // 重置移动状态
        }
        // 完成调整大小操作
        else if (mIsSelectionResizing)
        {
            imageArea.setImage(mImageCopy);  // 恢复原始图像
            paint(imageArea);                // 绘制调整前内容
            completeResizing(imageArea);     // 调用子类调整完成逻辑
            paint(imageArea);                // 绘制调整后内容
            drawBorder(imageArea);           // 绘制边框
            mIsPaint = false;                // 结束绘制
            mIsSelectionResizing = false;     // 重置调整状态
        }
    }

    // 首次创建选区完成
    if (mIsPaint)
    {
        if (event->button() == Qt::LeftButton)
        {
            imageArea.setImage(mImageCopy);  // 恢复原始图像

            // 确保选区不是点
            if (mTopLeftPoint != mBottomRightPoint)
            {
                imageArea.setImage(mImageCopy);
                paint(imageArea);                 // 绘制选区内容
                completeSelection(imageArea);     // 调用子类选择完成逻辑
                paint(imageArea);                 // 最终绘制
                mIsSelectionExists = true;        // 标记选区存在
            }
            drawBorder(imageArea);  // 绘制边框
            mIsPaint = false;       // 结束绘制状态
        }
    }

    mIsSelectionAdjusting = false;  // 重置调整状态
}

// 绘制选区边框
void AbstractSelection::drawBorder(ImageArea &imageArea)
{
    // 仅当选区有效时绘制
    if (mWidth > 1 && mHeight > 1)
    {
        QPainter painter(imageArea.getImage());
        // 设置蓝色虚线边框
        painter.setPen(QPen(Qt::blue, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBackgroundMode(Qt::TransparentMode);  // 透明背景

        // 非单点选区
        if(mTopLeftPoint != mBottomRightPoint)
        {
            // 绘制矩形（减1像素避免重叠）
            painter.drawRect(QRect(mTopLeftPoint, mBottomRightPoint - QPoint(1, 1)));
        }

        imageArea.setEdited(true);  // 标记图像已编辑
        painter.end();              // 结束绘制
        imageArea.update();         // 更新显示
    }
}

// 清除选区
void AbstractSelection::clearSelection(ImageArea &imageArea)
{
    if (mIsSelectionExists)
    {
        // 恢复原始图像
        imageArea.setImage(mImageCopy);
        paint(imageArea);  // 绘制清除后内容

        // 保存新状态为副本
        mImageCopy = *imageArea.getImage();

        // 重置所有状态标志
        mIsSelectionExists = mIsSelectionMoving = mIsSelectionResizing
            = mIsPaint = mIsImageSelected = false;

        imageArea.update();      // 更新显示
        imageArea.restoreCursor();  // 恢复默认光标
        clear();                // 调用子类清除逻辑
    }
}

// 保存图像变更，暂留接口
void AbstractSelection::saveImageChanges(ImageArea &)
{

}

// 更新鼠标光标形状
void AbstractSelection::updateCursor(QMouseEvent *event, ImageArea &imageArea)
{
    if (mIsSelectionExists)
    {
        // 在选区内：显示移动光标
        if (event->pos().x() > mTopLeftPoint.x() &&
            event->pos().x() < mBottomRightPoint.x() &&
            event->pos().y() > mTopLeftPoint.y() &&
            event->pos().y() < mBottomRightPoint.y())
        {
            imageArea.setCursor(Qt::SizeAllCursor);
        }
        // 在调整手柄区域：显示对角线调整光标
        else if (event->pos().x() >= mBottomRightPoint.x() &&
                 event->pos().x() <= mBottomRightPoint.x() + 6 &&
                 event->pos().y() >= mBottomRightPoint.y() &&
                 event->pos().y() <= mBottomRightPoint.y() + 6)
        {
            imageArea.setCursor(Qt::SizeFDiagCursor);
        }
        // 其他区域：恢复默认光标
        else
        {
            imageArea.restoreCursor();
        }
    }
    // 无选区时恢复默认光标
    else
    {
        imageArea.restoreCursor();
    }
}

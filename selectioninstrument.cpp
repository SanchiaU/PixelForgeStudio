#include "selectioninstrument.h"

// 构造函数
// 构造函数
SelectionInstrument::SelectionInstrument(QObject *parent) :
    AbstractSelection(parent)  // 调用基类构造函数
{
}

// 复制选区图像到剪贴板
void SelectionInstrument::copyImage(ImageArea &imageArea)
{
    if (mIsSelectionExists)  // 确保选区存在
    {
        imageArea.setImage(mImageCopy);  // 恢复原始图像

        QClipboard *globalClipboard = QApplication::clipboard();  // 获取系统剪贴板
        QImage copyImage;

        if(mIsImageSelected)  // 如果已有选中图像
        {
            copyImage = mSelectedImage;  // 直接使用缓存的选中图像
        }
        else  // 没有缓存图像时
        {
            // 从图像区域截取选区
            copyImage = imageArea.getImage()->copy(
                mTopLeftPoint.x(), mTopLeftPoint.y(),
                mWidth, mHeight);
        }

        globalClipboard->setImage(copyImage, QClipboard::Clipboard);  // 设置到剪贴板
    }
}

// 剪切选区图像到剪贴板
void SelectionInstrument::cutImage(ImageArea &imageArea)
{
    if (mIsSelectionExists)  // 确保选区存在
    {
        copyImage(imageArea);  // 先执行复制操作

        if(mIsSelectionExists)  // 检查选区是否仍存在
        {
            imageArea.setImage(mImageCopy);  // 恢复原始图像
            paint(imageArea);  // 绘制选区
        }

        makeUndoCommand(imageArea);  // 创建撤销命令

        // 判断选区类型
        if (mIsImageSelected)
        {
            imageArea.setImage(mImageCopy);  // 恢复原始图像
        }
        else
        {
            clearSelectionBackground(imageArea);  // 清除选区背景
        }

        // 重置选区参数
        mTopLeftPoint = QPoint(0, 0);
        mBottomRightPoint = QPoint(0, 0);
        mImageCopy = *imageArea.getImage();  // 更新图像副本
        imageArea.update();  // 更新显示

        mIsSelectionExists = false;  // 标记选区不存在
        imageArea.restoreCursor();  // 恢复光标
        emit sendEnableCopyCutActions(false);  // 通知禁用复制/剪切操作
    }
}
// 从剪贴板粘贴图像
void SelectionInstrument::pasteImage(ImageArea &imageArea)
{
    QClipboard *globalClipboard = QApplication::clipboard();  // 获取系统剪贴板

    // 如果已有选区，先恢复原始图像并绘制选区
    if(mIsSelectionExists)
    {
        imageArea.setImage(mImageCopy);
        paint(imageArea);
        mImageCopy = *imageArea.getImage();  // 更新图像副本
    }

    makeUndoCommand(imageArea);  // 创建撤销命令

    mPasteImage = globalClipboard->image();  // 获取剪贴板图像
    if (!mPasteImage.isNull())  // 检查剪贴板是否有有效图像
    {
        mSelectedImage = mPasteImage;  // 存储为选中图像
        mImageCopy = *imageArea.getImage();  // 备份当前图像

        // 设置选区参数（从(0,0)开始，大小为粘贴图像尺寸）
        mTopLeftPoint = QPoint(0, 0);
        mBottomRightPoint = QPoint(mPasteImage.width(), mPasteImage.height()) - QPoint(1, 1);
        mHeight = mPasteImage.height();
        mWidth = mPasteImage.width();

        // 设置选区状态标志
        mIsImageSelected = mIsSelectionExists = true;

        paint(imageArea);  // 绘制粘贴的图像
        drawBorder(imageArea);  // 绘制选区边框
        imageArea.restoreCursor();  // 恢复光标
        emit sendEnableCopyCutActions(true);  // 通知启用复制/剪切操作
    }
}

// 开始调整选区（准备阶段）
void SelectionInstrument::startAdjusting(ImageArea &imageArea)
{
    mImageCopy = *imageArea.getImage();  // 备份当前图像
    mIsImageSelected = false;  // 标记为未选择图像
}

// 开始创建选区（空实现，具体在鼠标事件中处理）
void SelectionInstrument::startSelection(ImageArea &)
{
}

// 开始调整选区大小
void SelectionInstrument::startResizing(ImageArea &imageArea)
{
    if (!mIsImageSelected)  // 如果没有选中图像
    {
        clearSelectionBackground(imageArea);  // 清除选区背景
    }
    if (mIsSelectionAdjusting)  // 如果正在调整选区
    {
        mIsImageSelected = false;  // 重置图像选择标志
    }
}

// 开始移动选区
void SelectionInstrument::startMoving(ImageArea &imageArea)
{
    clearSelectionBackground(imageArea);  // 清除选区背景
    if (mIsSelectionAdjusting)  // 如果正在调整选区
    {
        mIsImageSelected = false;  // 重置图像选择标志
    }
}

// 选区过程（空实现，具体在鼠标移动事件中处理）
void SelectionInstrument::select(ImageArea &)
{
}

// 调整大小过程（空实现）
void SelectionInstrument::resize(ImageArea &)
{
}

// 移动过程（空实现）
void SelectionInstrument::move(ImageArea &)
{
}

// 完成选区创建
void SelectionInstrument::completeSelection(ImageArea &imageArea)
{
    // 截取选区图像
    mSelectedImage = imageArea.getImage()->copy(
        mTopLeftPoint.x(), mTopLeftPoint.y(),
        mWidth, mHeight);

    emit sendEnableCopyCutActions(true);  // 通知启用复制/剪切操作
}

// 完成大小调整
void SelectionInstrument::completeResizing(ImageArea &imageArea)
{
    // 重新截取调整后的选区图像
    mSelectedImage = imageArea.getImage()->copy(
        mTopLeftPoint.x(), mTopLeftPoint.y(),
        mWidth, mHeight);
}

// 完成移动操作
void SelectionInstrument::completeMoving(ImageArea &imageArea)
{
    if (mIsSelectionAdjusting)  // 如果正在调整选区
    {
        // 重新截取移动后的选区图像
        mSelectedImage = imageArea.getImage()->copy(
            mTopLeftPoint.x(), mTopLeftPoint.y(),
            mWidth, mHeight);
    }
}

// 清除选区背景
void SelectionInstrument::clearSelectionBackground(ImageArea &imageArea)
{
    if (!mIsSelectionAdjusting)  // 确保不在调整过程中
    {
        QPainter blankPainter(imageArea.getImage());  // 在图像上绘制
        blankPainter.setPen(Qt::white);  // 设置白色画笔
        blankPainter.setBrush(QBrush(Qt::white));  // 设置白色画刷
        blankPainter.setBackgroundMode(Qt::OpaqueMode);  // 设置不透明模式

        // 绘制白色矩形覆盖选区
        blankPainter.drawRect(QRect(mTopLeftPoint,
                                    mBottomRightPoint - QPoint(1, 1)));
        blankPainter.end();

        mImageCopy = *imageArea.getImage();  // 更新图像副本
    }
}
// 清除选区
void SelectionInstrument::clear()
{
    mSelectedImage = QImage();  // 清空选中的图像
    emit sendEnableCopyCutActions(false);  // 通知禁用复制/剪切操作
}

// 绘制选区内容
void SelectionInstrument::paint(ImageArea &imageArea, bool, bool)
{
    if (mIsSelectionExists && !mIsSelectionAdjusting)  // 确保选区存在且不在调整中
    {
        if(mTopLeftPoint != mBottomRightPoint)  // 确保选区有效
        {
            QPainter painter(imageArea.getImage());  // 在图像上绘制

            // 设置源区域和目标区域
            QRect source(0, 0, mSelectedImage.width(), mSelectedImage.height());
            QRect target(mTopLeftPoint, mBottomRightPoint);

            painter.drawImage(target, mSelectedImage, source);  // 绘制图像
            painter.end();
        }

        imageArea.setEdited(true);  // 标记图像已被编辑
        imageArea.update();  // 更新显示
    }
}

// 显示上下文菜单（空实现）
void SelectionInstrument::showMenu(ImageArea &imageArea)
{
    // 可以在这里添加选区右键菜单的实现
    // 例如：复制、剪切、删除选区等选项
    Q_UNUSED(imageArea);  // 避免未使用参数警告
}


#include "undocommand.h"
#include "imagearea.h"


// 构造函数：初始化撤销命令
UndoCommand::UndoCommand(const QImage *img, ImageArea &imgArea, QUndoCommand *parent)
    : QUndoCommand(parent),      // 初始化基类
    mPrevImage(*img),          // 保存当前状态作为"前状态"（深拷贝）
    mImageArea(imgArea)        // 初始化图像区域引用
{
    mCurrImage = mPrevImage;     // 复制为当前状态（此处逻辑需确认）
}


// 撤销操作：恢复到操作前的图像状态
void UndoCommand::undo()
{
    mImageArea.clearSelection();
    mCurrImage = *(mImageArea.getImage());
    mImageArea.setImage(mPrevImage);
    mImageArea.update();
    mImageArea.saveImageChanges();
}

// 重做操作：恢复到操作后的图像状态
void UndoCommand::redo()
{
    mImageArea.setImage(mCurrImage);
    mImageArea.update();
    mImageArea.saveImageChanges();
}

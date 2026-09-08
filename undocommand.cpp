#include "undocommand.h"
#include "imagearea.h"


// 2025/07/28--代码已经审核通过



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
    // 1. 清除可能存在的选区
    mImageArea.clearSelection();

    // 2. 保存当前状态（用于重做时恢复）
    mCurrImage = *(mImageArea.getImage());

    // 3. 恢复到命令执行前的状态
    mImageArea.setImage(mPrevImage);

    // 4. 更新UI显示
    mImageArea.update();

    // 5. 保存图像变更状态
    mImageArea.saveImageChanges();
}

// 重做操作：恢复到操作后的图像状态
void UndoCommand::redo()
{
    // 1. 设置图像为命令执行后的状态
    mImageArea.setImage(mCurrImage);

    // 2. 更新UI显示
    mImageArea.update();

    // 3. 保存图像变更状态
    mImageArea.saveImageChanges();
}

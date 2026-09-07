#ifndef UNDOCOMMAND_H
#define UNDOCOMMAND_H



// 2025/07/28--代码已经审核通过



#include <QObject>

#include <QUndoCommand>  // 引入Qt的撤销命令基类
#include <QImage>        // 引入Qt图像处理类

#include "imagearea.h"


// 提供撤销、重做操作的命令类
// 此类用于封装图像编辑操作的撤销/重做功能。保存操作前后的图像状态，
// 并在撤销/重做时恢复对应状态
class UndoCommand : public QUndoCommand
{
    //  Q_OBJECT        // 千万要记住，要取注释掉此代码，不需要  启用Qt元对象系统（信号槽机制）
public:
    // 构造函数
    UndoCommand(const QImage* img, ImageArea &imgArea, QUndoCommand *parent = 0);

    // 撤销操作：恢复到操作前的图像状态
    virtual void undo() override;

    // 重做操作：恢复到操作后的图像状态
    virtual void redo() override;

private:
    QImage mPrevImage;          // 保存操作前的图像状态（撤销时使用）
    QImage mCurrImage;          // 保存操作后的图像状态（重做时使用）
    ImageArea& mImageArea;      // 关联的图像区域对象引用（用于实际图像更新）

};

#endif // UNDOCOMMAND_H

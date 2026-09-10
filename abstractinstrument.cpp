#include "abstractinstrument.h"     // 抽象工具基类声明
#include "imagearea.h"           // 图像区域操作类
#include "undocommand.h"         // 撤销命令实现

// 抽象绘图工具基类的构造函数
// parent父对象指针（遵循Qt对象树管理）
AbstractInstrument::AbstractInstrument(QObject *parent) :
    QObject(parent)  // 初始化基类QObject
{}

void AbstractInstrument::makeUndoCommand(ImageArea &imageArea)
{
    // 创建新的撤销命令
    UndoCommand *cmd = new UndoCommand(imageArea.getImage(), imageArea);

    // 将命令压入撤销栈（ImageArea管理栈对象）
    imageArea.pushUndoCommand(cmd);
    // imageArea.pushUndoCommand(new UndoCommand(imageArea.getImage(), imageArea));
}

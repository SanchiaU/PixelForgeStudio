#include "abstractinstrument.h"     // 抽象工具基类声明
#include "imagearea.h"           // 图像区域操作类
#include "undocommand.h"         // 撤销命令实现



// 2025/07/29--代码已经审核通过



// 抽象绘图工具基类的构造函数
// parent父对象指针（遵循Qt对象树管理）
AbstractInstrument::AbstractInstrument(QObject *parent) :
    QObject(parent)  // 初始化基类QObject
{
    // 构造函数体为空
    // 具体工具的初始化逻辑在派生类中实现
}

// 创建并提交撤销命令（默认实现）
// 此方法将当前图像状态保存到撤销栈中
// 采用命令模式实现撤销/重做功能

// 工程流程：
// 1.获取图像区域的当前图像
// 2.创建UndoCommand对象（封装图像状态）
// 3.将命令压入图像区域的撤销栈
void AbstractInstrument::makeUndoCommand(ImageArea &imageArea)
{
    // 创建新的撤销命令
    UndoCommand *cmd = new UndoCommand(imageArea.getImage(), imageArea);

    // 将命令压入撤销栈（ImageArea管理栈对象）
    imageArea.pushUndoCommand(cmd);


    // 上面两行可以一次性解决
    // imageArea.pushUndoCommand(new UndoCommand(imageArea.getImage(), imageArea));
}

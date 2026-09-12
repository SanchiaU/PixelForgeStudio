#ifndef SELECTIONINSTRUMENT_H
#define SELECTIONINSTRUMENT_H

#include "abstractselection.h"
#include "imagearea.h"
#include "undocommand.h"
#include "math.h"

#include <QPainter>
#include <QApplication>
#include <QClipboard>
#include <QObject>


QT_BEGIN_NAMESPACE
class QUndoStack;  // 前向声明Qt的撤销栈类
QT_END_NAMESPACE

// 实现图像选择工具类（复制/剪切/粘贴/移动等）
// 继承自AbstractSelection，提供完整的图像选区操作功能
// 包括选区管理、剪贴板操作和选区变换功能

class SelectionInstrument : public AbstractSelection
{
    Q_OBJECT  // 启用Qt元对象系统

public:
    explicit SelectionInstrument(QObject *parent = 0);  // 构造函数
    // 选区背景操作
    void clearSelectionBackground(ImageArea &imageArea);
    // 剪贴板操作
    void copyImage(ImageArea &imageArea);
    void pasteImage(ImageArea &imageArea);
    void cutImage(ImageArea &imageArea);
private:
    // 选区生命周期管理
    void startAdjusting(ImageArea &imageArea);  // 开始调整选区
    void startSelection(ImageArea &);           // 开始创建选区
    void startResizing(ImageArea &imageArea);   // 开始调整大小
    void startMoving(ImageArea &imageArea);     // 开始移动选区
    // 实时操作处理
    void select(ImageArea &);    // 处理选区绘制
    void resize(ImageArea &);    // 处理大小调整
    void move(ImageArea &);      // 处理移动操作
    // 操作完成处理
    void completeSelection(ImageArea &imageArea);  // 完成选区创建
    void completeResizing(ImageArea &imageArea);   // 完成大小调整
    void completeMoving(ImageArea &imageArea);     // 完成移动操作
    // 辅助功能
    void clear();                // 清除当前选区
    void paint(ImageArea &imageArea, bool = false, bool = false);  // 绘制选区
    void showMenu(ImageArea &imageArea);  // 显示上下文菜单
    // 图像缓存
    QImage mSelectedImage;  // 存储当前选中的图像区域副本（用于移动/调整操作）
    QImage mPasteImage;     // 存储从剪贴板粘贴的图像
signals:
    // UI状态更新信号
    void sendEnableCopyCutActions(bool enable);       // 启用/禁用复制剪切操作
    void sendEnableSelectionInstrument(bool enable);  // 启用/禁用选择工具

};

#endif // SELECTIONINSTRUMENT_H











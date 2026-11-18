#ifndef ABSTRACTSELECTION_H
#define ABSTRACTSELECTION_H


#include <QObject>
#include <QPainter>
#include "abstractinstrument.h"
#include "imagearea.h"          // 图像区域操作类
#include "undocommand.h"        // 撤销命令支持

QT_BEGIN_NAMESPACE
class QUndoStack;   // 用于撤销/重做操作的栈
class ImageArea;    // 图像显示区域类
QT_END_NAMESPACE

class AbstractSelection : public AbstractInstrument
{
    Q_OBJECT
public:
    explicit AbstractSelection(QObject *parent = 0);  // 构造函数
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea);
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea);
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea);


    void clearSelection(ImageArea &imageArea);

    void saveImageChanges(ImageArea &);

    virtual void startSelection(ImageArea &imageArea) = 0;    ///< 初始化选择操作
    virtual void startResizing(ImageArea &imageArea) = 0;     ///< 开始调整选择区域大小
    virtual void startMoving(ImageArea &imageArea) = 0;       ///< 开始移动选择区域
    virtual void startAdjusting(ImageArea &imageArea) = 0;    ///< 开始调整选择内容
    virtual void select(ImageArea &imageArea) = 0;            ///< 执行选择过程
    virtual void resize(ImageArea &imageArea) = 0;            ///< 执行大小调整
    virtual void move(ImageArea &imageArea) = 0;              ///< 执行移动操作
    virtual void completeSelection(ImageArea &imageArea) = 0; ///< 完成选择操作
    virtual void completeResizing(ImageArea &imageArea) = 0;  ///< 完成大小调整
    virtual void completeMoving(ImageArea &imageArea) = 0;    ///< 完成移动操作
    virtual void clear() = 0;                                 ///< 清除内部状态
    virtual void showMenu(ImageArea &imageArea) = 0;          ///< 显示上下文菜单

protected:
    void drawBorder(ImageArea &imageArea);

    // 根据鼠标位置更新光标形状
    void updateCursor(QMouseEvent *event, ImageArea &imageArea);

    QPoint mBottomRightPoint; ///< 选择区域右下角坐标
    QPoint mTopLeftPoint;     ///< 选择区域左上角坐标
    QPoint mMoveDiffPoint;    ///< 移动操作的偏移量

    bool mIsPaint;               ///< 是否正在绘制
    bool mIsSelectionExists;     ///< 是否存在有效选择区域
    bool mIsSelectionMoving;     ///< 是否正在移动选择区域
    bool mIsSelectionResizing;   ///< 是否正在调整选择区域大小
    bool mIsImageSelected;       ///< 是否有图像内容被选中
    bool mIsMouseMoved;          ///< 鼠标是否发生移动
    bool mIsSelectionAdjusting;  ///< 是否正在调整选择内容

    int mHeight; ///< 选择区域高度
    int mWidth;  ///< 选择区域宽度

    Qt::MouseButton mButton; ///< 当前操作的鼠标按钮


};

#endif // ABSTRACTSELECTION_H

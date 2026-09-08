#ifndef TEXTINSTRUMENT_H
#define TEXTINSTRUMENT_H



// 2025/08/09--代码已经审核通过



#include "abstractselection.h"
#include <QtCore/QObject>

#include "imagearea.h"      // 图像区域管理类
#include "datasingleton.h"  // 单例数据存储（颜色、字体等）
#include "undocommand.h"    // 撤销命令支持
#include "textdialog.h"     // 文本输入对话框

#include <QPainter>         // Qt绘图工具



// 文本工具类
// 继承自AbstractSelection类，实现在图像区域上添加/编辑文本的功能
// 支持文本选择、移动、调整大小等操作
class TextInstrument : public AbstractSelection
{
    Q_OBJECT  // Qt宏，启用信号槽机制和元对象系统

public:
    // 构造函数
    explicit TextInstrument(QObject *parent=0);


private:
    // ======== 重写基类抽象方法 ========
    // 开始调整操作（基类要求实现）
    void startAdjusting(ImageArea &);
    // 开始选择操作（初始化选择区域）
    void startSelection(ImageArea &);
    // 开始调整文本区域大小
    void startResizing(ImageArea &);
    // 开始移动文本区域
    void startMoving(ImageArea &);
    // 处理选择过程中的绘制
    void select(ImageArea &);
    // 处理大小调整逻辑
    void resize(ImageArea &imageArea);
    // 处理移动逻辑
    void move(ImageArea &imageArea);
    // 完成选择操作（确认文本位置）
    void completeSelection(ImageArea &imageArea);
    // 完成大小调整操作
    void completeResizing(ImageArea &);
    // 完成移动操作
    void completeMoving(ImageArea &);
    // 清除当前文本选择状态
    void clear();

    // 在图像区域绘制文本和选择框
    void paint(ImageArea &imageArea, bool = false, bool = false);

    // 显示文本编辑的右键菜单
    void showMenu(ImageArea &imageArea);

    // 成员变量
    QString mText;      // 存储当前编辑的文本内容
    bool mIsEdited;     // 标记文本是否被编辑过

signals:
    // 信号：请求关闭文本对话框，当文本操作完成或取消触发
    void sendCloseTextDialog();

private slots:
    // 更新文本内容
    void updateText(ImageArea *, QString);

    // 取消文本操作
    void cancel(ImageArea *);

};

#endif // TEXTINSTRUMENT_H

#ifndef PALETTEBAR_H
#define PALETTEBAR_H

#include <QObject>
#include <QToolBar>         // Qt工具栏基类
#include "toolbar.h"        // 自定义工具栏基类
#include "palettebutton.h"  // 自定义颜色按钮类
#include "datasingleton.h"  // 全局数据单例

// 调色板工具栏
// 包含多个不同颜色的按钮，用于快速选择颜色
class PaletteBar : public QToolBar
{
    Q_OBJECT
public:
    // 构造函数
    PaletteBar(ToolBar *toolBar);

private:
    // 初始化工具栏项，创建并配置颜色按钮
    void initializeItems();
    PaletteButton *mColorButton;      // 颜色按钮实例（实际实现可能有多个）
    ToolBar *mToolBar;                // 关联的父工具栏
    bool mIsRightButtonClicked;       // 右键点击状态标志

private slots:
    void colorClicked();

};

#endif // PALETTEBAR_H

#include "palettebar.h"


// 2025/07/22--代码已经审核通过


// 构造函数：创建调色板工具栏，toolbar参数是父工具栏指针
PaletteBar::PaletteBar(ToolBar *toolbar) :
    QToolBar(tr("Colors"))  // 设置工具栏标题为"Colors"
{
    mToolBar = toolbar;    // 保存父工具栏引用
    setMovable(false);     // 禁止工具栏拖动
    initializeItems();     // 初始化工具栏内容
}


// 初始化工具栏项，创建并配置颜色按钮
void PaletteBar::initializeItems()
{
    // 创建黑色按钮
    mColorButton = new PaletteButton(Qt::black);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建白色按钮
    mColorButton = new PaletteButton(Qt::white);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建红色按钮
    mColorButton = new PaletteButton(Qt::red);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建深红色按钮
    mColorButton = new PaletteButton(Qt::darkRed);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建绿色按钮
    mColorButton = new PaletteButton(Qt::green);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建深绿色按钮
    mColorButton = new PaletteButton(Qt::darkGreen);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建蓝色按钮
    mColorButton = new PaletteButton(Qt::blue);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建深蓝色按钮
    mColorButton = new PaletteButton(Qt::darkBlue);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建青色按钮
    mColorButton = new PaletteButton(Qt::cyan);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建深青色按钮
    mColorButton = new PaletteButton(Qt::darkCyan);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建洋红色按钮
    mColorButton = new PaletteButton(Qt::magenta);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建深洋红色按钮
    mColorButton = new PaletteButton(Qt::darkMagenta);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建黄色按钮
    mColorButton = new PaletteButton(Qt::yellow);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建深黄色按钮
    mColorButton = new PaletteButton(Qt::darkYellow);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

    // 创建灰色按钮
    mColorButton = new PaletteButton(Qt::gray);
    connect(mColorButton, SIGNAL(colorPicked()), this, SLOT(colorClicked()));
    addWidget(mColorButton);

}

// 颜色按钮点击槽函数，通知父工具栏更新颜色显示状态
void PaletteBar::colorClicked()
{
    // 更新父工具栏的主色和辅色显示
    mToolBar->setPrimaryColorView();
    mToolBar->setSecondaryColorView();
}

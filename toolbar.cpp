#include "toolbar.h"

// 构造函数
ToolBar::ToolBar(const QMap<InstrumentsEnum, QAction *> &actMap, QWidget *parent) :
    QToolBar(tr("Instruments"), parent), // 初始化工具栏并设置标题
    mActMap(actMap)                      // 初始化工具动作映射
{
    setMovable(false);        // 禁止用户拖动工具栏
    initializeItems();        // 初始化工具栏内容
    mPrevInstrumentSetted = false; // 初始化上一个工具状态标志
}

// 创建工具按钮
QToolButton* ToolBar::createToolButton(QAction *act)
{
    QToolButton *toolButton = new QToolButton();
    toolButton->setMinimumSize(QSize(30, 30));  // 设置按钮最小尺寸
    toolButton->setMaximumSize(QSize(30, 30));  // 设置按钮最大尺寸（固定大小）
    toolButton->setDefaultAction(act);          // 将动作与按钮关联
    toolButton->setStatusTip(act->text());      // 设置状态栏提示文本

    return toolButton;
}

// 初始化工具栏内容
void ToolBar::initializeItems()
{
    // 创建所有工具按钮=========
    // 通过动作映射创建对应的工具按钮
    mCursorButton = createToolButton(mActMap[CURSOR]);
    mEraserButton = createToolButton(mActMap[ERASER]);
    mPenButton = createToolButton(mActMap[PEN]);
    mLineButton = createToolButton(mActMap[LINE]);
    mColorPickerButton = createToolButton(mActMap[COLORPICKER]);
    mMagnifierButton = createToolButton(mActMap[MAGNIFIER]);
    mSprayButton = createToolButton(mActMap[SPRAY]);
    mFillButton = createToolButton(mActMap[FILL]);
    mRectangleButton = createToolButton(mActMap[RECTANGLE]);
    mEllipseButton = createToolButton(mActMap[ELLIPSE]);
    mCurveButton = createToolButton(mActMap[CURVELINE]);
    mTextButton = createToolButton(mActMap[TEXT]);

    // 创建工具按钮布局======================
    QGridLayout *bLayout = new QGridLayout();
    bLayout->setMargin(3);  // 设置布局边距

    // 将工具按钮添加到网格布局中（2列布局）
    bLayout->addWidget(mCursorButton, 0, 0);
    bLayout->addWidget(mEraserButton, 0, 1);
    bLayout->addWidget(mColorPickerButton, 1, 0);
    bLayout->addWidget(mMagnifierButton, 1, 1);
    bLayout->addWidget(mPenButton, 2, 0);
    bLayout->addWidget(mLineButton, 2, 1);
    bLayout->addWidget(mSprayButton, 3, 0);
    bLayout->addWidget(mFillButton, 3, 1);
    bLayout->addWidget(mRectangleButton, 4, 0);
    bLayout->addWidget(mEllipseButton, 4, 1);
    bLayout->addWidget(mCurveButton, 5, 0);
    bLayout->addWidget(mTextButton, 5, 1);

    // 创建工具按钮容器部件
    QWidget *bWidget = new QWidget();
    bWidget->setLayout(bLayout);

    // 创建颜色选择器 =====================================================
    // 主色选择器（初始为黑色）
    mPColorChooser = new ColorChooser(0, 0, 0, this);
    mPColorChooser->setStatusTip(tr("Primary color"));  // 状态栏提示
    mPColorChooser->setToolTip(tr("Primary color"));    // 悬停提示
    // 连接颜色改变信号到槽
    connect(mPColorChooser, SIGNAL(sendColor(QColor)),
            this, SLOT(primaryColorChanged(QColor)));

    // 辅色选择器（初始为白色）
    mSColorChooser = new ColorChooser(255, 255, 255, this);
    mSColorChooser->setStatusTip(tr("Secondary color"));
    mSColorChooser->setToolTip(tr("Secondary color"));
    connect(mSColorChooser, SIGNAL(sendColor(QColor)),
            this, SLOT(secondaryColorChanged(QColor)));


    // 创建画笔大小设置控件 =============================================
    QSpinBox *penSizeSpin = new QSpinBox();
    penSizeSpin->setRange(1, 20);       // 设置范围1-20像素
    penSizeSpin->setValue(1);           // 默认值1像素
    penSizeSpin->setStatusTip(tr("Pen size"));
    penSizeSpin->setToolTip(tr("Pen size"));
    // 连接值改变信号到槽
    connect(penSizeSpin, SIGNAL(valueChanged(int)),
            this, SLOT(penValueChanged(int)));

    // 创建颜色和画笔设置布局 ===========================================
    QGridLayout *tLayout = new QGridLayout();
    tLayout->setMargin(3);  // 设置布局边距
    tLayout->addWidget(mPColorChooser, 0, 0);     // 主色选择器
    tLayout->addWidget(mSColorChooser, 0, 1);     // 辅色选择器
    tLayout->addWidget(penSizeSpin, 1, 0, 1, 2);  // 画笔大小设置（跨两列）

    // 创建设置区域容器部件
    QWidget *tWidget = new QWidget();
    tWidget->setLayout(tLayout);

    // 将组件添加到工具栏 =================================
    addWidget(bWidget);    // 添加工具按钮区域
    addSeparator();        // 添加分隔线
    addWidget(tWidget);    // 添加颜色和画笔设置区域
}

// 处理画笔大小改变事件
void ToolBar::penValueChanged(const int &value)
{
    // 更新全局单例中的画笔大小
    DataSingleton::Instance()->setPenSize(value);
}

// 处理主色改变事件
void ToolBar::primaryColorChanged(const QColor &color)
{
    // 更新全局单例中的主色
    DataSingleton::Instance()->setPrimaryColor(color);
}

// 处理辅色改变事件
void ToolBar::secondaryColorChanged(const QColor &color)
{
    // 更新全局单例中的辅色
    DataSingleton::Instance()->setSecondaryColor(color);
}

// 更新主色显示（从全局数据同步）
void ToolBar::setPrimaryColorView()
{
    // 从单例获取主色并更新选择器显示
    mPColorChooser->setColor(DataSingleton::Instance()->getPrimaryColor());
}

// 更新辅色显示（从全局数据同步）
void ToolBar::setSecondaryColorView()
{
    // 从单例获取辅色并更新选择器显示
    mSColorChooser->setColor(DataSingleton::Instance()->getSecondaryColor());
}



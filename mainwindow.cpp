#include "mainwindow.h"

// 2025/08/16--代码已经审核通过
MainWindow::MainWindow(QStringList filePaths, QWidget *parent)
    : QMainWindow(parent), mPrevInstrumentSetted(false) // 初始化父类和标志位
{
    // 从单例获取保存的窗口尺寸
    QSize winSize = DataSingleton::Instance()->getWindowSize();
    // 如果设置了恢复窗口尺寸且尺寸有效，则调整窗口大小
    if (DataSingleton::Instance()->getIsRestoreWindowSize() && winSize.isValid()) {
        resize(winSize);
    }

    // 创建撤销/重做组
    mUndoStackGroup = new QUndoGroup(this);

    // 初始化界面组件
    initializeMainMenu();      // 主菜单
    initializeToolBar();       // 工具栏
    initializePaletteBar();    // 调色板栏
    initializeStatusBar();     // 状态栏
    initializeTabWidget();     // 标签页控件

    // 根据传入的文件路径初始化标签页
    if(filePaths.isEmpty()) {
        initializeNewTab();    // 无路径时创建新标签页
    } else {
        for(int i = 0; i < filePaths.size(); i++) {
            initializeNewTab(true, filePaths.at(i)); // 打开每个文件
        }
    }

    // 注册InstrumentsEnum为元类型，用于信号槽传递
    qRegisterMetaType<InstrumentsEnum>("InstrumentsEnum");
    // 标记应用已完成初始化
    DataSingleton::Instance()->setIsInitialized();
}


MainWindow::~MainWindow()
{

}

// 2025/08/26--代码已经审核通过
// 初始化标签页组件
void MainWindow::initializeTabWidget()
{
    // 标签页功能可能通过其他方式实现（如直接在主窗口中管理）
    mTabWidget = new QTabWidget();
    mTabWidget->setUsesScrollButtons(true);  // 启用滚动按钮：标签过多时可滚动
    mTabWidget->setTabsClosable(true);       // 标签可关闭：显示关闭按钮
    mTabWidget->setMovable(true);            // 标签可拖动：支持重新排列

    // 连接标签页变化信号到相应的处理槽函数
    connect(mTabWidget, SIGNAL(currentChanged(int)), this, SLOT(activateTab(int)));      // 激活指定标签
    connect(mTabWidget, SIGNAL(currentChanged(int)), this, SLOT(enableActions(int)));    // 启用/禁用菜单
    connect(mTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));      // 关闭指定标签
    setCentralWidget(mTabWidget);  // 将标签页设置为主窗口的中央控件

}

// 2025/08/26--代码已经审核通过
void MainWindow::initializeNewTab(const bool &isOpen, const QString &filePath)
{
    ImageArea *imageArea;  // 图像编辑区域对象指针
    QString fileName(tr("Untitled Image"));  // 默认文件名，用于新建文件

    // ===== 根据不同的参数组合创建ImageArea对象 =====
    if(isOpen && filePath.isEmpty()) {
        // 情况1：打开文件但未指定路径 -> 显示文件选择对话框
        imageArea = new ImageArea(isOpen, "", this);
        fileName = imageArea->getFileName();  // 获取用户选择的文件名
    } else if(isOpen && !filePath.isEmpty()) {
        // 情况2：直接打开指定路径的文件 -> 无需对话框
        imageArea = new ImageArea(isOpen, filePath, this);
        fileName = imageArea->getFileName();  // 获取文件名（从路径提取）
    } else {
        // 情况3：创建新的空白图像 -> 使用默认文件名
        imageArea = new ImageArea(false, "", this);
    }

    // ===== 检查图像创建/加载是否成功 =====
    if (!imageArea->getFileName().isNull()) {
        // 文件加载成功，继续创建标签页

        // ===== 创建滚动区域容器 =====
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setAttribute(Qt::WA_DeleteOnClose);  // 设置关闭时自动销毁属性
        scrollArea->setBackgroundRole(QPalette::Dark);   // 设置深色背景，与绘图区域对比
        scrollArea->setWidget(imageArea);                // 将图像编辑区域嵌入滚动容器

        // ===== 添加到标签页组件 =====
        mTabWidget->addTab(scrollArea, fileName);                 // 添加新标签页
        mTabWidget->setCurrentIndex(mTabWidget->count()-1);       // 激活新创建的标签页

        // ===== 集成撤销/重做系统 =====
        mUndoStackGroup->addStack(imageArea->getUndoStack());  // 将该图像的撤销栈添加到组中

        // ===== 建立信号槽连接：图像区域 -> 主窗口/工具栏 =====
        // 颜色更新信号：同步颜色选择器的显示
        connect(imageArea, SIGNAL(sendPrimaryColorView()), mToolbar, SLOT(setPrimaryColorView()));
        connect(imageArea, SIGNAL(sendSecondaryColorView()), mToolbar, SLOT(setSecondaryColorView()));

        // 工具切换信号：处理工具间的自动切换
        connect(imageArea, SIGNAL(sendRestorePreviousInstrument()), this, SLOT(restorePreviousInstrument()));
        connect(imageArea, SIGNAL(sendSetInstrument(InstrumentsEnum)), this, SLOT(setInstrument(InstrumentsEnum)));

        // 状态信息更新信号：同步状态栏显示
        connect(imageArea, SIGNAL(sendNewImageSize(QSize)), this, SLOT(setNewSizeToSizeLabel(QSize)));
        connect(imageArea, SIGNAL(sendCursorPos(QPoint)), this, SLOT(setNewPosToPosLabel(QPoint)));
        connect(imageArea, SIGNAL(sendColor(QColor)), this, SLOT(setCurrentPipetteColor(QColor)));

        // 编辑操作控制信号：处理复制/粘贴/剪切等操作的可用性
        connect(imageArea, SIGNAL(sendEnableCopyCutActions(bool)), this, SLOT(enableCopyCutActions(bool)));
        connect(imageArea, SIGNAL(sendEnableSelectionInstrument(bool)), this, SLOT(instumentsAct(bool)));

        // ===== 更新主窗口标题 =====
        setWindowTitle(QString("%1 - OS drawing software").arg(fileName));
    } else {
        // ===== 文件加载失败处理 =====
        // 释放已创建的对象内存，防止内存泄漏
        delete imageArea;
    }
}

// 2025/08/19--代码已经审核通过
// 初始化主菜单
void MainWindow::initializeMainMenu()
{
    /********** 文件菜单 **********/
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    // 新建
    mNewAction = new QAction(tr("&New"), this);
    mNewAction->setIcon(QIcon::fromTheme("document-new", QIcon(":/new/prefix1/resourcesfile/file/document-new.png")));
    mNewAction->setIconVisibleInMenu(true);
    connect(mNewAction, SIGNAL(triggered()), this, SLOT(newAct()));
    fileMenu->addAction(mNewAction);

    // 打开
    mOpenAction = new QAction(tr("&Open"), this);
    mOpenAction->setIcon(QIcon::fromTheme("document-open", QIcon(":/new/prefix1/resourcesfile/file/document-open.png")));
    mOpenAction->setIconVisibleInMenu(true);
    connect(mOpenAction, SIGNAL(triggered()), this, SLOT(openAct()));
    fileMenu->addAction(mOpenAction);

    // 保存
    mSaveAction = new QAction(tr("&Save"), this);
    mSaveAction->setIcon(QIcon::fromTheme("document-save", QIcon(":/new/prefix1/resourcesfile/file/document-save.png")));
    mSaveAction->setIconVisibleInMenu(true);
    connect(mSaveAction, SIGNAL(triggered()), this, SLOT(saveAct()));
    fileMenu->addAction(mSaveAction);

    // 另存为
    mSaveAsAction = new QAction(tr("Save as..."), this);
    mSaveAsAction->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/new/prefix1/resourcesfile/file/document-save-as.png")));
    mSaveAsAction->setIconVisibleInMenu(true);
    connect(mSaveAsAction, SIGNAL(triggered()), this, SLOT(saveAsAct()));
    fileMenu->addAction(mSaveAsAction);

    // 关闭
    mCloseAction = new QAction(tr("&Close"), this);
    mCloseAction->setIcon(QIcon::fromTheme("window-close", QIcon(":/new/prefix1/resourcesfile/file/window-close.png")));
    mCloseAction->setIconVisibleInMenu(true);
    connect(mCloseAction, SIGNAL(triggered()), this, SLOT(closeTabAct()));
    fileMenu->addAction(mCloseAction);

    fileMenu->addSeparator();  // 分隔线

    // 打印
    mPrintAction = new QAction(tr("&Print"), this);
    mPrintAction->setIcon(QIcon::fromTheme("document-print", QIcon(":/new/prefix1/resourcesfile/file/document-print.png")));
    mPrintAction->setIconVisibleInMenu(true);
    connect(mPrintAction, SIGNAL(triggered()), this, SLOT(printAct()));
    fileMenu->addAction(mPrintAction);

    fileMenu->addSeparator();  // 分隔线

    // 退出
    mExitAction = new QAction(tr("&Exit"), this);
    mExitAction->setIcon(QIcon::fromTheme("application-exit", QIcon(":/new/prefix1/resourcesfile/file/application-exit.png")));
    mExitAction->setIconVisibleInMenu(true);
    connect(mExitAction, SIGNAL(triggered()), SLOT(close()));
    fileMenu->addAction(mExitAction);

    /********** 编辑菜单 **********/
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    // 撤销
    mUndoAction = mUndoStackGroup->createUndoAction(this, tr("&Undo"));
    mUndoAction->setIcon(QIcon::fromTheme("edit-undo", QIcon(":/new/prefix1/resourcesfile/edit/edit-undo.png")));
    mUndoAction->setIconVisibleInMenu(true);
    mUndoAction->setEnabled(false);  // 初始禁用
    editMenu->addAction(mUndoAction);

    // 重做
    mRedoAction = mUndoStackGroup->createRedoAction(this, tr("&Redo"));
    mRedoAction->setIcon(QIcon::fromTheme("edit-redo", QIcon(":/new/prefix1/resourcesfile/edit/edit-redo.png")));
    mRedoAction->setIconVisibleInMenu(true);
    mRedoAction->setEnabled(false);  // 初始禁用
    editMenu->addAction(mRedoAction);

    editMenu->addSeparator();  // 分隔线

    // 复制
    mCopyAction = new QAction(tr("&Copy"), this);
    mCopyAction->setIcon(QIcon::fromTheme("edit-copy", QIcon(":/new/prefix1/resourcesfile/edit/edit-copy.png")));
    mCopyAction->setIconVisibleInMenu(true);
    mCopyAction->setEnabled(false);  // 初始禁用
    connect(mCopyAction, SIGNAL(triggered()), this, SLOT(copyAct()));
    editMenu->addAction(mCopyAction);

    // 粘贴
    mPasteAction = new QAction(tr("&Paste"), this);
    mPasteAction->setIcon(QIcon::fromTheme("edit-paste", QIcon(":/new/prefix1/resourcesfile/edit/edit-paste.png")));
    mPasteAction->setIconVisibleInMenu(true);
    connect(mPasteAction, SIGNAL(triggered()), this, SLOT(pasteAct()));
    editMenu->addAction(mPasteAction);

    // 剪切
    mCutAction = new QAction(tr("C&ut"), this);
    mCutAction->setIcon(QIcon::fromTheme("edit-cut", QIcon(":/new/prefix1/resourcesfile/edit/edit-cut.png")));
    mCutAction->setIconVisibleInMenu(true);
    mCutAction->setEnabled(false);  // 初始禁用
    connect(mCutAction, SIGNAL(triggered()), this, SLOT(cutAct()));
    editMenu->addAction(mCutAction);

    editMenu->addSeparator();  // 分隔线

    // 设置
    QAction *settingsAction = new QAction(tr("&Settings"), this);
    settingsAction->setShortcut(QKeySequence::Preferences);
    settingsAction->setIcon(QIcon::fromTheme("document-properties", QIcon(":/new/prefix1/resourcesfile/edit/document-properties.png")));
    settingsAction->setIconVisibleInMenu(true);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(settingsAct()));
    editMenu->addAction(settingsAction);

    /********** 工具菜单 **********/
    mInstrumentsMenu = menuBar()->addMenu(tr("&Instructions"));

    // 以下创建各种绘图工具动作，使用QMap存储
    // 选择工具
    QAction *mCursorAction = new QAction(tr("Selection"), this);
    mCursorAction->setCheckable(true);  // 可选中
    mCursorAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/cursor.png"));
    connect(mCursorAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mCursorAction);
    mInstrumentsActMap.insert(CURSOR, mCursorAction);

    // 橡皮擦
    QAction *mEraserAction = new QAction(tr("Eraser"), this);
    mEraserAction->setCheckable(true);
    mEraserAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/lastic.png"));
    connect(mEraserAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mEraserAction);
    mInstrumentsActMap.insert(ERASER, mEraserAction);

    // 颜色拾取器
    QAction *mColorPickerAction = new QAction(tr("Color picker"), this);
    mColorPickerAction->setCheckable(true);
    mColorPickerAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/pipette.png"));
    connect(mColorPickerAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mColorPickerAction);
    mInstrumentsActMap.insert(COLORPICKER, mColorPickerAction);

    // 放大镜
    QAction *mMagnifierAction = new QAction(tr("Magnifier"), this);
    mMagnifierAction->setCheckable(true);
    mMagnifierAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/loupe.png"));
    connect(mMagnifierAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mMagnifierAction);
    mInstrumentsActMap.insert(MAGNIFIER, mMagnifierAction);

    // 铅笔
    QAction *mPenAction = new QAction(tr("Pen"), this);
    mPenAction->setCheckable(true);
    mPenAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/pencil.png"));
    connect(mPenAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mPenAction);
    mInstrumentsActMap.insert(PEN, mPenAction);

    // 直线
    QAction *mLineAction = new QAction(tr("Line"), this);
    mLineAction->setCheckable(true);
    mLineAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/line.png"));
    connect(mLineAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mLineAction);
    mInstrumentsActMap.insert(LINE, mLineAction);

    // 喷枪
    QAction *mSprayAction = new QAction(tr("Spray"), this);
    mSprayAction->setCheckable(true);
    mSprayAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/spray.png"));
    connect(mSprayAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mSprayAction);
    mInstrumentsActMap.insert(SPRAY, mSprayAction);

    // 填充
    QAction *mFillAction = new QAction(tr("Fill"), this);
    mFillAction->setCheckable(true);
    mFillAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/fill.png"));
    connect(mFillAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mFillAction);
    mInstrumentsActMap.insert(FILL, mFillAction);

    // 矩形
    QAction *mRectangleAction = new QAction(tr("Rectangle"), this);
    mRectangleAction->setCheckable(true);
    mRectangleAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/rectangle.png"));
    connect(mRectangleAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mRectangleAction);
    mInstrumentsActMap.insert(RECTANGLE, mRectangleAction);

    // 椭圆
    QAction *mEllipseAction = new QAction(tr("Ellipse"), this);
    mEllipseAction->setCheckable(true);
    mEllipseAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/ellipse.png"));
    connect(mEllipseAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mEllipseAction);
    mInstrumentsActMap.insert(ELLIPSE, mEllipseAction);

    // 曲线
    QAction *curveLineAction = new QAction(tr("Curve"), this);
    curveLineAction->setCheckable(true);
    curveLineAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/curve.png"));
    connect(curveLineAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(curveLineAction);
    mInstrumentsActMap.insert(CURVELINE, curveLineAction);

    // 文本
    QAction *mTextAction = new QAction(tr("Text"), this);
    mTextAction->setCheckable(true);
    mTextAction->setIcon(QIcon(":/new/prefix1/resourcesfile/instruction/text.png"));
    connect(mTextAction, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsMenu->addAction(mTextAction);
    mInstrumentsActMap.insert(TEXT, mTextAction);

    /********** 工具菜单 **********/
    mToolsMenu = menuBar()->addMenu(tr("&Tools"));

    // 调整图像尺寸
    QAction *resizeImAction = new QAction(tr("Image size..."), this);
    connect(resizeImAction, SIGNAL(triggered()), this, SLOT(resizeImageAct()));
    mToolsMenu->addAction(resizeImAction);

    // 旋转子菜单
    QMenu *rotateMenu = new QMenu(tr("Rotate"));

    // 逆时针旋转
    QAction *rotateLAction = new QAction(tr("Counter-clockwise"), this);
    rotateLAction->setIcon(QIcon::fromTheme("object-rotate-left", QIcon(":/new/prefix1/resourcesfile/tools/object-rotate-left.png")));
    rotateLAction->setIconVisibleInMenu(true);
    connect(rotateLAction, SIGNAL(triggered()), this, SLOT(rotateLeftImageAct()));
    rotateMenu->addAction(rotateLAction);

    // 顺时针旋转
    QAction *rotateRAction = new QAction(tr("Clockwise"), this);
    rotateRAction->setIcon(QIcon::fromTheme("object-rotate-right", QIcon(":/new/prefix1/resourcesfile/tools/object-rotate-right.png")));
    rotateRAction->setIconVisibleInMenu(true);
    connect(rotateRAction, SIGNAL(triggered()), this, SLOT(rotateRightImageAct()));
    rotateMenu->addAction(rotateRAction);

    mToolsMenu->addMenu(rotateMenu);

    // 缩放子菜单
    QMenu *zoomMenu = new QMenu(tr("Zoom"));

    // 放大
    mZoomInAction = new QAction(tr("Zoom In"), this);
    mZoomInAction->setIcon(QIcon::fromTheme("zoom-in", QIcon(":/new/prefix1/resourcesfile/tools/zoom-in.png")));
    mZoomInAction->setIconVisibleInMenu(true);
    connect(mZoomInAction, SIGNAL(triggered()), this, SLOT(zoomInAct()));
    zoomMenu->addAction(mZoomInAction);

    // 缩小
    mZoomOutAction = new QAction(tr("Zoom Out"), this);
    mZoomOutAction->setIcon(QIcon::fromTheme("zoom-out", QIcon(":/new/prefix1/resourcesfile/tools/zoom-out.png")));
    mZoomOutAction->setIconVisibleInMenu(true);
    connect(mZoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOutAct()));
    zoomMenu->addAction(mZoomOutAction);

    mToolsMenu->addMenu(zoomMenu);

    /********** 特效菜单 **********/
    QMenu *effectsMenu=menuBar()->addMenu(tr("&Effects"));

    // 灰度特效
    QAction *grayEffectAction = new QAction(tr("Gray"), this);
    grayEffectAction->setStatusTip(tr("将图像转换为灰度图像"));
    connect(grayEffectAction, SIGNAL(triggered()), this, SLOT(applyGrayEffect()));
    effectsMenu->addAction(grayEffectAction);
    mEffectsActMap.insert(GRAY, grayEffectAction);

    // 负片特效
    QAction *negativeEffectAction = new QAction(tr("Negative"), this);
    negativeEffectAction->setStatusTip(tr("反转图像的所有颜色"));
    connect(negativeEffectAction, SIGNAL(triggered()), this, SLOT(applyNegativeEffect()));
    effectsMenu->addAction(negativeEffectAction);
    mEffectsActMap.insert(NEGATIVE, negativeEffectAction);





    /********** 帮助菜单 **********/
    QMenu *aboutMenu = menuBar()->addMenu(tr("&About"));

    // 关于软件
    QAction *aboutAction = new QAction(tr("&About software"), this);
    aboutAction->setShortcut(QKeySequence::HelpContents);
    aboutAction->setIconVisibleInMenu(true);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(helpAct()));
    aboutMenu->addAction(aboutAction);

    // 关于Qt
    QAction *aboutQtAction = new QAction(tr("About Qt"), this);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    aboutMenu->addAction(aboutQtAction);

    // 更新快捷键
    updateShortcuts();
}



// 2025/08/19--代码已经审核通过
// 初始化状态栏
void MainWindow::initializeStatusBar()
{
    mStatusBar = new QStatusBar();
    setStatusBar(mStatusBar);  // 设置为主窗口状态栏

    // 创建状态栏组件
    mSizeLabel = new QLabel();          // 显示图像尺寸
    mPosLabel = new QLabel();            // 显示光标位置
    mColorPreviewLabel = new QLabel();   // 颜色预览
    mColorRGBLabel = new QLabel();       // RGB值显示

    // 添加到状态栏
    mStatusBar->addPermanentWidget(mSizeLabel, -1);
    mStatusBar->addPermanentWidget(mPosLabel, 1);
    mStatusBar->addPermanentWidget(mColorPreviewLabel);
    mStatusBar->addPermanentWidget(mColorRGBLabel, -1);
}

// 2025/08/19--代码已经审核通过
// 初始化工具栏
void MainWindow::initializeToolBar()
{
    // 创建工具栏并添加到左侧
    mToolbar = new ToolBar(mInstrumentsActMap, this);
    addToolBar(Qt::LeftToolBarArea, mToolbar);

    // 连接信号
    connect(mToolbar, SIGNAL(sendClearStatusBarColor()), this, SLOT(clearStatusBarColor()));
    connect(mToolbar, SIGNAL(sendClearImageSelection()), this, SLOT(clearImageSelection()));
}

// 2025/08/19--代码已经审核通过
// 初始化调色板栏
void MainWindow::initializePaletteBar()
{
    mPaletteBar = new PaletteBar(mToolbar);  // 依赖工具栏
    addToolBar(Qt::BottomToolBarArea, mPaletteBar);  // 添加到底部
}

// 获取当前激活的图像区域
// 使用场景：文件操作，编辑操作，图像处理，状态获取
ImageArea* MainWindow::getCurrentImageArea()
{
    // 检查是否存在当前激活的标签页
    if (mTabWidget->currentWidget()) {
        // 第一层转换：标签页控件 -> QScrollArea
        QScrollArea *tempScrollArea = qobject_cast<QScrollArea*>(mTabWidget->currentWidget());
        // 第二层转换：滚动区域 -> ImageArea
        ImageArea *tempArea = qobject_cast<ImageArea*>(tempScrollArea->widget());
        return tempArea;
    }
    // 无激活标签页时返回空指针
    return NULL;
}

// 通过索引获取图像区域
// 使用场景：关闭指定标签页；遍历所有标签页检查状态；批量操作多个文件。
ImageArea* MainWindow::getImageAreaByIndex(int index)
{
    // 第一层转换：通过索引获取标签页控件（QScrollArea）
    QScrollArea *sa = static_cast<QScrollArea*>(mTabWidget->widget(index));
    // 第二层转换：从滚动区域获取内部的ImageArea
    ImageArea *ia = static_cast<ImageArea*>(sa->widget());
    return ia;
}

// 2025/08/26--代码已经审核通过
// 激活标签页时的处理
void MainWindow::activateTab(const int &index)
{
    // ===== 参数有效性检查 =====
    if(index == -1) return;  // -1表示无效索引，直接返回

    // ===== 设置标签页组件的当前激活页 =====
    mTabWidget->setCurrentIndex(index);

    // ===== 清理前一个文件的状态 =====
    getCurrentImageArea()->clearSelection();  // 清除任何选中区域

    // ===== 更新状态栏中的图像尺寸显示 =====
    QSize size = getCurrentImageArea()->getImage()->size();
    mSizeLabel->setText(QString("%1 x %2").arg(size.width()).arg(size.height()));

    // ===== 更新主窗口标题 =====
    if(!getCurrentImageArea()->getFileName().isEmpty()) {
        // 已保存或已打开的文件：显示实际文件名
        setWindowTitle(QString("%1 - OS drawing software").arg(getCurrentImageArea()->getFileName()));
    } else {
        // 新建的未保存文件：显示默认标题
        setWindowTitle(QString("%1 - OS drawing software").arg(tr("Untitled Image")));
    }

    // ===== 切换撤销/重做系统的活跃栈 =====
    // 每个图像都有自己的撤销历史，需要切换到对应的栈
    mUndoStackGroup->setActiveStack(getCurrentImageArea()->getUndoStack());
}

// 2025/08/26--代码已经审核通过
// 更新状态栏的尺寸显示
void MainWindow::setNewSizeToSizeLabel(const QSize &size)
{
    // 格式化尺寸信息并显示在状态栏的尺寸标签中
    mSizeLabel->setText(QString("%1 x %2").arg(size.width()).arg(size.height()));

}

// 2025/08/26--代码已经审核通过
// 更新状态栏的光标位置
void MainWindow::setNewPosToPosLabel(const QPoint &pos)
{
    // 格式化坐标信息并显示在状态栏的位置标签中
    mPosLabel->setText(QString("%1,%2").arg(pos.x()).arg(pos.y()));

}

// 2025/08/26--代码已经审核通过
// 更新状态栏的颜色信息
void MainWindow::setCurrentPipetteColor(const QColor &color)
{
    // ===== 更新RGB数值文本显示 =====
    mColorRGBLabel->setText(QString("RGB: %1,%2,%3")
                                .arg(color.red())      // 红色分量 (0-255)
                                .arg(color.green())    // 绿色分量 (0-255)
                                .arg(color.blue()));   // 蓝色分量 (0-255)

    // ===== 创建颜色预览小方块 =====
    QPixmap statusColorPixmap = QPixmap(10, 10);  // 创建10x10像素的位图
    QPainter statusColorPainter;
    statusColorPainter.begin(&statusColorPixmap);  // 开始绘制
    statusColorPainter.fillRect(0, 0, 15, 15, color);  // 填充颜色（范围载大，确保完全覆盖）
    statusColorPainter.end();  // 结束绘制

    // ===== 将颜色预览图设置到标签中 =====
    mColorPreviewLabel->setPixmap(statusColorPixmap);

}

// 2025/08/26--代码已经审核通过
// 清除状态栏颜色显示
void MainWindow::clearStatusBarColor()
{
    // 清除颜色预览小方块的显示
    mColorPreviewLabel->clear();

    // 清除RGB数值文本显示
    mColorRGBLabel->clear();

}

/********** 菜单动作处理函数 **********/
// 新建文件
// 如下情况被调用：用户点击文件菜单-->新建；使用快捷键新建文件
void MainWindow::newAct()
{
    // 创建新的空白图像标签页
    initializeNewTab();
}

// 打开文件
void MainWindow::openAct()
{
    // 通过对话框打开文件（true表示打开模式，空路径表示显示对话框）
    initializeNewTab(true);
}

// 保存文件
void MainWindow::saveAct()
{
    // 检查是否有当前激活的图像编辑区域
    if (getCurrentImageArea()) {
        // 调用ImageArea的保存方法
        getCurrentImageArea()->save();

        // ===== 更新标签页标题 =====
        // 保存后文件名可能发生变化（特别是首次保存时）
        mTabWidget->setTabText(mTabWidget->currentIndex(),
                               getCurrentImageArea()->getFileName().isEmpty() ?
                                   tr("Untitled Image") : getCurrentImageArea()->getFileName());
    }
}

// 另存为
void MainWindow::saveAsAct()
{
    // 检查是否有当前激活的图像编辑区域
    if (getCurrentImageArea()) {
        // 调用ImageArea的另存为方法
        getCurrentImageArea()->saveAs();

        // ===== 更新标签页标题 =====
        // 另存为后文件名可能发生变化
        mTabWidget->setTabText(mTabWidget->currentIndex(),
                               getCurrentImageArea()->getFileName().isEmpty() ?
                                   tr("Untitled Image") : getCurrentImageArea()->getFileName());
    }
}

// 打印
void MainWindow::printAct()
{
    // 检查是否有当前激活的图像编辑区域
    if (getCurrentImageArea()) {
        // 调用ImageArea的打印方法
        getCurrentImageArea()->print();
    }
}

// 2025/08/19--代码已经审核通过
// 打开设置对话框
void MainWindow::settingsAct()
{
    SettingsDialog settingsDialog(this);
    if(settingsDialog.exec() == QDialog::Accepted) {
        settingsDialog.sendSettingsToSingleton();  // 应用设置到单例
        DataSingleton::Instance()->writeSettings(); // 保存设置
        updateShortcuts();  // 更新快捷键
    }
}

// 复制
void MainWindow::copyAct()
{
    // 获取当前激活的图像编辑区域
    if (ImageArea *imageArea = getCurrentImageArea()) {
        // 调用ImageArea的复制方法
        imageArea->copyImage();
    }
}

// 粘贴
void MainWindow::pasteAct()
{
    // 获取当前激活的图像编辑区域
    if (ImageArea *imageArea = getCurrentImageArea()) {
        // 调用ImageArea的粘贴方法
        imageArea->pasteImage();
    }
}

// 剪切
void MainWindow::cutAct()
{
    // 获取当前激活的图像编辑区域
    if (ImageArea *imageArea = getCurrentImageArea()) {
        // 调用ImageArea的剪切方法
        imageArea->cutImage();
    }
}


// 2025/08/19--代码已经审核通过
// 更新所有快捷键
void MainWindow::updateShortcuts()
{
    // 文件操作快捷键
    mNewAction->setShortcut(DataSingleton::Instance()->getFileShortcutByKey("New"));
    mOpenAction->setShortcut(DataSingleton::Instance()->getFileShortcutByKey("Open"));
    mSaveAction->setShortcut(DataSingleton::Instance()->getFileShortcutByKey("Save"));
    mSaveAsAction->setShortcut(DataSingleton::Instance()->getFileShortcutByKey("SaveAs"));
    mCloseAction->setShortcut(DataSingleton::Instance()->getFileShortcutByKey("Close"));
    mPrintAction->setShortcut(DataSingleton::Instance()->getFileShortcutByKey("Print"));
    mExitAction->setShortcut(DataSingleton::Instance()->getFileShortcutByKey("Exit"));

    // 编辑操作快捷键
    mUndoAction->setShortcut(DataSingleton::Instance()->getEditShortcutByKey("Undo"));
    mRedoAction->setShortcut(DataSingleton::Instance()->getEditShortcutByKey("Redo"));
    mCopyAction->setShortcut(DataSingleton::Instance()->getEditShortcutByKey("Copy"));
    mPasteAction->setShortcut(DataSingleton::Instance()->getEditShortcutByKey("Paste"));
    mCutAction->setShortcut(DataSingleton::Instance()->getEditShortcutByKey("Cut"));

    // 工具快捷键
    mInstrumentsActMap[CURSOR]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Cursor"));
    mInstrumentsActMap[ERASER]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Lastic"));
    mInstrumentsActMap[COLORPICKER]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Pipette"));
    mInstrumentsActMap[MAGNIFIER]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Loupe"));
    mInstrumentsActMap[PEN]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Pen"));
    mInstrumentsActMap[LINE]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Line"));
    mInstrumentsActMap[SPRAY]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Spray"));
    mInstrumentsActMap[FILL]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Fill"));
    mInstrumentsActMap[RECTANGLE]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Rect"));
    mInstrumentsActMap[ELLIPSE]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Ellipse"));
    mInstrumentsActMap[CURVELINE]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Curve"));
    mInstrumentsActMap[TEXT]->setShortcut(DataSingleton::Instance()->getInstrumentShortcutByKey("Text"));

    // 缩放快捷键
    mZoomInAction->setShortcut(DataSingleton::Instance()->getToolShortcutByKey("ZoomIn"));
    mZoomOutAction->setShortcut(DataSingleton::Instance()->getToolShortcutByKey("ZoomOut"));
}

// 调整图像尺寸
void MainWindow::resizeImageAct()
{
    if (getCurrentImageArea()) {
        getCurrentImageArea()->resizeImage();
    }
}

// 逆时针旋转
void MainWindow::rotateLeftImageAct()
{
    if (getCurrentImageArea()) {
        getCurrentImageArea()->rotateImage(false);  // false表示逆时针
    }
}

// 顺时针旋转
void MainWindow::rotateRightImageAct()
{
    if (getCurrentImageArea()) {
        getCurrentImageArea()->rotateImage(true);  // true表示顺时针
    }
}

// 放大
void MainWindow::zoomInAct()
{
    if (getCurrentImageArea()) {
        getCurrentImageArea()->zoomImage(2.0);     // 缩放因子2.0
        getCurrentImageArea()->setZoomFactor(2.0); // 记录缩放因子
    }
}

// 缩小
void MainWindow::zoomOutAct()
{
    if (getCurrentImageArea()) {
        getCurrentImageArea()->zoomImage(0.5);     // 缩放因子0.5
        getCurrentImageArea()->setZoomFactor(0.5); // 记录缩放因子
    }
}

// 关闭当前标签页
void MainWindow::closeTabAct()
{
    closeTab(mTabWidget->currentIndex());
}

// 2025/08/26--代码已经审核通过
// 关闭指定标签页
void MainWindow::closeTab(int index)
{
    ImageArea *ia = getImageAreaByIndex(index);
    // 如果图像已被修改
    if(ia->getEdited()) {
        // 弹出保存提示
        int ans = QMessageBox::warning(this, tr("关闭"),
                                       tr("系统提示：是否要关闭文件，\n当前文件已被修改，\n请问是否要保存更改?"),
                                       QMessageBox::Yes | QMessageBox::Default,
                                       QMessageBox::No,
                                       QMessageBox::Cancel | QMessageBox::Escape);
        switch(ans) {
        case QMessageBox::Yes:
            if (!ia->save()) return;  // 保存失败则取消关闭
            break;
        case QMessageBox::Cancel:
            return;  // 取消关闭
        }
    }

    // 从撤销组移除栈
    mUndoStackGroup->removeStack(ia->getUndoStack());
    // 删除标签页
    QWidget *wid = mTabWidget->widget(index);
    mTabWidget->removeTab(index);
    delete wid;

    // 所有标签页关闭后更新标题
    if (mTabWidget->count() == 0) {
        setWindowTitle("Empty - OS drawing software");
    }
}


// 主窗口关闭事件
void MainWindow::closeEvent(QCloseEvent *event)
{
    // 检查所有标签页修改状态
    if(!isSomethingModified() || closeAllTabs()) {
        // 保存窗口状态
        DataSingleton::Instance()->setWindowSize(size());
        DataSingleton::Instance()->writeState();
        event->accept();  // 接受关闭
    } else {
        event->ignore();  // 忽略关闭
    }
}

// 检查是否有修改过的标签页
bool MainWindow::isSomethingModified()
{
    for(int i = 0; i < mTabWidget->count(); ++i) {
        if(getImageAreaByIndex(i)->getEdited())
            return true;
    }
    return false;
}

// 关闭所有标签页
bool MainWindow::closeAllTabs()
{
    while(mTabWidget->count() != 0) {
        ImageArea *ia = getImageAreaByIndex(0);
        if(ia->getEdited()) {
            // 弹出保存提示
            int ans = QMessageBox::warning(this, tr("关闭"),
                                           tr("系统提示：是否要关闭应用程序，\n当前文件已被修改，\n请问是否要保存更改?"),
                                           QMessageBox::Yes | QMessageBox::Default,
                                           QMessageBox::No,
                                           QMessageBox::Cancel | QMessageBox::Escape);
            switch(ans) {
            case QMessageBox::Yes:
                if (!ia->save()) return false;  // 保存失败返回false
                break;
            case QMessageBox::Cancel:
                return false;  // 取消关闭
            }
        }
        // 移除标签页
        QWidget *wid = mTabWidget->widget(0);
        mTabWidget->removeTab(0);
        delete wid;
    }
    return true;
}


// 取消所有工具的选中状态
void MainWindow::setAllInstrumentsUnchecked(QAction *action)
{
    clearImageSelection();  // 清除选区
    // 遍历所有工具动作
    foreach (QAction *temp, mInstrumentsActMap) {
        if(temp != action)  // 排除当前动作
            temp->setChecked(false);
    }
}

// 设置指定工具为选中状态
void MainWindow::setInstrumentChecked(InstrumentsEnum instrument)
{
    setAllInstrumentsUnchecked(NULL);  // 先取消所有选中
    if(instrument == NONE_INSTRUMENT || instrument == INSTRUMENTS_COUNT)
        return;
    mInstrumentsActMap[instrument]->setChecked(true);  // 选中指定工具
}


// 2025/08/26--代码已经审核通过
// 工具动作触发处理
void MainWindow::instumentsAct(bool state)
{
    QAction *currentAction = static_cast<QAction*>(sender());
    if(state) {  // 如果是选中状态
        // 颜色拾取器特殊处理（记录前一个工具）
        if(currentAction == mInstrumentsActMap[COLORPICKER] && !mPrevInstrumentSetted) {
            DataSingleton::Instance()->setPreviousInstrument(DataSingleton::Instance()->getInstrument());
            mPrevInstrumentSetted = true;
        }

        // 更新界面状态
        setAllInstrumentsUnchecked(currentAction);
        currentAction->setChecked(true);

        // 更新当前工具
        InstrumentsEnum instrument = mInstrumentsActMap.key(currentAction);
        DataSingleton::Instance()->setInstrument(instrument);
        emit sendInstrumentChecked(instrument);
    } else {  // 如果是取消选中状态
        setAllInstrumentsUnchecked(NULL);
        DataSingleton::Instance()->setInstrument(NONE_INSTRUMENT);
        emit sendInstrumentChecked(NONE_INSTRUMENT);

        // 选择工具特殊处理（记录为上一个工具）
        if(currentAction == mInstrumentsActMap[CURSOR]) {
            DataSingleton::Instance()->setPreviousInstrument(mInstrumentsActMap.key(currentAction));
        }
    }
}

// 2025/08/26--代码已经审核通过
// 根据标签页状态启用/禁用菜单
void MainWindow::enableActions(int index)
{
    // 是否有激活的标签页
    bool isEnable = (index != -1);

    // 设置菜单可用状态
    mToolsMenu->setEnabled(isEnable);
    mInstrumentsMenu->setEnabled(isEnable);
    mToolbar->setEnabled(isEnable);
    mPaletteBar->setEnabled(isEnable);

    // 设置文件操作可用状态
    mSaveAction->setEnabled(isEnable);
    mSaveAsAction->setEnabled(isEnable);
    mCloseAction->setEnabled(isEnable);
    mPrintAction->setEnabled(isEnable);

    // 无标签页时重置工具状态
    if(!isEnable) {
        setAllInstrumentsUnchecked(NULL);
        DataSingleton::Instance()->setInstrument(NONE_INSTRUMENT);
        emit sendInstrumentChecked(NONE_INSTRUMENT);
    }
}

// 2025/08/26--代码已经审核通过
// 启用/禁用复制剪切操作
void MainWindow::enableCopyCutActions(bool enable)
{
    mCopyAction->setEnabled(enable);
    mCutAction->setEnabled(enable);
}


// 2025/08/19--代码已经审核通过
// 清除图像选区
void MainWindow::clearImageSelection()
{
    if(getCurrentImageArea())
    {
        getCurrentImageArea()->clearSelection();
        DataSingleton::Instance()->setPreviousInstrument(NONE_INSTRUMENT);
    }
}

// 2025/08/26--代码已经审核通过
// 恢复前一个工具
void MainWindow::restorePreviousInstrument()
{
    InstrumentsEnum prev = DataSingleton::Instance()->getPreviousInstrument();
    setInstrumentChecked(prev);
    DataSingleton::Instance()->setInstrument(prev);
    emit sendInstrumentChecked(prev);
    mPrevInstrumentSetted = false;  // 重置标志
}

// 2025/08/26--代码已经审核通过
// 设置当前工具
void MainWindow::setInstrument(InstrumentsEnum instrument)
{
    setInstrumentChecked(instrument);
    DataSingleton::Instance()->setInstrument(instrument);
    emit sendInstrumentChecked(instrument);
    mPrevInstrumentSetted = false;
}

// 2025/08/19--代码已经审核通过
// 显示关于软件信息
void MainWindow::helpAct()
{
    QMessageBox::about(this, tr("About software"),
                       QString("<b>Software</b> %1: %2 <br> %3: "
                               "<li>LingSheng Education</li>"
                               ).arg(tr("Version")).arg("2.2.2").arg(tr("Authors")));

}


// 特效操作实现
// 应用灰度特效
void MainWindow::applyGrayEffect()
{
    if(ImageArea *imageArea=getCurrentImageArea()){
        imageArea->applyEffect(GRAY);
    }
}

// 应用负片特效
void MainWindow::applyNegativeEffect()
{
    if(ImageArea *imageArea=getCurrentImageArea()){
        imageArea->applyEffect(NEGATIVE);
    }

}

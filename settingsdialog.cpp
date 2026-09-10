#include "settingsdialog.h"


// 构造函数：初始化对话框
SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)  // 调用基类构造函数
{
    initializeGui();  // 初始化GUI界面
    layout()->setSizeConstraint(QLayout::SetFixedSize); // 设置固定窗口大小
    setWindowTitle(tr("Settings")); // 设置窗口标题（支持多语言翻译）`
}
SettingsDialog::~SettingsDialog(){}

// 将设置发送到单例对象保存
void SettingsDialog::sendSettingsToSingleton()
{
    // 保存基础设置
    DataSingleton::Instance()->setBaseSize(QSize(mWidth->value(), mHeight->value()));
    DataSingleton::Instance()->setHistoryDepth(mHistoryDepth->value());
    DataSingleton::Instance()->setIsAutoSave(mIsAutoSave->isChecked());
    DataSingleton::Instance()->setIsRestoreWindowSize(mIsRestoreWindowSize->isChecked());
    DataSingleton::Instance()->setIsAskCanvasSize(mIsAskCanvasSize->isChecked());
    DataSingleton::Instance()->setAutoSaveInterval(mAutoSaveInterval->value());

    // 保存语言设置
    QStringList languages;
    languages << "system" << "easypaint_en_EN" << "easypaint_cs_CZ"
              << "easypaint_fr_FR" << "easypaint_ru_RU";
    DataSingleton::Instance()->setAppLanguage(languages.at(mLanguageBox->currentIndex()));

    // 保存快捷键设置（遍历树形结构）
    QTreeWidgetItem *item;
    for(int i = 0; i < mShortcutsTree->topLevelItemCount(); i++){
        item = mShortcutsTree->topLevelItem(i);
        for(int y = 0; y < item->childCount(); y++){
            QString group = item->text(0); // 分组名称
            QTreeWidgetItem *child = item->child(y);
            QString command = child->text(0); // 命令名称
            QKeySequence shortcut = child->data(1, Qt::DisplayRole).value<QKeySequence>(); // 快捷键

            // 根据分组保存到不同的设置区域
            if(group == "File") {
                DataSingleton::Instance()->setFileShortcutByKey(command, shortcut);
            }
            else if(group == "Edit") {
                DataSingleton::Instance()->setEditShortcutByKey(command, shortcut);
            }
            else if(group == "Instruments") {
                DataSingleton::Instance()->setInstrumentShortcutByKey(command, shortcut);
            }
            else if(group == "Tools") {
                DataSingleton::Instance()->setToolShortcutByKey(command, shortcut);
            }
        }
    }
}

// 初始化GUI界面
void SettingsDialog::initializeGui()
{
    QTabWidget *tabWidget = new QTabWidget(this); // 创建标签页容器
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                       QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept())); // 确定按钮连接accept()
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject())); // 取消按钮连接reject()

    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    /********************* 第一标签页：常规设置 *********************/
    // 语言设置部分
    QLabel *label1 = new QLabel(tr("Language:"));
    mLanguageBox = new QComboBox();
    // 添加语言选项（注意：第一个是系统默认）
    mLanguageBox->addItem(tr("<System>"));
    mLanguageBox->addItem("English");
    mLanguageBox->setCurrentIndex(getLanguageIndex()); // 设置当前选中项

    QHBoxLayout *hBox1 = new QHBoxLayout();
    hBox1->addWidget(label1);
    hBox1->addWidget(mLanguageBox);

    QVBoxLayout *vBox1 = new QVBoxLayout();
    vBox1->addLayout(hBox1);

    // 语言更改提示  注意：更改语言需要重新启动应用程序
    QLabel *label4 = new QLabel(tr("Note: language changing requires application restart"));
    vBox1->addWidget(label4);

    // 窗口尺寸恢复选项  开始时恢复窗口大小
    mIsRestoreWindowSize = new QCheckBox(tr("Restore window size on start"));
    mIsRestoreWindowSize->setChecked(DataSingleton::Instance()->getIsRestoreWindowSize());
    vBox1->addWidget(mIsRestoreWindowSize);

    // 画布尺寸询问选项   在创建新图像时询问画布大小
    mIsAskCanvasSize = new QCheckBox(tr("Ask canvas size on new image creation"));
    mIsAskCanvasSize->setChecked(DataSingleton::Instance()->getIsAskCanvasSize());
    vBox1->addWidget(mIsAskCanvasSize);

    // 用户界面设置组
    QGroupBox *groupBox1 = new QGroupBox(tr("User interface"));
    groupBox1->setLayout(vBox1);

    // 图像相关设置
    QLabel *label2 = new QLabel(tr("Base size:"));
    QLabel *label3 = new QLabel(" x ");
    mWidth = new QSpinBox();  // 画布宽度
    mWidth->setRange(1, 9999); // 设置范围
    mWidth->setValue(DataSingleton::Instance()->getBaseSize().width());
    mHeight = new QSpinBox(); // 画布高度
    mHeight->setRange(1, 9999);
    mHeight->setValue(DataSingleton::Instance()->getBaseSize().height());

    // 尺寸布局
    QHBoxLayout *hBox2 = new QHBoxLayout();
    hBox2->addWidget(mWidth);
    hBox2->addWidget(label3);
    hBox2->addWidget(mHeight);

    // 历史记录深度
    QLabel *label5 = new QLabel(tr("History depth:"));
    mHistoryDepth = new QSpinBox();
    mHistoryDepth->setRange(1, 99); // 撤销/重做历史深度
    mHistoryDepth->setValue(DataSingleton::Instance()->getHistoryDepth());
    mHistoryDepth->setFixedWidth(80); // 固定宽度

    // 自动保存设置
    mIsAutoSave = new QCheckBox(tr("Autosave"));
    mIsAutoSave->setChecked(DataSingleton::Instance()->getIsAutoSave());

    // 自动保存间隔
    QLabel *label6 = new QLabel(tr("Autosave interval (sec):"));
    mAutoSaveInterval = new QSpinBox();
    mAutoSaveInterval->setRange(1, 3000); // 1-3000秒范围
    mAutoSaveInterval->setValue(DataSingleton::Instance()->getAutoSaveInterval());
    mAutoSaveInterval->setFixedWidth(80);

    // 网格布局组装图像设置
    QGridLayout *gLayout = new QGridLayout();
    gLayout->addWidget(label2, 0, 0);
    gLayout->addLayout(hBox2, 0, 1);
    gLayout->addWidget(label5, 1, 0);
    gLayout->addWidget(mHistoryDepth, 1, 1);
    gLayout->addWidget(mIsAutoSave, 2, 0);
    gLayout->addWidget(label6, 3, 0);
    gLayout->addWidget(mAutoSaveInterval, 3, 1);

    // 图像设置组
    QGroupBox *groupBox2 = new QGroupBox(tr("Image"));
    groupBox2->setLayout(gLayout);

    // 组装第一标签页
    QVBoxLayout *vBox2 = new QVBoxLayout();
    vBox2->addWidget(groupBox1); // 用户界面组
    vBox2->addWidget(groupBox2); // 图像设置组

    QWidget *firstTabWidget = new QWidget();
    firstTabWidget->setLayout(vBox2);

    tabWidget->addTab(firstTabWidget, tr("General")); // 添加标签页

    /********************* 第二标签页：快捷键设置 *********************/
    QGroupBox *groupBox3 = new QGroupBox(tr("Keyboard shortcuts"));
    QVBoxLayout *vBox3 = new QVBoxLayout();
    groupBox3->setLayout(vBox3);

    // 快捷键树形列表
    mShortcutsTree = new QTreeWidget();
    QStringList header;
    header << tr("Command") << tr("Shortcut"); // 表头
    mShortcutsTree->setHeaderLabels(header);
    // 连接选择变化信号
    connect(mShortcutsTree, SIGNAL(itemSelectionChanged()),
            this, SLOT(itemSelectionChanged()));

    // 从单例加载快捷键并创建分组
    createItemsGroup("File", DataSingleton::Instance()->getFileShortcuts());
    createItemsGroup("Edit", DataSingleton::Instance()->getEditShortcuts());
    createItemsGroup("Instruments", DataSingleton::Instance()->getInstrumentsShortcuts());
    createItemsGroup("Tools", DataSingleton::Instance()->getToolsShortcuts());

    vBox3->addWidget(mShortcutsTree); // 添加到布局

    // 快捷键编辑区域
    QGroupBox *groupBox4 = new QGroupBox(tr("Shortcut"));
    QHBoxLayout *hBox5 = new QHBoxLayout();
    groupBox4->setLayout(hBox5);

    QLabel *label7 = new QLabel(tr("Key sequence:"));
    mShortcutEdit = new ShortcutEdit(); // 自定义快捷键编辑框
    mShortcutEdit->setEnabled(false); // 初始禁用
    // 文本变化信号连接
    connect(mShortcutEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    hBox5->addWidget(label7);
    hBox5->addWidget(mShortcutEdit);

    // 组装第二标签页
    QVBoxLayout *vBox5 = new QVBoxLayout();
    vBox5->addWidget(groupBox3); // 快捷键列表
    vBox5->addWidget(groupBox4); // 编辑区域

    QWidget *secondTabWidget = new QWidget();
    secondTabWidget->setLayout(vBox5);

    tabWidget->addTab(secondTabWidget, tr("Keyboard")); // 添加标签页
}


// 获取当前语言在组合框中索引（预留）
int SettingsDialog::getLanguageIndex()
{
    // 与组合框选项顺序对应的语言标识列表
    QStringList languages;

    // 其他部分到时再实现
    languages<<"system"<<"pfs_en_EN";

    return languages.indexOf(DataSingleton::Instance()->getAppLanguage());
}

// 创建快捷键分组项
void SettingsDialog::createItemsGroup(const QString &name, const QMap<QString, QKeySequence> &shortcuts)
{
    // 创建顶级分组项
    QTreeWidgetItem *topLevel = new QTreeWidgetItem(mShortcutsTree);
    topLevel->setText(0, name); // 设置分组名称
    topLevel->setExpanded(true); // 默认展开

    // 遍历快捷键映射
    QMapIterator<QString, QKeySequence> iterator(shortcuts);
    while(iterator.hasNext()) {
        iterator.next();
        // 创建子项（命令+快捷键）
        QTreeWidgetItem *subLevel = new QTreeWidgetItem(topLevel);
        subLevel->setText(0, iterator.key()); // 命令名称
        subLevel->setData(1, Qt::DisplayRole, iterator.value()); // 快捷键数据
    }
}

// 当树形控件选择变化时的处理
void SettingsDialog::itemSelectionChanged()
{
    if(!mShortcutsTree->selectedItems().isEmpty()) {
        QTreeWidgetItem *selected = mShortcutsTree->selectedItems().first();

        // 如果选中的是分组项（有子项）
        if(selected->childCount() != 0) {
            mShortcutEdit->setEnabled(false); // 禁用编辑
            mShortcutEdit->clear(); // 清空内容
        }
        // 选中的是具体命令项
        else {
            mShortcutEdit->setEnabled(true); // 启用编辑
            // 显示当前快捷键（第1列数据）
            mShortcutEdit->setText(selected->text(1));
        }
        mShortcutEdit->setFocus(); // 聚焦到编辑框
    }
}

// 当快捷键编辑框内容变化时的处理
void SettingsDialog::textChanged(const QString &text)
{
    if(!mShortcutsTree->selectedItems().isEmpty()) {
        QTreeWidgetItem *selected = mShortcutsTree->selectedItems().first();
        // 更新树形项的第1列数据（快捷键）
        selected->setData(1, Qt::DisplayRole, text);
    }
}


// 重置快捷键设置
void SettingsDialog::reset()
{
    mShortcutEdit->clear(); // 仅清空编辑框
    // 注意：此处应添加恢复默认快捷键的逻辑
}

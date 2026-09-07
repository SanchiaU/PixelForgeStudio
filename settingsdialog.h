// 防止头文件重复包含
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


// 2025/07/24--代码已经审核通过



#include <QObject>
#include <QDialog>

#include "shortcutedit.h"       // 包含自定义快捷键编辑控件
#include "datasingleton.h"      // 包含数据单例的头文件

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>


// 声明前置的Qt类（不包含具体定义，加快编译速度）
QT_BEGIN_NAMESPACE
class QComboBox;        // 下拉选择框
class QSpinBox;         // 数字输入框
class QCheckBox;        // 复选框
class QLineEdit;        // 单行文本输入框
class QTreeWidget;      // 树形控件
class QTreeWidgetItem;  // 树形控件项
class QPushButton;      // 按钮
class QKeySequence;     // 键盘序列（快捷键）
QT_END_NAMESPACE

class ShortcutEdit;         // 自定义的快捷键编辑控件


// 应用程序设计对话框
// 主要用于配置应用程序的各种设置：包括语言、界面尺寸、历史记录深度、自动保存等，并支持管理管理键盘快捷键
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent);  // 显式构造函数（避免隐式转换）
    ~SettingsDialog();  // 析构函数

    // 将当前对话框中的设置发送到单例数据管理器
    // 收集用户在对话框中的所有设置值，并通过单例模式存储到应用程序的全局设置中
    void sendSettingsToSingleton();

private:
    // 初始化GUI界面
    void initializeGui();

    // 获取当前选择的语言索引（预留）
    int getLanguageIndex();

    // 在快捷键树形控件中创建分组项
    // name 分组名称（比如：文件操作）
    // shortcuts 该分组包含的快捷键映射（动作名称->快捷键序列）
    void createItemsGroup(const QString &name,const QMap<QString,QKeySequence> &shortcuts);

    // GUI控件成员变量
    QComboBox *mLanguageBox;          // 语言选择下拉框
    QSpinBox *mWidth, *mHeight;       // 画布宽度/高度设置
    QSpinBox *mHistoryDepth;          // 撤销/重做历史记录深度
    QSpinBox *mAutoSaveInterval;      // 自动保存间隔时间（单位：分钟）
    QCheckBox *mIsAutoSave;           // 是否启用自动保存
    QCheckBox *mIsRestoreWindowSize;  // 是否记住窗口尺寸
    ShortcutEdit *mShortcutEdit;      // 自定义快捷键编辑器
    QTreeWidget *mShortcutsTree;      // 显示所有快捷键的树形控件
    QCheckBox *mIsAskCanvasSize;      // 是否在新建文件时询问画布尺寸

private slots:
    // 槽函数（响应界面交互）
    void itemSelectionChanged();            // 当树形控件中选择项变化时触发
    void textChanged(const QString &text);  // 当快捷键编辑框内容变化时触发
    void reset();                           // 重置所有设置为默认值

};

#endif // SETTINGSDIALOG_H

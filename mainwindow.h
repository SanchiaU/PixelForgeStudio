#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// 2025/08/14--代码已经审核通过

#include <QMainWindow>          // 主窗口基类
#include <QtCore/QMap>          // Qt映射容器
#include "easypaintenums.h"     // 自定义枚举类型（工具和效果枚举）

#include "toolbar.h"            // 自定义工具栏头文件
#include "imagearea.h"          // 绘图区域类头文件
#include "datasingleton.h"      // 单例数据管理类头文件
#include "settingsdialog.h"     // 设置对话框头文件
#include "palettebar.h"         // 调色板工具栏头文件

// Qt核心模块
#include <QApplication>
#include <QAction>                    // 动作对象
#include <QMenu>                      // 菜单
#include <QMenuBar>                   // 菜单栏
#include <QStatusBar>                 // 状态栏
#include <QMessageBox>                // 消息框
#include <QScrollArea>                // 滚动区域
#include <QLabel>                     // 标签
#include <QtEvents>                   // Qt事件系统
#include <QPainter>                   // 绘图工具
#include <QInputDialog>               // 输入对话框
#include <QUndoGroup>                 // 撤销/重做组
#include <QtCore/QTimer>              // 定时器



// 前置声明Qt类（优化编译速度）
QT_BEGIN_NAMESPACE
class QAction;        // 界面操作（菜单项/工具栏按钮）
class QStatusBar;     // 状态栏组件
class QTabWidget;     // 标签页组件
class ToolBar;        // 自定义工具栏
class PaletteBar;     // 自定义调色板栏
class ImageArea;      // 图像编辑区域
class QLabel;         // 标签组件
class QUndoGroup;     // 撤销/重做管理组
QT_END_NAMESPACE



// 绘图软件主窗口类：负责管理整个应用程序的用户界面
class MainWindow : public QMainWindow
{
    Q_OBJECT    // Qt元对象系统宏，支持信号槽机制

public:
    // 构造函数
    // filePaths--启动时要打开的文件路径列表
    // parent--父窗口指针
    MainWindow(QStringList filePaths, QWidget *parent = 0);
    ~MainWindow();  // 析构函数

protected:
    // 重写容器关闭事件处理函数
    // event--关闭事件对象
    void closeEvent(QCloseEvent *event) override;

private:
    // 初始化函数
    void initializeMainMenu();     // 初始化主菜单
    void initializeStatusBar();    // 初始化状态栏
    void initializeToolBar();      // 初始化工具栏
    void initializePaletteBar();   // 初始化调色板栏
    void initializeTabWidget();    // 初始化标签页组件

    /**
      * @brief 创建新标签页（图像编辑区域）
      * @param isOpen 是否打开现有文件
      * @param filePath    要打开的文件路径
      */
    void initializeNewTab(const bool &isOpen = false, const QString &filePath = "");

    // 辅助函数
    ImageArea* getCurrentImageArea();              // 获取当前激活的图像区域
    ImageArea* getImageAreaByIndex(int index);     // 通过索引获取图像区域
    bool closeAllTabs();                           // 关闭所有标签页
    bool isSomethingModified();                    // 检查是否有未保存的修改
    void updateShortcuts();                        // 更新快捷键设置

    // UI组件指针
    QStatusBar *mStatusBar;         // 状态栏
    QTabWidget *mTabWidget;         // 标签页容器（管理多个图像编辑区域）
    ToolBar *mToolbar;              // 自定义工具栏（工具选择）
    PaletteBar *mPaletteBar;        // 自定义调色板栏（颜色选择）

    // 状态栏标签
    QLabel *mSizeLabel;            // 显示图像尺寸
    QLabel *mPosLabel;             // 显示光标位置
    QLabel *mColorPreviewLabel;    // 颜色预览方块
    QLabel *mColorRGBLabel;        // 颜色RGB值

    // 工具和效果映射
    QMap<InstrumentsEnum, QAction*> mInstrumentsActMap;  // 工具->动作映射

    // 文件操作动作
    QAction *mSaveAction;      // 保存动作
    QAction *mSaveAsAction;    // 另存为动作
    QAction *mCloseAction;     // 关闭标签页动作
    QAction *mPrintAction;     // 打印动作

    // 编辑操作动作
    QAction *mUndoAction;      // 撤销动作
    QAction *mRedoAction;      // 重做动作
    QAction *mCopyAction;      // 复制动作
    QAction *mCutAction;       // 剪切动作

    // 其他动作
    QAction *mNewAction;       // 新建动作
    QAction *mOpenAction;      // 打开动作
    QAction *mExitAction;      // 退出动作
    QAction *mPasteAction;     // 粘贴动作
    QAction *mZoomInAction;    // 放大动作
    QAction *mZoomOutAction;   // 缩小动作

    // 菜单指针
    QMenu *mInstrumentsMenu;   // 工具菜单
    QMenu *mToolsMenu;         // 工具菜单（图像操作）

    // 撤销管理
    QUndoGroup *mUndoStackGroup;  // 撤销/重做管理组（管理多个编辑区域的撤销栈）

    // 状态标志
    bool mPrevInstrumentSetted;  // 标记是否已设置上一个工具（用于取色器等特殊工具）

private slots:  // 槽函数（事件处理函数）
    void activateTab(const int &index);                  // 激活标签页
    void setNewSizeToSizeLabel(const QSize &size);       // 更新尺寸标签
    void setNewPosToPosLabel(const QPoint &pos);         // 更新位置标签
    void setCurrentPipetteColor(const QColor &color);    // 更新取色器颜色显示
    void clearStatusBarColor();                          // 清除颜色显示
    void setInstrumentChecked(InstrumentsEnum instrument); // 设置工具选中状态

    // 菜单动作处理函数
    void newAct();        // 新建文件
    void openAct();       // 打开文件
    void helpAct();       // 显示帮助信息
    void saveAct();       // 保存文件
    void saveAsAct();     // 另存为
    void printAct();      // 打印
    void copyAct();       // 复制
    void pasteAct();      // 粘贴
    void cutAct();        // 剪切
    void settingsAct();   // 打开设置对话框

    // 图像操作
    void resizeImageAct();       // 调整图像尺寸
    void rotateLeftImageAct();   // 向左旋转图像
    void rotateRightImageAct();  // 向右旋转图像
    void zoomInAct();            // 放大图像
    void zoomOutAct();           // 缩小图像

    // 标签页操作
    void closeTabAct();          // 关闭当前标签页
    void closeTab(int index);    // 关闭指定索引的标签页

    // 工具管理
    void setAllInstrumentsUnchecked(QAction *action);  // 取消所有工具的选中状态
    void instumentsAct(bool state);                   // 工具动作触发处理
    void enableActions(int index);                    // 启用/禁用动作
    void enableCopyCutActions(bool enable);           // 启用/禁用复制剪切动作
    void clearImageSelection();                       // 清除图像选区
    void restorePreviousInstrument();                 // 恢复上一个工具
    void setInstrument(InstrumentsEnum instrument);   // 设置当前工具

signals:  // 信号（事件通知）
    void sendInstrumentChecked(InstrumentsEnum);  // 通知工具选中状态变化


};
#endif // MAINWINDOW_H

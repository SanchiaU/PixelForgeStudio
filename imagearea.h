#ifndef IMAGEAREA_H
#define IMAGEAREA_H



// 2025/07/31--代码已经审核通过



#include "easypaintenums.h"         // 自定义枚举类型（工具类型等）
#include "additionaltools.h"        // 辅助工具类（提供颜色选择器等工具）
#include "datasingleton.h"    // 单例数据管理器（全局数据共享）
#include "undocommand.h"            // 自定义撤销命令实现（支持撤销/重做操作）

//#include "grayeffect.h"

#include <QWidget>             // Qt 窗口部件基类
#include <QImage>              // Qt 图像处理类

// 其他Qt相关头文件
#include <QApplication>         // Qt应用程序类
#include <QPainter>             // 绘图工具类
#include <QFileDialog>          // 文件对话框
#include <QtCore/QDebug>        // 调试输出
#include <QMouseEvent>          // 鼠标事件处理
#include <QPaintEvent>          // 绘制事件
#include <QPrinter>             // 打印支持
#include <QPrintDialog>         // 打印对话框
#include <QtCore/QTimer>        // 定时器
#include <QImageReader>         // 图像读取
#include <QImageWriter>         // 图像写入
#include <QUndoStack>           // 撤销/重做栈
#include <QtCore/QDir>          // 目录操作
#include <QMessageBox>          // 消息提示框
#include <QClipboard>           // 剪贴板操作

// 声明Qt命名空间中的类（前置声明）
QT_BEGIN_NAMESPACE
class QUndoStack;       //Qt的标准撤销/重做栈类
QT_END_NAMESPACE

// 前置声明自定义类
class UndoCommand;          // 自定义撤销命令类
class AbstractInstrument;   // 抽象绘图工具基类
class AbstractEffect;         // 抽象特效基类


// 图像编辑区域的核心类，集成图像视图和绘制控制器的基类功能
// 负责管理图像数据、处理用户交互、执行编辑操作，并支持撤销/重做功能
// 继承自QWidget，作为图像编辑的主画布区域
class ImageArea : public QWidget
{
    Q_OBJECT  // 启用Qt元对象系统（支持信号槽机制和属性系统）

public:
    // 构造函数
    explicit ImageArea(const bool &isOpen,  // 是否初始打开文件
                       const QString &filePath, // 初始文件路径
                       QWidget *parent);     // 父窗口
    ~ImageArea();  // 析构函数（释放资源）

    /// 文件操作 ///
    bool save();        // 保存到当前路径（若未保存过则调用saveAs）
    bool saveAs();      // 弹出对话框选择路径保存
    void print();       // 打印当前图像

    /// 图像变换 ///
    void resizeImage();     // 调整图像尺寸（保持宽高比）
    void rotateImage(bool flag); // 旋转图像（true=顺时针90°, false=逆时针90°）

    /// 属性访问 ///
    // 获取文件名（不含路径）
    inline QString getFileName() {
        return (mFilePath.isEmpty() ? mFilePath : mFilePath.split('/').last());
    }

    inline QImage* getImage() { return mImage; }  // 获取图像指针（直接像素访问）
    inline void setImage(const QImage &image) { *mImage = image; }  // 设置新图像

    // 编辑状态管理
    inline void setEdited(bool flag) { mIsEdited = flag; }  // 设置修改标志（需保存）
    inline bool getEdited() { return mIsEdited; }           // 获取修改状态

    /// 光标管理 ///
    void restoreCursor();   // 恢复工具对应的光标样式

    /// 特效处理 ///
    void applyEffect(EffectsEnum effect);  // 应用指定特效

    /// 缩放控制 ///
    bool zoomImage(qreal factor); // 缩放图像（factor>1放大，<1缩小）
    inline void setZoomFactor(qreal factor) { mZoomFactor *= factor; }  // 累积缩放因子
    inline qreal getZoomFactor() { return mZoomFactor; }  // 获取当前缩放比例

    /// 撤销系统 ///
    inline QUndoStack* getUndoStack() { return mUndoStack; }  // 获取撤销栈指针

    /// 绘制状态 ///
    inline void setIsPaint(bool isPaint) { mIsPaint = isPaint; }  // 设置绘制中标志
    inline bool isPaint() { return mIsPaint; }                   // 检查是否绘制中

    /// 信号发射封装 ///
    inline void emitPrimaryColorView() { emit sendPrimaryColorView(); }     // 通知更新主色
    inline void emitSecondaryColorView() { emit sendSecondaryColorView(); } // 通知更新辅色
    inline void emitColor(QColor &color) { emit sendColor(color); }         // 发送颜色值
    inline void emitRestorePreviousInstrument() { emit sendRestorePreviousInstrument(); } // 恢复前工具

    /// 剪贴板操作 ///
    void copyImage();   // 复制选区到剪贴板
    void pasteImage();  // 从剪贴板粘贴图像
    void cutImage();    // 剪切选区到剪贴板

    /// 版本管理 ///
    void saveImageChanges(); // 保存当前状态（用于撤销）
    void clearSelection();   // 清除选区状态

    /// 撤销命令管理 ///
    void pushUndoCommand(UndoCommand *command);  // 提交自定义撤销命令

private:
    /// 初始化方法 ///
    void initializeImage();  // 初始化空白图像

    /// 文件操作 ///
    void open();                    // 打开默认路径图像
    void open(const QString &filePath); // 打开指定路径图像

    /// 辅助功能 ///
    void drawCursor();         // 绘制自定义工具光标
    void makeFormatsFilters(); // 生成文件格式过滤器字符串
    void makeUndoCommand(); // 创建撤销命令（用于特效应用

    // 图像数据
    QImage *mImage;         // 主图像数据（当前编辑状态）
    QImage mImageCopy;      // 图像副本（用于临时操作/恢复）
    AdditionalTools *mAdditionalTools; // 辅助工具管理器

    // 文件信息
    QString mFilePath;      // 当前文件路径
    QString mOpenFilter;    // 打开文件格式过滤器（如"Images (*.png *.jpg)"）
    QString mSaveFilter;    // 保存文件格式过滤器

    // 状态标志
    bool mIsEdited;         // 图像是否被修改
    bool mIsPaint;          // 是否正在进行绘制操作
    bool mIsResize;         // 是否正在调整尺寸
    bool mRightButtonPressed; // 鼠标右键按下状态

    // 图形显示
    QPixmap *mPixmap;       // 用于高效显示的像素图
    QCursor *mCurrentCursor; // 当前活动光标

    qreal mZoomFactor;      // 当前缩放比例（1.0=原始大小）

    // 撤销系统
    QUndoStack *mUndoStack;  // 操作历史栈

    // 工具系统
    QVector<AbstractInstrument*> mInstrumentsHandlers;  // 可用工具集合
    AbstractInstrument *mInstrumentHandler;             // 当前激活工具

    // 特效系统
    QVector<AbstractEffect*> mEffectsHandlers;  // 可用特效集合
    AbstractEffect *mEffectHandler;             // 当前激活特效

signals:
    // 颜色信号
    void sendPrimaryColorView();    // 请求更新主色显示
    void sendSecondaryColorView();  // 请求更新辅色显示
    void sendColor(const QColor&);  // 传递颜色值

    // 图像状态信号
    void sendNewImageSize(const QSize&);  // 图像尺寸变化时发出
    void sendCursorPos(const QPoint&);    // 光标位置变化时发出

    // 工具管理信号
    void sendRestorePreviousInstrument(); // 请求恢复前一个工具
    void sendSetInstrument(InstrumentsEnum); // 请求切换工具

    // 菜单状态信号
    void sendEnableCopyCutActions(bool enable);  // 启用/禁用复制剪切动作
    void sendEnableSelectionInstrument(bool enable); // 启用/禁用选区工具

private slots:
    void autoSave();  // 定时自动保存槽函数

protected:
    // 事件处理重载
    void mousePressEvent(QMouseEvent *event);    // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event);     // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event);  // 鼠标释放事件
    void paintEvent(QPaintEvent *event);         // 窗口绘制事件
};

#endif // IMAGEAREA_H








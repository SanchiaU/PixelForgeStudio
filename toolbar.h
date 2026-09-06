// 防止头文件被重复包含
#ifndef TOOLBAR_H
#define TOOLBAR_H


#include <QObject>
#include "easypaintenums.h"// 包含自定义的枚举类型（定义绘图工具类型）
#include "colorchooser.h"
#include "datasingleton.h"

// 包含Qt工具栏基类
#include <QToolBar>
#include <QToolButton>
#include <QGridLayout>        // 网格布局
#include <QSpinBox>           // 数字输入框
#include <QAction>
#include <QtCore/QMap>

// 声明前置类（优化编译）
QT_BEGIN_NAMESPACE
class QToolButton;     // Qt工具按钮控件
class ColorChooser;    // 自定义颜色选择器控件
QT_END_NAMESPACE


// 应用程序主工具栏类
// 包含绘图工具按钮、颜色选择器等交互控件，继承自Qt标准工具栏QToolBar
class ToolBar : public QToolBar
{
    Q_OBJECT            // 启用Qt元对象系统（信号槽机制）
public:
    // 构造函数
    explicit ToolBar(const QMap<InstrumentsEnum, QAction*> &actMap, QWidget *parent = 0);


private:
    // 初始化所有工具栏控件
    void initializeItems();

    // 创建工具按钮
    // act 关联的QAction（提供图标、文本等信息）
    QToolButton* createToolButton(QAction *act);

    // 工具按钮成员（按功能命名）
    QToolButton *mCursorButton;     // 光标选择工具
    QToolButton *mEraserButton;     // 橡皮擦工具
    QToolButton *mPenButton;        // 画笔工具
    QToolButton *mLineButton;       // 直线工具
    QToolButton *mColorPickerButton;// 取色器工具
    QToolButton *mMagnifierButton;  // 放大镜工具
    QToolButton *mSprayButton;      // 喷枪工具
    QToolButton *mFillButton;       // 填充工具
    QToolButton *mRectangleButton;  // 矩形工具
    QToolButton *mEllipseButton;    // 椭圆工具
    QToolButton *mCurveButton;      // 曲线工具
    QToolButton *mTextButton;       // 文字工具

    // 颜色选择器
    ColorChooser *mPColorChooser;   // 主色选择器
    ColorChooser *mSColorChooser;   // 辅色（次色）选择器

    bool mPrevInstrumentSetted;     // 标记上一个工具是否已设置
    const QMap<InstrumentsEnum, QAction*> &mActMap; // 工具枚举到动作的映射引用
signals:
    // 请求清除状态栏颜色显示
    void sendClearStatusBarColor();
    // 请求清除图片选区
    void sendClearImageSelection();
public slots:
    void setPrimaryColorView();// 更新主色显示
    void setSecondaryColorView();// 更新辅色显示

private slots:
    void penValueChanged(const int &value);// 处理画笔粗细值变化
    void primaryColorChanged(const QColor &color);// 处理主色变更
    void secondaryColorChanged(const QColor &color);// 处理辅色变更

};

#endif // TOOLBAR_H

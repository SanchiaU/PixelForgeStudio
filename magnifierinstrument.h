#ifndef MAGNIFIERINSTRUMENT_H
#define MAGNIFIERINSTRUMENT_H



// 2025/08/05--代码已经审核通过



#include <QtCore/QObject>           // 引入Qt核心对象模块，支持信号槽机制
#include "abstractinstrument.h"
#include "imagearea.h"              // 包含图像区域类，提供图像操作功能


// 放大镜工具类
// 继承自 AbstractInstrument，实现图像编辑中的放大镜功能
// 用户可通过鼠标操作对特定区域进行放大查看。
class MagnifierInstrument : public AbstractInstrument
{
    Q_OBJECT  // 启用Qt的元对象系统（信号槽、属性等）
public:
    // 构造函数
    explicit MagnifierInstrument(QObject *parent = 0);

    // 鼠标按下事件处理
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea) override;

    // 鼠标移动事件处理
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea) override;

    // 鼠标释放事件处理
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea) override;

    // 执行实际绘制操作（重写基类虚函数）
    // 备注：放大镜工具不修改原图
protected:
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false) override;

};

#endif // MAGNIFIERINSTRUMENT_H

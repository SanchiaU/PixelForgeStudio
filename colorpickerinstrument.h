#ifndef COLORPICKERINSTRUMENT_H
#define COLORPICKERINSTRUMENT_H



// 2025/08/02--代码已经审核通过




// 包含基类头文件
#include "abstractinstrument.h"

#include "abstractinstrument.h"     // 包含基类头文件
#include "imagearea.h"              // 图像区域管理类
#include "datasingleton.h"          // 单例数据管理

// 颜色拾取器工具类
// 该工具用于从图像区域中拾取颜色。当用户在图像上点击或移动时，
// 会获取对应像素位置的颜色值，并将其设置为当前主颜色或次颜色。
class ColorpickerInstrument : public AbstractInstrument
{
    Q_OBJECT                // 启用Qt的元对象系统（信号/槽机制）
public:
    // 构造函数
    explicit ColorpickerInstrument(QObject *parent = 0);


    // ========================鼠标事件处理函数========================
    // 鼠标按下事件
    // 当鼠标按下时拾取当前像素颜色，并设置为当前主颜色
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea) override;

    // 鼠标移动事件
    // 通常用于实现拖动时连续拾取颜色
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea) override;

    // 鼠标释放事件
    // 执行最后的清理操作
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea) override;

protected:
    // 实际颜色设置实现函数
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false) override;

};

#endif // COLORPICKERINSTRUMENT_H



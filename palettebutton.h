// 防止头文件重复包含
#ifndef PALETTEBUTTON_H
#define PALETTEBUTTON_H


// 2025/07/22--代码已经审核通过


#include <QObject>

// 包含Qt的工具按钮类
#include <QToolButton>
// 包含Qt鼠标事件处理类
#include <QMouseEvent>

// 包含自定义的单列数据类
#include "datasingleton.h"


// 自定义的调色板按钮类，继承自QToolButton
class PaletteButton : public QToolButton
{
    Q_OBJECT  // 启用Qt的元对象系统（信号槽等特性）

public:
    // 构造函数：接收一个颜色参数初始化按钮
    explicit PaletteButton(const QColor &color);

signals:
    // 信号：当用户点击选择该颜色时发射
    void colorPicked();

private:
    QColor mColor;  // 存储该按钮代表的颜色

protected:
    // 重写鼠标点击事件处理函数
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // QPALETTEBUTTON_H

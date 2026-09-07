#include "palettebutton.h"


// 2025/07/22--代码已经审核通过


// 构造函数：创建颜色选择按钮
PaletteButton::PaletteButton(const QColor &color)
{
    mColor = color;  // 存储按钮关联的颜色

    // 设置按钮尺寸（固定为30x30像素）
    setMinimumSize(QSize(30, 30));
    setMaximumSize(QSize(30, 30));

    // 创建20x20像素的色块图标
    QPixmap pixmap(20, 20);
    pixmap.fill(color);  // 用指定颜色填充图标

    // 设置按钮图标（显示颜色方块）
    setIcon(pixmap);

    // 设置状态提示（鼠标悬停时显示颜色HEX值）
    setStatusTip(color.name());
}

// 鼠标点击事件处理函数
void PaletteButton::mousePressEvent(QMouseEvent *event)
{
    // 左键点击：设置为主色
    if(event->button() == Qt::LeftButton) {
        // 通过单例模式设置全局主色
        DataSingleton::Instance()->setPrimaryColor(mColor);
    }
    // 右键点击：设置为辅色
    else if(event->button() == Qt::RightButton) {
        // 通过单例模式设置全局辅色
        DataSingleton::Instance()->setSecondaryColor(mColor);
    }

    // 发射颜色选择信号（通知其他组件颜色已更新）
    emit colorPicked();

    // 可选：调用基类实现保持默认行为
    QToolButton::mousePressEvent(event);
}

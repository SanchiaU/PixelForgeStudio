#include "palettebutton.h"

// 构造函数：创建颜色选择按钮
PaletteButton::PaletteButton(const QColor &color)
{
    mColor = color;  // 存储按钮关联的颜色

    // 设置按钮尺寸（固定为30x30像素）
    setMinimumSize(QSize(30, 30));
    setMaximumSize(QSize(30, 30));
    QPixmap pixmap(20, 20);
    pixmap.fill(color);  // 用指定颜色填充图标
    setIcon(pixmap);
    setStatusTip(color.name());
}

// 鼠标点击事件处理函数
void PaletteButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        DataSingleton::Instance()->setPrimaryColor(mColor);
    }else if(event->button() == Qt::RightButton) {
        DataSingleton::Instance()->setSecondaryColor(mColor);
    }
    emit colorPicked();
    QToolButton::mousePressEvent(event);
}

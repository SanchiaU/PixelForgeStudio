#include "colorchooser.h"

/*
构造函数：初始化颜色选择器
初始化流程：
1.设置标签样式
2.创建当前颜色对象
3.创建颜色显示位置
4.绘制初始化颜色块
5.设置标签显示属性
*/
ColorChooser::ColorChooser(const int &r, const int &g, const int &b, QWidget *parent) :
    QLabel(parent)  // 调用基类构造函数
{
    // 设置边框样式：凸起+盒子效果
    setFrameStyle(QFrame::Raised | QFrame::Box);
    mCurrentColor = new QColor(r, g, b);
    mPixmapColor = new QPixmap(20, 20);
    mPainterColor = new QPainter(mPixmapColor);
    mPainterColor->fillRect(0, 0, 20, 20, *mCurrentColor);
    mPainterColor->end();
    setMargin(3);
    setAlignment(Qt::AlignHCenter);
    setPixmap(*mPixmapColor);
}
ColorChooser::~ColorChooser(){
    delete mCurrentColor;    // 释放颜色对象
    delete mPainterColor;    // 释放绘制工具
    delete mPixmapColor;     // 释放位图资源
}
// 设置新颜色的槽函数，更新当前颜色并刷新显示   更新当前颜色-->重新绘制颜色块-->更新UI显示
void ColorChooser::setColor(const QColor &color){
    *mCurrentColor = color;
    mPainterColor->begin(mPixmapColor);
    mPainterColor->fillRect(0, 0, 20, 20, *mCurrentColor);
    mPainterColor->end();
    setPixmap(*mPixmapColor);
}

// 鼠标点击事件处理，点击时弹出颜色选择对话框  检测左键点击-->弹出颜色选择对话框-->处理用户选择的颜色
void ColorChooser::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        QColor color = QColorDialog::getColor(*mCurrentColor, this);
        if(color.isValid()){
            setColor(color);
            emit sendColor(color);
        }
    }
}



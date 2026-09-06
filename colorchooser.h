#ifndef COLORCHOOSER_H
#define COLORCHOOSER_H

#include <QLabel>  //显示颜色块
#include <QObject>
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>

QT_BEGIN_NAMESPACE
// 前置声明Qt类（避免包含整个头文件）
class QColor;       // 颜色对象
class QPixmap;      // 图像处理类
class QPainter;     // 绘图工具
class QMouseEvent;  // 鼠标事件
class QColorDialog; // 颜色选择对话框
QT_END_NAMESPACE

// 颜色选择器控件
// 基于QLabel实现的可点击颜色选择器，显示当前颜色块
// 点击时弹出颜色对话框供用户选择新颜色
class ColorChooser : public QLabel
{
    Q_OBJECT
public:
    explicit ColorChooser(const int &r, const int &g, const int &b,
                          QWidget *parent = 0);
    ~ColorChooser();
private:
    // 成员变量
    QColor *mCurrentColor;    // 存储当前颜色的对象指针
    QPixmap *mPixmapColor;    // 用于绘制颜色块的像素图
    QPainter *mPainterColor;   // 绘图工具，用于在像素图上绘制颜色
public slots:
    void setColor(const QColor &color);// 设置新颜色的槽函数，更新当前颜色并刷新显示
signals:
    void sendColor(const QColor &);
protected:
    void mousePressEvent(QMouseEvent *event);

};


#endif // COLORCHOOSER_H

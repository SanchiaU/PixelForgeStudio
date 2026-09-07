#ifndef FILLINSTRUMENT_H
#define FILLINSTRUMENT_H



// 2025/08/06--代码已经审核通过



#include "abstractinstrument.h"  // 包含基础工具类的定义
#include <QtCore/QObject>       // 包含Qt核心对象模块

#include "imagearea.h"      // 包含绘图区域类
#include "datasingleton.h"  // 包含全局数据单例

#include <QPen>                     // Qt画笔类
#include <QPainter>                 // Qt绘图工具


// 填充工具类（油漆桶效果）
// 实现类似油漆桶的区域填充功能：
// 基于种子填充算法（种子扩散法）
// 递归填充相邻相似颜色区域
// 支持主色/次色填充选择
class FillInstrument : public AbstractInstrument  // 继承自抽象工具基类
{
    Q_OBJECT  // Qt宏，启用信号槽机制和元对象系统

public:
    // 构造函数
    explicit FillInstrument(QObject *parent = 0);

    // 鼠标按下事件处理
    void mousePressEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标移动事件处理
    void mouseMoveEvent(QMouseEvent *event, ImageArea &imageArea);

    // 鼠标释放事件处理
    void mouseReleaseEvent(QMouseEvent *event, ImageArea &imageArea);

protected:
    // 实际填充方法
    // 核心算法：获取点击位置的原始颜色，创建临时图像副本，调用递归填充算法
    void paint(ImageArea &imageArea, bool isSecondaryColor = false, bool additionalFlag = false);

private:
    // 递归填充算法实现
    // 算法流程：检查当前像素是否需要填充，填充当前像素，递归检查四个方向（上、下、左、右）
    // 注意：递归深度可能受栈大小限制，大区域可能需迭代算法
    void fillRecurs(int x, int y, QRgb switchColor, QRgb oldColor, QImage &tempImage);


};

#endif // FILLINSTRUMENT_H

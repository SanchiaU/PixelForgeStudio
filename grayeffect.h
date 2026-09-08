#ifndef GRAYEFFECT_H
#define GRAYEFFECT_H



// 2025/08/30--代码已经审核通过



#include "abstracteffect.h"


// 灰度化特效类
// 将彩色图像转换为灰度图像
// 使用加权平均法：Gray = 0.299*R + 0.587*G + 0.114*B
class GrayEffect : public AbstractEffect
{
    Q_OBJECT
public:
    // 构造函数
    explicit GrayEffect(QObject *parent = nullptr);

    // 应用灰度化特效
    bool apply(QImage &image) override;

    // 获取特效名称
    QString getName() const override;

    // 获取特效描述
    QString getDescription() const override;

private:
    /**
         * @brief 计算像素的灰度值
         * @param r 红色分量
         * @param g 绿色分量
         * @param b 蓝色分量
         * @return 灰度值
         */
    int calculateGrayValue(int r, int g, int b) const;


};

#endif // GRAYEFFECT_H

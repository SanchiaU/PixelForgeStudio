#ifndef NEGATIVEEFFECT_H
#define NEGATIVEEFFECT_H

// 2025/09/02--代码已经审核通过

#include "abstracteffect.h"


// 负片特效类
// 反转图像的颜色，创建类似胶片底片的效果
// 计算公式：newValue=255-originalVlaue
class NegativeEffect : public AbstractEffect
{
    Q_OBJECT
public:
    // 构造函数
    // parent 父对象指针
    explicit NegativeEffect(QObject *parent = nullptr);

    // 应用负片特效
    // image 要处理的图像引用
    // return 处理成功返回true，失败返回false
    bool apply(QImage & image) override;

    // 获取特效名称
    QString getName() const override;

    // 获取特效描述
    QString getDescription() const override;

private:
    // 反转颜色分量
    // value 原始颜色值（0-255）
    // return 返回后的颜色值
    inline int invertValue(int value) const{
        return 255-value;
    }

};

#endif // NEGATIVEEFFECT_H

#ifndef ABSTRACTEFFECT_H
#define ABSTRACTEFFECT_H



// 2025/08/30--代码已经审核通过



#include <QtCore/QObject>       // Qt核心对象基类
#include <QImage>               // 图像处理类


// 声明前置引用（避免循环依赖）
QT_BEGIN_NAMESPACE
class ImageArea; // 声明ImageArea类
QT_END_NAMESPACE


// 抽象图像特效基类
// 定义所有图像特效（灰度化、模糊等）必须实现的通用接口和行为
class AbstractEffect : public QObject
{
    Q_OBJECT        // 启用Qt的元对象系统（信号槽机制）
public:
    // 构造函数
    // parent 父对象指针（遵循Qt对象树管理）
    explicit AbstractEffect(QObject *parent = nullptr);

    // 虚拟函数（确保派生类正确销毁）
    virtual ~AbstractEffect() {}

    // 纯虚函数：应用特效到图像
    virtual bool apply(QImage &image)=0;

    // 获取特效名称
    virtual QString getName() const =0;

    // 获取特效描述
    virtual QString getDescription() const =0;

    // 检查特效是否支持指定的图像格式
    virtual bool supportsFormat(QImage::Format format) const;

protected:
    // 验证图像是否有效
    bool validateImage(const QImage &image) const;

    // 将图像转换为RGB格式（如果需要）
    bool ensureRGBFormat(QImage &image) const;

    // 限制数值在指定范围内
    template<typename T>
    T clamp(T value, T min, T max) const {
        return (value < min) ? min : (value > max) ? max : value;
    }


signals:
    // 特效处理进度信号
    void progressChanged(int progress);

    // 特效处理完成信号
    void effectCompleted(bool success);

};

#endif // ABSTRACTEFFECT_H

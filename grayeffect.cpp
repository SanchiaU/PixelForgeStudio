#include "grayeffect.h"



// 2025/08/30--代码已经审核通过



#include <QRgb>

// GrayEffect构造函数
GrayEffect::GrayEffect(QObject *parent) :
    AbstractEffect(parent)
{
}

// 应用灰度化特效
bool GrayEffect::apply(QImage &image)
{
    // 验证图像有效性
    if (!validateImage(image)) {
        emit effectCompleted(false);
        return false;
    }

    // 确保图像格式正确
    if (!ensureRGBFormat(image)) {
        emit effectCompleted(false);
        return false;
    }

    int width = image.width();
    int height = image.height();
    int totalPixels = width * height;
    int processedPixels = 0;

    // 逐像素处理
    for (int y = 0; y < height; ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(image.scanLine(y));

        for (int x = 0; x < width; ++x) {
            QRgb pixel = line[x];

            // 提取RGB分量
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);
            int a = qAlpha(pixel);

            // 计算灰度值
            int grayValue = calculateGrayValue(r, g, b);

            // 设置新的像素值（保持透明度）
            line[x] = qRgba(grayValue, grayValue, grayValue, a);

            processedPixels++;
        }

        // 更新进度（每处理一行更新一次）
        int progress = (processedPixels * 100) / totalPixels;
        emit progressChanged(progress);
    }

    emit effectCompleted(true);
    return true;
}

// 获取特效名称
QString GrayEffect::getName() const
{
    return tr("灰度特效");
}

// 获取特效描述
QString GrayEffect::getDescription() const
{
    return tr("将彩色图像转换为灰度图像，使用加权平均算法以保持视觉亮度");
}


/**
 * @brief 计算像素的灰度值
 * @param r 红色分量
 * @param g 绿色分量
 * @param b 蓝色分量
 * @return 灰度值
 */
int GrayEffect::calculateGrayValue(int r, int g, int b) const
{
    // 使用标准的加权平均公式
    // 人眼对绿色最敏感，对蓝色最不敏感
    double gray = 0.299 * r + 0.587 * g + 0.114 * b;
    return this->clamp(static_cast<int>(gray + 0.5), 0, 255);
}




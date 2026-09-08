#include "negativeeffect.h"



// 2025/09/02--代码已经审核通过



NegativeEffect::NegativeEffect(QObject *parent):
    AbstractEffect(parent)
{

}

// 应用负片特效
bool NegativeEffect::apply(QImage &image)
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

            // 反转RGB分量（保持透明度不变）
            int newR = invertValue(r);
            int newG = invertValue(g);
            int newB = invertValue(b);

            // 设置新的像素值
            line[x] = qRgba(newR, newG, newB, a);

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
QString NegativeEffect::getName() const
{
    return tr("负片效果");
}

// 获取特效描述
QString NegativeEffect::getDescription() const
{
    return tr("反转图像的所有颜色，创建类似胶片底片的视觉效果");
}


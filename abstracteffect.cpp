#include "abstracteffect.h"



// 2025/08/30--代码已经审核通过



// AbstractEffect构造函数
AbstractEffect::AbstractEffect(QObject *parent) : QObject(parent)
{

}

// 检查特效是否支持指定的图像格式
bool AbstractEffect::supportsFormat(QImage::Format format) const
{
    // 默认支持常见的RGB和ARGB格式
    switch (format) {
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_RGB888:
    case QImage::Format_RGBX8888:
    case QImage::Format_RGBA8888:
    case QImage::Format_RGBA8888_Premultiplied:
        return true;
    default:
        return false;
    }
}


// 验证图像是否有效
bool AbstractEffect::validateImage(const QImage &image) const
{
    if (image.isNull()) {
        return false;
    }

    if (image.width() <= 0 || image.height() <= 0) {
        return false;
    }

    if (!supportsFormat(image.format())) {
        return false;
    }

    return true;
}

// 将图像转换为RGB格式
bool AbstractEffect::ensureRGBFormat(QImage &image) const
{
    if (image.isNull()) {
        return false;
    }

    // 如果已经是支持的格式，不需要转换
    if (supportsFormat(image.format())) {
        return true;
    }

    // 转换为ARGB32格式（最通用的格式）
    QImage convertedImage = image.convertToFormat(QImage::Format_ARGB32);
    if (convertedImage.isNull()) {
        return false;
    }

    image = convertedImage;

    return true;
}



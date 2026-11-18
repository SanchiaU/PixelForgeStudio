#include "additionaltools.h"


#include "imagearea.h"          // 图像显示区域
#include "resizedialog.h"       // 尺寸调整对话框

AdditionalTools::AdditionalTools(ImageArea *pImageArea, QObject *parent) :
    QObject(parent)
{
    mPImageArea = pImageArea;  // 保存图像区域指针
    mZoomedFactor = 1;         // 初始缩放因子设为1（原始大小）
}

// void AdditionalTools::resizeCanvas(int width, int height, bool flag)
// {
//     if(flag)
//     {
//         ResizeDialog resizeDialog(QSize(width, height), qobject_cast<QWidget *>(this->parent()));
//         if(resizeDialog.exec() == QDialog::Accepted)
//         {
//             QSize newSize = resizeDialog.getNewSize();
//             width = newSize.width();
//             height = newSize.height();
//         } else {
//             return;
//         }
//     }

//     if(width < 1 || height < 1)
//         return;

//     QImage *tempImage = new QImage(width, height, QImage::Format_ARGB32_Premultiplied);
//     QPainter painter(tempImage);

//     painter.setPen(Qt::NoPen);
//     painter.setBrush(QBrush(Qt::white));
//     painter.drawRect(QRect(0, 0, width, height));

//     painter.drawImage(0, 0, *mPImageArea->getImage());
//     painter.end(); // 结束绘制

//     // 更新图像区域
//     mPImageArea->setImage(*tempImage);

//     // 调整显示区域尺寸（图像尺寸+6像素边框）
//     mPImageArea->resize(mPImageArea->getImage()->rect().right() + 6,
//                         mPImageArea->getImage()->rect().bottom() + 6);

//     // 标记图像已被编辑
//     mPImageArea->setEdited(true);
//     // 清除可能存在的选区
//     mPImageArea->clearSelection();
// }
void AdditionalTools::resizeCanvas(int width, int height, bool flag)
{
    if (flag)
    {
        ResizeDialog resizeDialog(QSize(width, height), qobject_cast<QWidget *>(this->parent()));
        if (resizeDialog.exec() == QDialog::Accepted)
        {
            QSize newSize = resizeDialog.getNewSize();
            width  = newSize.width();
            height = newSize.height();
        }
        else
        {
            return;
        }
    }

    if (width < 1 || height < 1)
        return;

    QImage *oldImage = mPImageArea->getImage();
    if (!oldImage || oldImage->isNull())      // ★ 上游已经是空图，别再往下做
        return;

    // ★★ 短路①：尺寸没变就直接返回
    //    拖动时大量 mouseMove 的目标尺寸和上一帧相同/没跨过像素边界
    if (oldImage->width() == width && oldImage->height() == height)
        return;

    // ★★ 短路②：上限保护，别让一次拖动把进程撑爆
    static const int kMaxSide = 8192;         // 8192² × 4B ≈ 256MB，可按需调整
    width  = qBound(1, width,  kMaxSide);
    height = qBound(1, height, kMaxSide);

    // ★★ 修正核心：栈对象，靠 RAII 自动释放，彻底没有 new/delete
    QImage newImage(width, height, QImage::Format_ARGB32_Premultiplied);
    if (newImage.isNull())                    // 分配失败：放弃这一帧，不要污染 mImage
    {
        qWarning() << "resizeCanvas: QImage 分配失败" << width << height;
        return;
    }

    {
        QPainter painter(&newImage);
        painter.fillRect(newImage.rect(), Qt::white);   // 新区域铺白
        painter.drawImage(0, 0, *oldImage);             // 左上角对齐，保留原内容
    }                                                   // ★ painter 析构 = end()

    mPImageArea->setImage(newImage);                    // 只有这一处赋值

    // 保持原来的"图像 + 6px 手柄"语义
    mPImageArea->resize(mPImageArea->getImage()->rect().right() + 6,
                        mPImageArea->getImage()->rect().bottom() + 6);

    mPImageArea->setEdited(true);
    mPImageArea->clearSelection();
}


// 调整图像尺寸（可能保持宽度比或适应画布）
void AdditionalTools::resizeImage()
{
    ResizeDialog resizeDialog(mPImageArea->getImage()->size(), qobject_cast<QWidget *>(this->parent()));

    if(resizeDialog.exec() == QDialog::Accepted)
    {
        mPImageArea->setImage(mPImageArea->getImage()->scaled(resizeDialog.getNewSize()));

        mPImageArea->resize(mPImageArea->getImage()->rect().right() + 6,
                            mPImageArea->getImage()->rect().bottom() + 6);

        mPImageArea->setEdited(true);
        mPImageArea->clearSelection();
    }
}

// 旋转图像（旋转图像（90度））
// flag 旋转方向标识（true表示顺时针90度，false表示逆时针90度）
void AdditionalTools::rotateImage(bool flag)
{
    QTransform transform;
    if(flag)
    {
        transform.rotate(90);  // 顺时针旋转90度
    }
    else
    {
        transform.rotate(-90); // 逆时针旋转90度
    }

    // 应用旋转变换
    mPImageArea->setImage(mPImageArea->getImage()->transformed(transform));

    // 调整显示区域尺寸
    mPImageArea->resize(mPImageArea->getImage()->rect().right() + 6,
                        mPImageArea->getImage()->rect().bottom() + 6);

    mPImageArea->update();
    mPImageArea->setEdited(true);
    mPImageArea->clearSelection();
}


// 缩放图像
// factor 缩放因子（>1放大  <1缩小）
bool AdditionalTools::zoomImage(qreal factor)
{
    // 累积缩放因子
    mZoomedFactor *= factor;

    // 缩放范围限制（0.25x - 4x）
    if(mZoomedFactor < 0.25)
    {
        mZoomedFactor = 0.25; // 达到最小限制
        return false;
    }
    else if(mZoomedFactor > 4)
    {
        mZoomedFactor = 4; // 达到最大限制
        return false;
    }
    else
    {
        // 应用缩放变换
        mPImageArea->setImage(mPImageArea->getImage()->transformed(QTransform::fromScale(factor, factor)));

        // 调整显示区域尺寸
        mPImageArea->resize(
            (mPImageArea->rect().width()) * factor,
            (mPImageArea->rect().height()) * factor
            );

        // 发送新尺寸信号（通知其他组件更新）
        emit sendNewImageSize(mPImageArea->size());

        // 标记编辑状态并清除选区
        mPImageArea->setEdited(true);
        mPImageArea->clearSelection();
        return true;
    }
}








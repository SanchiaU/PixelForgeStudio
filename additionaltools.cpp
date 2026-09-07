#include "additionaltools.h"



// 2025/07/28--代码已经审核通过



#include "imagearea.h"          // 图像显示区域
#include "resizedialog.h"       // 尺寸调整对话框

AdditionalTools::AdditionalTools(ImageArea *pImageArea, QObject *parent) :
    QObject(parent)
{
    mPImageArea = pImageArea;  // 保存图像区域指针
    mZoomedFactor = 1;         // 初始缩放因子设为1（原始大小）
}

// 调整画布尺寸
// width 目标宽度 height 目标高度 flag控制是否弹出对话框
void AdditionalTools::resizeCanvas(int width, int height, bool flag)
{
    // 如果flag为true，显示尺寸调整对话框
    if(flag)
    {
        // 创建对话框，传入当前尺寸和父窗口
        ResizeDialog resizeDialog(QSize(width, height), qobject_cast<QWidget *>(this->parent()));
        // 对话框确认处理
        if(resizeDialog.exec() == QDialog::Accepted)
        {
            QSize newSize = resizeDialog.getNewSize();
            width = newSize.width();
            height = newSize.height();
        } else {
            return; // 用户取消则直接返回
        }
    }

    // 尺寸有效性检查
    if(width < 1 || height < 1)
        return;

    // 创建新画布（带透明通道的预乘格式）
    QImage *tempImage = new QImage(width, height, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(tempImage);

    // 填充白色背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(QRect(0, 0, width, height));

    // 将原图像绘制到新画布左上角
    painter.drawImage(0, 0, *mPImageArea->getImage());
    painter.end(); // 结束绘制

    // 更新图像区域
    mPImageArea->setImage(*tempImage);

    // 调整显示区域尺寸（图像尺寸+6像素边框）
    mPImageArea->resize(mPImageArea->getImage()->rect().right() + 6,
                        mPImageArea->getImage()->rect().bottom() + 6);

    // 标记图像已被编辑
    mPImageArea->setEdited(true);
    // 清除可能存在的选区
    mPImageArea->clearSelection();
}

// 调整图像尺寸（可能保持宽度比或适应画布）
void AdditionalTools::resizeImage()
{
    // 创建对话框并传入当前图像尺寸
    ResizeDialog resizeDialog(mPImageArea->getImage()->size(), qobject_cast<QWidget *>(this->parent()));

    // 对话框确认处理
    if(resizeDialog.exec() == QDialog::Accepted)
    {
        // 按新尺寸缩放图像
        mPImageArea->setImage(mPImageArea->getImage()->scaled(resizeDialog.getNewSize()));

        // 调整显示区域
        mPImageArea->resize(mPImageArea->getImage()->rect().right() + 6,
                            mPImageArea->getImage()->rect().bottom() + 6);

        // 标记编辑状态并清除选区
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

    // 刷新显示
    mPImageArea->update();
    // 标记编辑状态并清除选区
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








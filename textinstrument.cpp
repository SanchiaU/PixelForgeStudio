#include "textinstrument.h"



// 2025/08/09--代码已经审核通过




// 构造函数
TextInstrument::TextInstrument(QObject *parent) :
    AbstractSelection(parent)     // 调用基类构造函数
{
    mText = QString();            // 初始化文本为空字符串
    mIsEdited = false;            // 标记文本未被编辑过
}

// 更新文本内容并重绘
void TextInstrument::updateText(ImageArea *imageArea, QString textString)
{
    mText = textString;                      // 存储新文本
    imageArea->setImage(mImageCopy);         // 恢复原始图像状态
    if (!mIsEdited)                          // 首次编辑时创建撤销点
    {
        makeUndoCommand(*imageArea);         // 创建撤销命令
        mIsEdited = true;                    // 标记已编辑
    }
    paint(*imageArea);                       // 绘制文本
    drawBorder(*imageArea);                  // 绘制选择边框
}

// 当前文本工具未使用
void TextInstrument::startAdjusting(ImageArea &) {}
void TextInstrument::startSelection(ImageArea &) {}
void TextInstrument::startResizing(ImageArea &) {}
void TextInstrument::startMoving(ImageArea &) {}
void TextInstrument::select(ImageArea &) {}
void TextInstrument::completeResizing(ImageArea &) {}
void TextInstrument::completeMoving(ImageArea &) {}

// 调整大小时的重绘处理
void TextInstrument::resize(ImageArea &imageArea)
{
    paint(imageArea);  // 重绘文本
}


// 移动时的重绘处理
void TextInstrument::move(ImageArea &imageArea)
{
    paint(imageArea);  // 重绘文本
}

// 完成选择操作（弹出文本输入框）
/*
 * 创建文件输入对话框
 * 连接对话框信号与文本更新柄
 * 设置对话框关闭时自动删除
 * 显示对话框
*/
void TextInstrument::completeSelection(ImageArea &imageArea)
{
    // 创建文本对话框（传入当前文本和图像区域）
    TextDialog *td = new TextDialog(mText, &imageArea);

    // 连接信号槽：
    connect(td, SIGNAL(textChanged(ImageArea *, QString)), // 文本改变信号
            this, SLOT(updateText(ImageArea *, QString))); // 更新文本槽

    connect(this, SIGNAL(sendCloseTextDialog()), // 关闭对话框信号
            td, SLOT(accept()));                // 对话框接受关闭

    connect(td, SIGNAL(canceled(ImageArea *)),  // 对话框取消信号
            this, SLOT(cancel(ImageArea *)));   // 取消操作槽

    td->setAttribute(Qt::WA_DeleteOnClose);     // 关闭时自动删除
    td->show();                                 // 显示对话框
}


// 清除文本状态
void TextInstrument::clear()
{
    mText = QString();      // 清空文本
    mIsEdited = false;      // 重置编辑标记
    emit sendCloseTextDialog(); // 通知关闭文本对话框
}

// 取消文本操作
void TextInstrument::cancel(ImageArea *imageArea)
{
    mText = QString();             // 清空文本
    clearSelection(*imageArea);    // 清除选择区域
}


// 在图像上绘制文本
/*
 * 检查选择区域有效性
 * 设置画笔颜色和字体
 * 在待定矩形内绘制文本
 * 标记图标已编辑
 * 更新显示
*/
void TextInstrument::paint(ImageArea &imageArea, bool, bool)
{
    // 检查选择区域是否有效（非零大小）
    if(mTopLeftPoint != mBottomRightPoint)
    {
        QPainter painter(imageArea.getImage());  // 在图像上创建绘图工具

        // 设置绘图属性
        painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor())); // 主颜色
        painter.setFont(DataSingleton::Instance()->getTextFont());          // 预设字体

        // 在选定矩形内绘制文本
        painter.drawText(QRect(mTopLeftPoint, mBottomRightPoint), mText);

        painter.end();  // 结束绘制

        imageArea.setEdited(true); // 标记图像已被编辑
        imageArea.update();        // 更新显示
    }
}

// 显示右键菜单
void TextInstrument::showMenu(ImageArea &imageArea)
{
    emit sendCloseTextDialog();     // 关闭现有对话框
    completeSelection(imageArea);   // 弹出新的文本编辑对话框
}



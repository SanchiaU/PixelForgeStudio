#include "textinstrument.h"



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
    mText = textString;
    imageArea->setImage(mImageCopy);
    if (!mIsEdited)
    {
        makeUndoCommand(*imageArea);
        mIsEdited = true;
    }
    paint(*imageArea);
    drawBorder(*imageArea);
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


void TextInstrument::clear()
{
    mText = QString();
    mIsEdited = false;
    emit sendCloseTextDialog();
}

void TextInstrument::cancel(ImageArea *imageArea)
{
    mText = QString();
    clearSelection(*imageArea);
}



void TextInstrument::paint(ImageArea &imageArea, bool, bool)
{
    if(mTopLeftPoint != mBottomRightPoint)
    {
        QPainter painter(imageArea.getImage());

        painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor())); // 主颜色
        painter.setFont(DataSingleton::Instance()->getTextFont());          // 预设字体

        painter.drawText(QRect(mTopLeftPoint, mBottomRightPoint), mText);

        painter.end();

        imageArea.setEdited(true);
        imageArea.update();
    }
}

void TextInstrument::showMenu(ImageArea &imageArea)
{
    emit sendCloseTextDialog();
    completeSelection(imageArea);
}



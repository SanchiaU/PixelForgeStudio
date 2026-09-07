#include "imagearea.h"


#include "datasingleton.h"    // 单例数据管理器（用于访问全局设置）
#include "undocommand.h"      // 自定义撤销命令实现

// 包含所有绘图工具的实现
#include "selectioninstrument.h"

#include "resizedialog.h"  // 调整尺寸对话框
#include "abstractinstrument.h"
#include "eraserinstrument.h"
#include "colorpickerinstrument.h"
#include "magnifierinstrument.h"
#include "pencilinstrument.h"
#include "lineinstrument.h"
#include "sprayinstrument.h"
#include "fillinstrument.h"
#include "rectangleinstrument.h"
#include "ellipseinstrument.h"
#include "curvelineinstrument.h"
#include "textinstrument.h"


// Qt 相关头文件
#include <QApplication>         // 应用程序基础功能
#include <QPainter>             // 绘图工具
#include <QFileDialog>          // 文件对话框
#include <QtCore/QDebug>        // 调试输出
#include <QMouseEvent>          // 鼠标事件处理
#include <QPaintEvent>          // 绘制事件
#include <QPrinter>             // 打印支持
#include <QPrintDialog>         // 打印对话框
#include <QtCore/QTimer>        // 定时器（用于自动保存）
#include <QImageReader>         // 图像读取
#include <QImageWriter>         // 图像写入
#include <QUndoStack>           // 撤销/重做栈
#include <QtCore/QDir>          // 目录操作
#include <QMessageBox>          // 消息提示框
#include <QClipboard>           // 剪贴板操作

//#include "negativeeffect.h"

// 构造函数
// isOpen 是否打开图像文件
// filePath 图像文件路径
// parent 父窗口指针
ImageArea::ImageArea(const bool &isOpen, const QString &filePath, QWidget *parent) :
    QWidget(parent), mIsEdited(false), mIsPaint(false), mIsResize(false) // 初始化状态标志
{
    setMouseTracking(true);  // 启用鼠标跟踪（即使没有按钮按下也接收移动事件）

    mRightButtonPressed = false;  // 鼠标右键状态初始化为未按下
    mFilePath = QString();        // 初始化文件路径为空
    makeFormatsFilters();         // 创建支持的文件格式过滤器
    initializeImage();            // 初始化图像对象
    mZoomFactor = 1;              // 默认缩放因子为1（原始大小）

    // 创建附加工具对象（颜色选择器等）
    mAdditionalTools = new AdditionalTools(this, this->parent());

    // 创建撤销栈并设置历史深度（从全局设置获取）
    mUndoStack = new QUndoStack(this);
    mUndoStack->setUndoLimit(DataSingleton::Instance()->getHistoryDepth());

    // 根据打开标志处理图像
    if(isOpen && filePath.isEmpty()) {
        open();  // 打开文件对话框让用户选择文件
    } else if(isOpen && !filePath.isEmpty()) {
        open(filePath);  // 直接打开指定路径的文件
    } else {
        // 创建新图像（默认尺寸）
        int width = DataSingleton::Instance()->getBaseSize().width();
        int height = DataSingleton::Instance()->getBaseSize().height();

        // 检查是否需要询问画布大小
        if (DataSingleton::Instance()->getIsInitialized() &&
            DataSingleton::Instance()->getIsAskCanvasSize()) {

            // 检查剪贴板中是否有图像（用作默认尺寸）
            QClipboard *globalClipboard = QApplication::clipboard();
            QImage mClipboardImage = globalClipboard->image();
            if (!mClipboardImage.isNull()) {
                width = mClipboardImage.width();
                height = mClipboardImage.height();
            }

            // 显示尺寸调整对话框
            ResizeDialog resizeDialog(QSize(width, height), this);
            if(resizeDialog.exec() != QDialog::Accepted) // 用户取消则返回
                return;

            // 获取用户设置的新尺寸
            QSize newSize = resizeDialog.getNewSize();
            width = newSize.width();
            height = newSize.height();
            mAdditionalTools->resizeCanvas(width, height, false); // 调整画布大小
            mIsEdited = false; // 新创建未被编辑
        }

        // 创建空白图像（白色背景）
        QPainter *painter = new QPainter(mImage);
        painter->fillRect(0, 0, width, height, Qt::white);
        painter->end();

        // 调整窗口大小以适应图像（额外6像素用于调整手柄）
        resize(mImage->rect().right() + 6, mImage->rect().bottom() + 6);
        mFilePath = QString("");  // 空路径表示新创建的标签页
    }

    // 设置自动保存定时器
    QTimer *autoSaveTimer = new QTimer(this);
    // 设置自动保存间隔（秒转换为毫秒）
    autoSaveTimer->setInterval(DataSingleton::Instance()->getAutoSaveInterval() * 1000);
    connect(autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave())); // 连接超时信号
    // 连接尺寸变化信号
    connect(mAdditionalTools, SIGNAL(sendNewImageSize(QSize)), this, SIGNAL(sendNewImageSize(QSize)));
    autoSaveTimer->start(); // 启动定时器

    // 创建选区工具（特殊处理）
    SelectionInstrument *selectionInstrument = new SelectionInstrument(this);
    // 连接选区工具的启用/禁用信号
    connect(selectionInstrument, SIGNAL(sendEnableCopyCutActions(bool)),
            this, SIGNAL(sendEnableCopyCutActions(bool)));
    connect(selectionInstrument, SIGNAL(sendEnableSelectionInstrument(bool)),
            this, SIGNAL(sendEnableSelectionInstrument(bool)));

    // 初始化所有工具处理器（填充nullptr）
    mInstrumentsHandlers.fill(nullptr, static_cast<int>(INSTRUMENTS_COUNT));

    // 实例化各种工具并存入处理器数组
    mInstrumentsHandlers[CURSOR] = selectionInstrument;        // 选区工具
    mInstrumentsHandlers[PEN] = new PencilInstrument(this);    // 铅笔工具
    mInstrumentsHandlers[LINE] = new LineInstrument(this);     // 直线工具
    mInstrumentsHandlers[ERASER] = new EraserInstrument(this); // 橡皮擦工具
    mInstrumentsHandlers[RECTANGLE] = new RectangleInstrument(this); // 矩形工具
    mInstrumentsHandlers[ELLIPSE] = new EllipseInstrument(this);    // 椭圆工具
    mInstrumentsHandlers[FILL] = new FillInstrument(this);     // 填充工具
    mInstrumentsHandlers[SPRAY] = new SprayInstrument(this);   // 喷枪工具
    mInstrumentsHandlers[MAGNIFIER] = new MagnifierInstrument(this); // 放大镜工具
    mInstrumentsHandlers[COLORPICKER] = new ColorpickerInstrument(this); // 取色器工具
    mInstrumentsHandlers[CURVELINE] = new CurveLineInstrument(this); // 曲线工具
    mInstrumentsHandlers[TEXT] = new TextInstrument(this);     // 文字工具

    // 初始化特效处理数组
    mEffectsHandlers.fill(nullptr,static_cast<int>(EFFECTS_COUNT));

    // 实例化各种特效并存入处理器数组
    mEffectsHandlers[NONE_EFFECT] = nullptr;
    mEffectsHandlers[GRAY] = new GrayEffect(this);
    mEffectsHandlers[NEGATIVE]=new NegativeEffect(this);





}

// 析构函数
ImageArea::~ImageArea()
{


    // 释放所有特效处理器对象（如果已实现）
    for (int i = 0; i < mEffectsHandlers.size(); ++i) {
        if (mEffectsHandlers[i]) {
            delete mEffectsHandlers[i];
            mEffectsHandlers[i] = nullptr;
        }
    }
    mEffectsHandlers.clear();



    // 注意：mUndoStack由Qt的父子对象机制自动管理，无需手动释放
}


// 初始化图像对象
void ImageArea::initializeImage()
{
    // 创建预设大小的图像（使用ARGB32预乘格式，适合透明处理）
    mImage = new QImage(DataSingleton::Instance()->getBaseSize(),
                        QImage::Format_ARGB32_Premultiplied);
}

// 打开图像文件（通过文件对话框）
void ImageArea::open()
{
    QString fileName(mFilePath);
    // 创建打开文件对话框
    QFileDialog dialog(this, tr("Open image..."), "", mOpenFilter);
    QString prevPath = DataSingleton::Instance()->getLastFilePath();

    // 设置初始目录（使用上次打开的路径或用户主目录）
    if (!prevPath.isEmpty())
        dialog.selectFile(prevPath);
    else
        dialog.setDirectory(QDir::homePath());

    // 显示对话框并获取选择的文件
    if (dialog.exec()) {
        QStringList selectedFiles = dialog.selectedFiles();
        if (!selectedFiles.isEmpty()) {
            open(selectedFiles.takeFirst());  // 打开选择的第一个文件
        }
    }
}

// 打开指定路径的图像文件
// filePath 图像文件路径
void ImageArea::open(const QString &filePath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);  // 设置等待光标（沙漏）

    if(mImage->load(filePath)) {
        // 转换为标准ARGB32预乘格式（确保兼容性）
        *mImage = mImage->convertToFormat(QImage::Format_ARGB32_Premultiplied);
        mFilePath = filePath;  // 更新当前文件路径
        DataSingleton::Instance()->setLastFilePath(filePath);  // 更新最近文件路径

        // 调整窗口大小以适应图像（额外6像素用于调整手柄）
        resize(mImage->rect().right() + 6, mImage->rect().bottom() + 6);
        QApplication::restoreOverrideCursor();  // 恢复原始光标
    } else {
        // 打开失败处理
        qDebug() << QString("Can't open file %1").arg(filePath);
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Error opening file"),
                             tr("Can't open file \"%1\".").arg(filePath));
    }
}

// 保存图像
// 保存成功返回true，否则false
bool ImageArea::save()
{
    // 如果文件路径为空（新创建未保存），则执行另存为
    if(mFilePath.isEmpty()) {
        return saveAs();
    }

    clearSelection();  // 清除选区（确保保存完整图像）

    if (!mImage->save(mFilePath)) {  // 尝试保存图像
        QMessageBox::warning(this, tr("Error saving file"),
                             tr("Can't save file \"%1\".").arg(mFilePath));
        return false;
    }

    mIsEdited = false;  // 重置编辑标志（已保存）
    return true;
}

// 另存为
// 保存成功返回true，否则false
bool ImageArea::saveAs()
{
    bool result = true;
    QString filter;
    QString fileName(mFilePath);

    clearSelection();  // 清除选区（确保保存完整图像）

    // 设置默认文件名（新图像使用"Untitled image.png"）
    if(fileName.isEmpty()) {
        fileName = QDir::homePath() + "/" + tr("Untitled image") + ".png";
    }

    // 显示保存文件对话框
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save image..."),
        fileName,
        mSaveFilter,
        &filter,
        QFileDialog::DontUseNativeDialog
        );

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(!filePath.isEmpty()) {
        QString extension;
        // 解析文件扩展名
        QString temp = filePath.split("/").last();

        // 检查文件名是否包含扩展名
        if(temp.contains('.')) {
            temp = temp.split('.').last();
            // 验证是否为支持的格式
            if(QImageWriter::supportedImageFormats().contains(temp.toLatin1()))
                extension = temp;
            else
                extension = "png";  // 未知格式默认为png
        } else {
            // 从过滤器获取扩展名
            extension = filter.split('.').last().remove(')');
            filePath += '.' + extension; // 添加扩展名
        }

        // 保存图像
        if(mImage->save(filePath, extension.toLatin1().data())) {
            mFilePath = filePath;  // 更新当前文件路径
            mIsEdited = false;     // 重置编辑标志
        } else {
            QMessageBox::warning(this, tr("Error saving file"),
                                 tr("Can't save file \"%1\".").arg(filePath));
            result = false;
        }
    }
    QApplication::restoreOverrideCursor();
    return result;
}

// 自动保存槽函数（由定时器触发）
void ImageArea::autoSave()
{
    // 检查自动保存条件：
    // 1. 图像已被编辑
    // 2. 文件路径非空（已保存过）
    // 3. 全局设置启用自动保存
    if(mIsEdited && !mFilePath.isEmpty() && DataSingleton::Instance()->getIsAutoSave()) {
        if(mImage->save(mFilePath)) {  // 尝试保存
            mIsEdited = false; // 保存成功则重置编辑标志
        }
    }
}

// 打印图像
void ImageArea::print()
{
    QPrinter *printer = new QPrinter();
    QPrintDialog *printDialog = new QPrintDialog(printer, this);

    // 显示打印对话框
    if(printDialog->exec()) {
        QPainter painter(printer);
        QRect rect = painter.viewport();
        QSize size = mImage->size();

        // 保持宽高比调整图像大小以适应打印机页面
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(mImage->rect()); // 设置绘图窗口
        painter.drawImage(0, 0, *mImage);  // 绘制图像
    }

    // 注意：实际项目中应删除printer和printDialog防止内存泄漏
}

// 调用图像大小
void ImageArea::resizeImage()
{
    mAdditionalTools->resizeImage();  // 委托给附加工具处理
    emit sendNewImageSize(mImage->size());  // 发出尺寸变化信号
}

// 旋转图像
void ImageArea::rotateImage(bool flag)
{
    mAdditionalTools->rotateImage(flag);  // 委托给附加工具处理
    emit sendNewImageSize(mImage->size());  // 发出尺寸变化信号
}

// 应用特效实现
void ImageArea::applyEffect(EffectsEnum effect)
{
    // 1. 验证特效编号
    if (effect < 0 || effect >= EFFECTS_COUNT) {
        qDebug() << "Invalid effect enum:" << effect;
        return;
    }

    // 2. 检查特效处理器是否存在
    if (effect == NONE_EFFECT || mEffectsHandlers[effect] == nullptr) {
        qDebug() << "Effect not implemented or is NONE_EFFECT:" << effect;
        return;
    }

    // 3. 检查图像是否有效
    if (!mImage || mImage->isNull()) {
        qDebug() << "Image is null, cannot apply effect";
        return;
    }

    // 4. 创建撤销命令
    makeUndoCommand();

    // 5. 获取特效处理器
    mEffectHandler = mEffectsHandlers[effect];

    // 6. 应用特效到图像
    bool success = mEffectHandler->apply(*mImage);

    if (success) {
        // 特效应用成功，标记图像已被编辑
        setEdited(true);

        // 更新显示
        update();

        qDebug() << "Effect applied successfully:" << mEffectHandler->getName();
    } else {
        qDebug() << "Failed to apply effect:" << mEffectHandler->getName();
    }

}

// 创建撤销命令（用于特效应用）
void ImageArea::makeUndoCommand()
{
    UndoCommand *cmd = new UndoCommand(mImage, *this);
    pushUndoCommand(cmd);
}



// 缩放图像
bool ImageArea::zoomImage(qreal factor)
{
    return mAdditionalTools->zoomImage(factor);     // 调用工具类方法
}

// 复制图像（复制当前选区到剪贴板）
void ImageArea::copyImage()
{
    // 获取选区工具实例
    SelectionInstrument *instrument =
        static_cast<SelectionInstrument*>(mInstrumentsHandlers.at(CURSOR));

    // 执行复制操作
    instrument->copyImage(*this);
}

// 粘贴图像（从剪贴板粘贴图像）
void ImageArea::pasteImage()
{
    // 自动切换到选区工具（如果当前不是）
    if(DataSingleton::Instance()->getInstrument() != CURSOR)
        emit sendSetInstrument(CURSOR);

    // 获取选区工具实例
    SelectionInstrument *instrument =
        static_cast<SelectionInstrument*>(mInstrumentsHandlers.at(CURSOR));

    // 执行粘贴操作
    instrument->pasteImage(*this);
}

// 剪切图像（剪切当前选区到剪贴板）
void ImageArea::cutImage()
{
    // 获取选区工具实例
    SelectionInstrument *instrument =
        static_cast<SelectionInstrument*>(mInstrumentsHandlers.at(CURSOR));

    // 执行剪切操作（复制+清除选区）
    instrument->cutImage(*this);
}


// 鼠标按下事件处理
void ImageArea::mousePressEvent(QMouseEvent *event)
{
    // 检查是否在右下角调整大小区域
    if(event->button() == Qt::LeftButton &&
        event->pos().x() < mImage->rect().right() + 6 &&
        event->pos().x() > mImage->rect().right() &&
        event->pos().y() > mImage->rect().bottom() &&
        event->pos().y() < mImage->rect().bottom() + 6) {
        mIsResize = true;  // 设置调整大小标志
        setCursor(Qt::SizeFDiagCursor);  // 设置调整大小光标
    }
    // 处理绘图工具事件
    else if(DataSingleton::Instance()->getInstrument() != NONE_INSTRUMENT) {
        // 获取当前工具处理器
        mInstrumentHandler = mInstrumentsHandlers.at(
            DataSingleton::Instance()->getInstrument()
            );
        // 调用工具特定的鼠标按下处理
        mInstrumentHandler->mousePressEvent(event, *this);
    }
}

// 鼠标移动事件处理
void ImageArea::mouseMoveEvent(QMouseEvent *event)
{
    // 获取当前工具类型和处理器
    InstrumentsEnum instrument = DataSingleton::Instance()->getInstrument();
    mInstrumentHandler = mInstrumentsHandlers.at(instrument);

    // 处理调整大小
    if(mIsResize) {
        mAdditionalTools->resizeCanvas(event->x(), event->y()); // 调整画布大小
        emit sendNewImageSize(mImage->size()); // 发送新尺寸信号
    }
    // 右下角调整区域的光标设置
    else if(event->pos().x() < mImage->rect().right() + 6 &&
             event->pos().x() > mImage->rect().right() &&
             event->pos().y() > mImage->rect().bottom() &&
             event->pos().y() < mImage->rect().bottom() + 6) {
        setCursor(Qt::SizeFDiagCursor);  // 设置调整光标
        if (qobject_cast<AbstractSelection*>(mInstrumentHandler))
            return; // 如果是选区工具则不改变其光标
    }
    // 恢复工具默认光标
    else if (!qobject_cast<AbstractSelection*>(mInstrumentHandler)) {
        restoreCursor(); // 恢复当前工具的光标
    }

    // 发送光标位置信号（用于状态栏显示）
    if(event->pos().x() < mImage->width() &&
        event->pos().y() < mImage->height()) {
        emit sendCursorPos(event->pos());
    }

    // 处理工具特定的鼠标移动
    if(instrument != NONE_INSTRUMENT) {
        mInstrumentHandler->mouseMoveEvent(event, *this);
    }
}

// 鼠标释放事件处理
void ImageArea::mouseReleaseEvent(QMouseEvent *event)
{
    // 结束调整大小
    if(mIsResize) {
        mIsResize = false;
        restoreCursor();  // 恢复默认光标
    }
    // 处理绘图工具事件
    else if(DataSingleton::Instance()->getInstrument() != NONE_INSTRUMENT) {
        // 获取当前工具处理器
        mInstrumentHandler = mInstrumentsHandlers.at(
            DataSingleton::Instance()->getInstrument()
            );
        // 调用工具特定的鼠标释放处理
        mInstrumentHandler->mouseReleaseEvent(event, *this);
    }
}

// 绘制事件处理
void ImageArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 绘制透明背景（棋盘格图案表示透明）
    painter.setBrush(QBrush(QPixmap(":media/textures/transparent.jpg")));
    painter.drawRect(0, 0,
                     mImage->rect().right() - 1,
                     mImage->rect().bottom() - 1);

    // 绘制当前图像（仅绘制需要更新的区域）
    painter.drawImage(event->rect(), *mImage, event->rect());

    // 绘制右下角黑色调整块（6x6像素）
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect(QRect(mImage->rect().right(),
                           mImage->rect().bottom(), 6, 6));
}

// 光标恢复样式（恢复当前工具对应的光标样式）
void ImageArea::restoreCursor()
{
    // 根据当前工具类型设置光标
    switch(DataSingleton::Instance()->getInstrument()) {
    case INSTRUMENTS_COUNT:
        break; // 枚举结束，不做处理
    case MAGNIFIER:  // 放大镜工具 :/new/prefix1/resourcesfile/instruction/
        setCursor(QCursor(QPixmap(":/new/prefix1/resourcesfile/instruction/cursor_loupe.png")));
        break;
    case NONE_INSTRUMENT:  // 无工具状态
        setCursor(Qt::ArrowCursor); // 默认箭头光标
        break;
    case CURSOR:  // 选区工具
        setCursor(Qt::CrossCursor); // 十字光标
        break;
    case ERASER:
    case PEN:  // 橡皮擦和铅笔工具
        drawCursor();  // 绘制自定义光标
        setCursor(QCursor(*mPixmap)); // 应用自定义光标
        break;
    case COLORPICKER:  // 取色器工具
        setCursor(QCursor(QPixmap(":/new/prefix1/resourcesfile/instruction/cursor_pipette.png")));
        break;
    case RECTANGLE:
    case ELLIPSE:
    case LINE:
    case CURVELINE:
    case TEXT: // 形状和文字工具
        setCursor(Qt::CrossCursor); // 十字光标
        break;
    case SPRAY:  // 喷枪工具
        setCursor(QCursor(QPixmap(":/new/prefix1/resourcesfile/instruction/cursor_spray.png")));
        break;
    case FILL:  // 填充工具
        setCursor(QCursor(QPixmap(":/new/prefix1/resourcesfile/instruction/cursor_fill.png")));
        break;
    }
}

// 绘制自定义光标（铅笔/橡皮檫）
void ImageArea::drawCursor()
{
    // 创建25x25像素的光标图像
    mPixmap = new QPixmap(25, 25);
    QPoint center(13, 13); // 中心点（12.5,12.5近似为13）

    // 初始化透明背景
    mPixmap->fill(QColor(0, 0, 0, 0));
    QPainter painter(mPixmap);

    // 根据工具类型绘制光标
    switch(DataSingleton::Instance()->getInstrument()) {
    case PEN: // 铅笔工具
        // 根据鼠标按键选择颜色（右键用辅色，左键用主色）
        if(mRightButtonPressed) {
            painter.setPen(QPen(DataSingleton::Instance()->getSecondaryColor()));
            painter.setBrush(QBrush(DataSingleton::Instance()->getSecondaryColor()));
        } else {
            painter.setPen(QPen(DataSingleton::Instance()->getPrimaryColor()));
            painter.setBrush(QBrush(DataSingleton::Instance()->getPrimaryColor()));
        }
        // 绘制圆形表示笔尖大小
        painter.drawEllipse(center,
                            DataSingleton::Instance()->getPenSize()/2,
                            DataSingleton::Instance()->getPenSize()/2);
        break;
    case ERASER: // 橡皮擦工具
        painter.setBrush(QBrush(Qt::white));  // 白色表示擦除
        painter.drawEllipse(center,
                            DataSingleton::Instance()->getPenSize()/2,
                            DataSingleton::Instance()->getPenSize()/2);
        break;
    default:
        break; // 其他工具不在此处理
    }

    // 绘制十字准星（黑色点）- 辅助定位
    painter.setPen(Qt::black);
    // 垂直方向点
    painter.drawPoint(13, 13); // 中心
    painter.drawPoint(13, 3);  // 上边缘
    painter.drawPoint(13, 5);  // 上边缘内
    painter.drawPoint(13, 21); // 下边缘内
    painter.drawPoint(13, 23); // 下边缘
    // 水平方向点
    painter.drawPoint(3, 13);  // 左边缘
    painter.drawPoint(5, 13);  // 左边缘内
    painter.drawPoint(21, 13); // 右边缘内
    painter.drawPoint(23, 13); // 右边缘

    // 绘制十字准星（白色点）- 提高对比度
    painter.setPen(Qt::white);
    // 中心十字
    painter.drawPoint(13, 12); // 中心上
    painter.drawPoint(13, 14); // 中心下
    painter.drawPoint(12, 13); // 中心左
    painter.drawPoint(14, 13); // 中心右
    // 垂直方向点（内层）
    painter.drawPoint(13, 4);
    painter.drawPoint(13, 6);
    painter.drawPoint(13, 20);
    painter.drawPoint(13, 22);
    // 水平方向点（内层）
    painter.drawPoint(4, 13);
    painter.drawPoint(6, 13);
    painter.drawPoint(20, 13);
    painter.drawPoint(22, 13);
}

// 创建文件格式过滤器
void ImageArea::makeFormatsFilters()
{
    // 创建打开文件过滤器
    QList<QByteArray> ba = QImageReader::supportedImageFormats();
    mOpenFilter = "All supported ("; // 初始部分

    // 添加所有支持格式的通配符
    foreach (QByteArray temp, ba)
        mOpenFilter += "*." + temp + " ";

    mOpenFilter[mOpenFilter.length() - 1] = ')'; // 替换最后一个空格为括号
    mOpenFilter += ";;"; // 过滤器分隔符

    // 添加特定格式的过滤器（按优先级）
    if(ba.contains("png"))
        mOpenFilter += "Portable Network Graphics(*.png);;";
    if(ba.contains("bmp"))
        mOpenFilter += "Windows Bitmap(*.bmp);;";
    if(ba.contains("jpg") || ba.contains("jpeg"))
        mOpenFilter += "JPEG(*.jpg *.jpeg);;";
    if(ba.contains("gif"))
        mOpenFilter += "GIF(*.gif);;";
    if(ba.contains("tif") || ba.contains("tiff"))
        mOpenFilter += "TIFF(*.tif *.tiff);;";
    if(ba.contains("ppm"))
        mOpenFilter += "PPM(*.ppm);;";
    if(ba.contains("xbm"))
        mOpenFilter += "XBM(*.xbm);;";
    if(ba.contains("xpm"))
        mOpenFilter += "XPM(*.xpm);;";
    if(ba.contains("svg"))
        mOpenFilter += "SVG(*.svg);;";

    mOpenFilter += "All Files(*.*)";  // 添加所有文件过滤器

    // 创建保存文件过滤器
    ba = QImageWriter::supportedImageFormats();
    if(ba.contains("png"))
        mSaveFilter += "Portable Network Graphics(*.png)";
    if(ba.contains("bmp"))
        mSaveFilter += ";;Windows Bitmap(*.bmp)";
    if(ba.contains("jpg") || ba.contains("jpeg"))
        mSaveFilter += ";;JPEG(*.jpg *.jpeg)";
    if(ba.contains("gif"))
        mSaveFilter += ";;GIF(*.gif)";
    if(ba.contains("tif") || ba.contains("tiff"))
        mSaveFilter += ";;TIFF(*.tif *.tiff)";
    if(ba.contains("ppm"))
        mSaveFilter += ";;PPM(*.ppm)";
    if(ba.contains("xbm"))
        mSaveFilter += ";;XBM(*.xbm)";
    if(ba.contains("xpm"))
        mSaveFilter += ";;XPM(*.xpm)";
    if(ba.contains("svg"))
        mSaveFilter += ";;SVG(*.svg)";
}

// 保存图像变更（用于撤销操作）
void ImageArea::saveImageChanges()
{
    // 遍历所有工具，找到选区工具并调用其保存方法
    foreach (AbstractInstrument* instrument, mInstrumentsHandlers) {
        if (AbstractSelection *selection = qobject_cast<AbstractSelection*>(instrument))
            selection->saveImageChanges(*this);
    }
}

// 清除选区
void ImageArea::clearSelection()
{
    // 遍历所有工具，找到选区工具并调用其清除方法
    foreach (AbstractInstrument* instrument, mInstrumentsHandlers) {
        if (AbstractSelection *selection = qobject_cast<AbstractSelection*>(instrument))
            selection->clearSelection(*this);
    }
}

// 推送撤销命令到撤销栈
void ImageArea::pushUndoCommand(UndoCommand *command)
{
    if(command != nullptr)
        mUndoStack->push(command);  // 推送到撤销栈
}

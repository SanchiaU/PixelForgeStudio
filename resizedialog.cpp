#include "resizedialog.h"

ResizeDialog::ResizeDialog(const QSize &size,QWidget *parent):
    QDialog(parent),
    mWidth(size.width()),
    mHeight(size.height()),
    mOrigWidth(size.width()),
    mOrigHeight(size.height())
{
    initializeGui();//初始化GUI界面
    //设置布局大小策略为固定大小，防止对话框改变大小
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}
void ResizeDialog::initializeGui(){
    //原始尺寸标签
    QLabel *label1 = new QLabel(tr("Original size:"));
    QLabel *label2 = new QLabel(QString("%1 * %2").arg(mWidth).arg(mHeight));
    //新尺寸标签
    QLabel *label3 = new QLabel(tr("New size:"));
    mNewSizeLabel = new QLabel(QString("%1 x %2").arg(mWidth).arg(mHeight));
    //单选按钮默认选中
    QRadioButton *pixelButton = new QRadioButton(tr("Pixels"));
    pixelButton->setChecked(true);
    connect(pixelButton,SIGNAL(clicked(bool)),this,SLOT(pixelsButtonClicked(bool)));

    mPixelWButton = new QSpinBox();
    mPixelWButton->setRange(1,9999); //设置有效范围（1-9999
    mPixelWButton->setValue(mWidth); //初始化原宽度
    connect(mPixelWButton,SIGNAL(valueChanged(int)),this,SLOT(pixelsWValueChanged(int)));
    mPixelHButton = new QSpinBox();
    mPixelHButton->setRange(1,9999); //设置有效范围（1-9999
    mPixelHButton->setValue(mHeight); //初始化原高度
    connect(mPixelHButton,SIGNAL(valueChanged(int)),this,SLOT(pixelsHValueChanged(int)));

    //像素模式标签
    QLabel *label4 = new QLabel(tr("Width:"));
    QLabel *label5 = new QLabel(tr("x Height:"));
    //创建百分比模式单选按钮
    QRadioButton *percentButton = new QRadioButton(tr("Percent"));
    connect(percentButton,SIGNAL(clicked()),this,SLOT(percentButtonClicked(bool)));
    mPercentWButton = new QSpinBox();
    mPercentWButton->setRange(1,200);
    mPercentWButton->setValue(100);
    mPercentWButton->setEnabled(false);
    connect(mPercentWButton,SIGNAL(valueChanged(int)),this,SLOT(percentWValueChanged(int)));
    mPercentHButton = new QSpinBox();
    mPercentHButton->setRange(1,200);
    mPercentHButton->setValue(100);
    mPercentHButton->setEnabled(false);
    connect(mPercentHButton,SIGNAL(valueChanged(int)),this,SLOT(percentHValueChanged(int)));

    //百分比模式标签
    QLabel *label6 = new QLabel(tr("Width:"));
    QLabel *label7 = new QLabel(tr("% x Heigth:"));
    QLabel *label8 = new QLabel(tr("%"));
    //创建宽高比比例 复选框
    mPreserveAspectBox = new QCheckBox(tr("Preserve Aspect Ratio"));

    //=============布局设置==============
    // 第一组布局：原始尺寸和新建尺寸显示
    QGridLayout *gLayout1 = new QGridLayout();
    gLayout1->addWidget(label1, 0, 0);     // 行0列0: "Original size:"
    gLayout1->addWidget(label2, 0, 1);     // 行0列1: "宽 x 高"
    gLayout1->addWidget(label3, 1, 0);     // 行1列0: "New size:"
    gLayout1->addWidget(mNewSizeLabel, 1, 1); // 行1列1: 动态尺寸显示

    // 第二组布局：尺寸调整选项
    QGridLayout *gLayout2 = new QGridLayout();
    gLayout2->addWidget(pixelButton, 0, 0);       // 行0列0: 像素单选按钮
    gLayout2->addWidget(label4, 0, 1);            // 行0列1: "Width:"
    gLayout2->addWidget(mPixelWButton, 0, 2);     // 行0列2: 像素宽度输入框
    gLayout2->addWidget(label5, 0, 3);            // 行0列3: "x Height:"
    gLayout2->addWidget(mPixelHButton, 0, 4);     // 行0列4: 像素高度输入框
    gLayout2->addWidget(percentButton, 1, 0);     // 行1列0: 百分比单选按钮
    gLayout2->addWidget(label6, 1, 1);            // 行1列1: "Width:"
    gLayout2->addWidget(mPercentWButton, 1, 2);   // 行1列2: 百分比宽度输入框
    gLayout2->addWidget(label7, 1, 3);            // 行1列3: "% x Height:"
    gLayout2->addWidget(mPercentHButton, 1, 4);   // 行1列4: 百分比高度输入框
    gLayout2->addWidget(label8, 1, 5);            // 行1列5: "%"
    gLayout2->addWidget(mPreserveAspectBox, 2, 0, 1, 6); // 行2跨6列: 宽高比复选框

    // 创建对话框按钮盒(确定/取消)
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    // 连接按钮信号
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    // 将布局放入分组框
    QGroupBox *groupBox1 = new QGroupBox();  // 尺寸信息分组框
    groupBox1->setLayout(gLayout1);
    QGroupBox *groupBox2 = new QGroupBox();  // 调整选项分组框
    groupBox2->setLayout(gLayout2);

    // 主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(groupBox1);   // 添加尺寸信息组
    mainLayout->addWidget(groupBox2);   // 添加调整选项组
    mainLayout->addWidget(buttonBox);   // 添加按钮盒
    setLayout(mainLayout);
}
//像素模式单选按钮点击处理
void ResizeDialog::pixelsButtonClicked(bool flag){
    if(flag){
        //启动像素输入框
        mPixelWButton->setEnabled(true);
        mPixelHButton->setEnabled(true);

        mPercentWButton->setEnabled(false);
        mPercentHButton->setEnabled(false);
    }
}
//百分比模式按钮点击处理
void ResizeDialog::percentButtonClicked(bool flag){
    if(flag){
        //启动像素输入框
        mPercentWButton->setEnabled(true);
        mPercentHButton->setEnabled(true);

        mPixelWButton->setEnabled(false);
        mPixelHButton->setEnabled(false);
    }
}
// 像素宽度值变化处理，value（新的宽度值（像素））
void ResizeDialog::pixelsWValueChanged(const int &value)
{
    if(mPreserveAspectBox->isChecked())  // 保持宽高比
    {
        // 计算新高度: 新高度 = 原高度 * (新宽度 / 原宽度)
        mHeight = mOrigHeight * value / mOrigWidth;
        mWidth = value;  // 更新宽度
        mPixelHButton->setValue(mHeight);// 更新高度输入框值（避免递归调用）
    }else{
        mWidth = value;  // 仅更新宽度
    }
    mNewSizeLabel->setText(QString("%1 x %2").arg(mWidth).arg(mHeight));
}

// 像素高度值变化处理，value（新的高度值（像素））
void ResizeDialog::pixelsHValueChanged(const int &value)
{
    if(mPreserveAspectBox->isChecked()){
        mWidth = mOrigWidth * value / mOrigHeight;
        mHeight = value;  // 更新高度
        mPixelWButton->setValue(mWidth);
    }else{
        mHeight = value;
    }
    mNewSizeLabel->setText(QString("%1 x %2").arg(mWidth).arg(mHeight));
}

// 百分比宽度值变化处理，value（新的宽度百分比）
void ResizeDialog::percentWValueChanged(const int &value)
{
    if(mPreserveAspectBox->isChecked())  // 保持宽高比
    {
        // 同步高度百分比值
        mPercentHButton->setValue(value);
        // 计算实际像素尺寸
        mWidth = mOrigWidth * value / 100;
        mHeight = mOrigHeight * value / 100;
    }
    else  // 不保持宽高比
    {
        // 仅计算宽度
        mWidth = mOrigWidth * value / 100;
    }
    // 更新新尺寸标签
    mNewSizeLabel->setText(QString("%1 x %2").arg(mWidth).arg(mHeight));
}
// 百分比高度值变化处理，value（新的高度百分比）
void ResizeDialog::percentHValueChanged(const int &value)
{
    if(mPreserveAspectBox->isChecked()){
        mPercentWButton->setValue(value);
        mWidth = mOrigWidth * value / 100;
        mHeight = mOrigHeight * value / 100;
    }else{
        mHeight = mOrigHeight * value / 100;
    }
    mNewSizeLabel->setText(QString("%1 x %2").arg(mWidth).arg(mHeight));
}

#include "datasingleton.h"

DataSingleton* DataSingleton::m_pInstance = 0;//初始化单例静态指针
//私有构造函数（单例模式）
//初始化所有成员变量并读取配置
DataSingleton::DataSingleton() {
    mPrimaryColor = Qt::black;
    mSecondaryColor = Qt::white;
    mPenSize = 1;
    mTextFont = QFont("Times",12);
    mCurrentInstrument = NONE_INSTRUMENT;  //默认当前工具：无
    mPreviousInstrument = NONE_INSTRUMENT; //默认先前的工具：无
    mIsInitialized = false;  //初始化状态标记：未初始化

    readSetting();
    readState();
}

DataSingleton* DataSingleton::Instance(){
    if(!m_pInstance){
        m_pInstance = new DataSingleton;
    }
    return m_pInstance;
}
// ------------------------------- 4个配置读取方法
void DataSingleton::readSetting(){
    QSettings settings;

    mBaseSize = settings.value(".Settings/BaseSize",QSize(800,500)).toSize();
    mIsAutoSave = settings.value("/Settings/IsAutoSave",false).toBool();
    mAutoSaveInterval = settings.value("/Settings/AutoSaveInteral",300).toInt();
    mHistoryDepth = settings.value("Settings/HistoryDepth",50).toInt();
    mAppLanguage = settings.value("Settings/AppLanguage","system").toString();
    mIsRestoreWindowSize = settings.value("Settings/IsRestoreWindowSize").toBool();
    mIsAskCanvasSize = settings.value("/Settings/IsAskCanvasSize",true).toBool();
    //读取文件编辑菜单快捷键
    mFileShortcuts.insert("New",settings.value("/ShortCuts/File/New",QKeySequence(QKeySequence::New)).value<QKeySequence>());
    mFileShortcuts.insert("Open",settings.value("/ShortCuts/File/Open",QKeySequence(QKeySequence::Open)).value<QKeySequence>());
    mFileShortcuts.insert("Save",settings.value("/ShortCuts/File/Save",QKeySequence(QKeySequence::Save)).value<QKeySequence>());
    mFileShortcuts.insert("SaveAs",settings.value("/ShortCuts/File/SaveAs",QKeySequence(QKeySequence::SaveAs)).value<QKeySequence>());
    mFileShortcuts.insert("Close",settings.value("/ShortCuts/File/Close",QKeySequence(QKeySequence::Close)).value<QKeySequence>());
    mFileShortcuts.insert("Print",settings.value("/ShortCuts/File/Print",QKeySequence(QKeySequence::Print)).value<QKeySequence>());
    mFileShortcuts.insert("Exit",settings.value("/ShortCuts/File/Exit",QKeySequence(QKeySequence::Quit)).value<QKeySequence>());
    //读取编辑菜单快捷键（带默认值）
    mEditShortcuts.insert("Undo",settings.value("/ShortCuts/Edit/Undo",QKeySequence(QKeySequence::Undo)).value<QKeySequence>());
    mEditShortcuts.insert("Redo",settings.value("/ShortCuts/Edit/Redo",QKeySequence(QKeySequence::Redo)).value<QKeySequence>());
    mEditShortcuts.insert("Copy",settings.value("/ShortCuts/Edit/Copy",QKeySequence(QKeySequence::Copy)).value<QKeySequence>());
    mEditShortcuts.insert("Paste",settings.value("/ShortCuts/Edit/Paste",QKeySequence(QKeySequence::Paste)).value<QKeySequence>());
    mEditShortcuts.insert("Cut",settings.value("/ShortCuts/Edit/Cut",QKeySequence(QKeySequence::Cut)).value<QKeySequence>());
    //读取指令（工具）菜单快捷键（带自定义默认值）
    mInstrumentsShortcuts.insert("Cursor",settings.value("/ShortCuts/Instruments/Cursor","Ctrl+1").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Lastic",settings.value("/ShortCuts/Instruments/Lastic","Ctrl+2").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Pipette",settings.value("/ShortCuts/Instruments/Pipette","Ctrl+3").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Loupe",settings.value("/ShortCuts/Instruments/Loupe","Ctrl+4").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Pen",settings.value("/ShortCuts/Instruments/Pen","Ctrl+5").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Line",settings.value("/ShortCuts/Instruments/Line","Ctrl+6").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Spray",settings.value("/ShortCuts/Instruments/Spray","Ctrl+7").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Fill",settings.value("/ShortCuts/Instruments/Fill","Ctrl+8").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Rect",settings.value("/ShortCuts/Instruments/Rect","Ctrl+9").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Ellipse",settings.value("/ShortCuts/Instruments/Ellipse","Ctrl+0").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Curve",settings.value("/ShortCuts/Instruments/Curve","").value<QKeySequence>());
    mInstrumentsShortcuts.insert("Text",settings.value("/ShortCuts/Instruments/Text","").value<QKeySequence>());
    //读取其他工具菜单快捷键（带默认值.）
    mToolsShortcuts.insert("ZoomIn",settings.value("/ShortCuts/Zoom/ZoomIn",QKeySequence(QKeySequence::ZoomIn)).value<QKeySequence>());
    mToolsShortcuts.insert("ZoomOut",settings.value("/ShortCuts/Zoom/ZoomOut",QKeySequence::ZoomOut).value<QKeySequence>());
}
//写入设置到持久化存储，使用QSettings保存应用程序配置
void DataSingleton::writeSettings(){
    QSettings settings; // 创建设置对象
    // 保存基本设置
    settings.setValue("/Settings/BaseSize", mBaseSize);
    settings.setValue("/Settings/IsAutoSave", mIsAutoSave);
    settings.setValue("/Settings/AutoSaveInterval", mAutoSaveInterval);
    settings.setValue("/Settings/HistoryDepth", mHistoryDepth);
    settings.setValue("/Settings/AppLanguage", mAppLanguage);
    settings.setValue("/Settings/IsRestoreWindowSize", mIsRestoreWindowSize);
    settings.setValue("/Settings/IsAskCanvasSize", mIsAskCanvasSize);
    // 保存文件菜单快捷键
    settings.setValue("/Shortcuts/File/New", mFileShortcuts["New"]);
    settings.setValue("/Shortcuts/File/Open", mFileShortcuts["Open"]);
    settings.setValue("/Shortcuts/File/Save", mFileShortcuts["Save"]);
    settings.setValue("/Shortcuts/File/SaveAs", mFileShortcuts["SaveAs"]);
    settings.setValue("/Shortcuts/File/Close", mFileShortcuts["Close"]);
    settings.setValue("/Shortcuts/File/Print", mFileShortcuts["Print"]);
    settings.setValue("/Shortcuts/File/Exit", mFileShortcuts["Exit"]);
    // 保存编辑菜单快捷键
    settings.setValue("/Shortcuts/Edit/Undo", mEditShortcuts["Undo"]);
    settings.setValue("/Shortcuts/Edit/Redo", mEditShortcuts["Redo"]);
    settings.setValue("/Shortcuts/Edit/Copy", mEditShortcuts["Copy"]);
    settings.setValue("/Shortcuts/Edit/Paste", mEditShortcuts["Paste"]);
    settings.setValue("/Shortcuts/Edit/Cut", mEditShortcuts["Cut"]);
    // 保存工具菜单快捷键
    settings.setValue("/Shortcuts/Instruments/Cursor", mInstrumentsShortcuts["Cursor"]);
    settings.setValue("/Shortcuts/Instruments/Lastic", mInstrumentsShortcuts["Lastic"]);
    settings.setValue("/Shortcuts/Instruments/Pipette", mInstrumentsShortcuts["Pipette"]);
    settings.setValue("/Shortcuts/Instruments/Loupe", mInstrumentsShortcuts["Loupe"]);
    settings.setValue("/Shortcuts/Instruments/Pen", mInstrumentsShortcuts["Pen"]);
    settings.setValue("/Shortcuts/Instruments/Line", mInstrumentsShortcuts["Line"]);
    settings.setValue("/Shortcuts/Instruments/Spray", mInstrumentsShortcuts["Spray"]);
    settings.setValue("/Shortcuts/Instruments/Fill", mInstrumentsShortcuts["Fill"]);
    settings.setValue("/Shortcuts/Instruments/Rect", mInstrumentsShortcuts["Rect"]);
    settings.setValue("/Shortcuts/Instruments/Ellipse", mInstrumentsShortcuts["Ellipse"]);
    // 保存其他工具快捷键
    settings.setValue("/Shortcuts/Tools/Zoom/ZoomIn", mToolsShortcuts["ZoomIn"]);
    settings.setValue("/Shortcuts/Tools/Zoom/ZoomOut", mToolsShortcuts["ZoomOut"]);
}
void DataSingleton::readState(){
    QSettings settings;
    // 读取窗口大小（默认值为空QSize）
    mWindowSize = settings.value("/State/WindowSize", QSize()).toSize();
}
void DataSingleton::writeState(){
    QSettings settings;
    // 仅在窗口大小有效时保存
    if (mWindowSize.isValid()) {
        settings.setValue("/State/WindowSize", mWindowSize);
    }
}

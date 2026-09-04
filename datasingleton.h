#ifndef DATASINGLETON_H
#define DATASINGLETON_H

#include <QColor>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtCore/QMap>   //映射容器
#include <QKeySequence>  //快速键序列
#include <QFont>

#include <QtCore/QSettings>

#include "easypaintenums.h"     // 包含应用枚举定义



// 单例类：用来存储程序运行所需的全局变量
// 此类管理应用程序的核心状态、配置和用户设置

class DataSingleton
{
public:
    // 获取单例实例（静态方法），返回单例对象指针
    static DataSingleton* Instance();

    // 主要颜色访问器
    inline QColor getPrimaryColor(){ return mPrimaryColor;}
    inline void setPrimaryColor(const QColor &color){mPrimaryColor=color;}
    // 次要颜色访问器
    inline QColor getSecondaryColor(){return mSecondaryColor;}
    inline void setSecondaryColor(const QColor &color){mSecondaryColor=color;}
    // 画笔尺寸访问器
    inline int getPenSize(){return mPenSize;}
    inline void setPenSize(const int &size){ mPenSize=size;}
    // 当前工具访问器（设置时会重置曲线状态）
    inline InstrumentsEnum getInstrument(){return mCurrentInstrument;}
    inline void setInstrument(const InstrumentsEnum &instrument){
        mCurrentInstrument=instrument;
        mIsResetCurve=true;
    }
    // 先前工具访问器
    inline InstrumentsEnum getPreviousInstrument(){return mPreviousInstrument;}
    inline void setPreviousInstrument(const InstrumentsEnum &instrument){mPreviousInstrument=instrument;}
    // 基础尺寸访问器
    inline QSize getBaseSize(){return mBaseSize;}
    inline void setBaseSize(const QSize &baseSize){mBaseSize=baseSize;}
    // 自动保存设置访问器
    inline bool getIsAutoSave(){return mIsAutoSave;}
    inline void setIsAutoSave(const bool &isAutoSave){ mIsAutoSave=isAutoSave;}
    // 自动保存间隔访问器
    inline int getAutoSaveInterval(){ return mAutoSaveInterval;}
    inline void setAutoSaveInterval(const int &interval){ mAutoSaveInterval=interval;}
    // 历史刻录深度访问器（撤销/重做步数）
    inline int getHistoryDepth(){ return mHistoryDepth;}
    inline void setHistoryDepth(const int &historyDepth){ mHistoryDepth=historyDepth;}
    // 应用语言访问器
    inline QString getAppLanguage(){ return mAppLanguage;}
    inline void setAppLanguage(const QString &appLanguage){ mAppLanguage=appLanguage;}
    // 窗口尺寸恢复设置访问器
    inline bool getIsRestoreWindowSize(){ return mIsRestoreWindowSize;}
    inline void setIsRestoreWindowSize(const bool &isRestoreWindowSize){mIsRestoreWindowSize=isRestoreWindowSize;}
    // 画布尺寸访问设置访问器（新建时是否询问）
    inline bool getIsAskCanvasSize(){ return mIsAskCanvasSize;}
    inline void setIsAskCanvasSize(const bool &isAskCanvasSize){ mIsAskCanvasSize=isAskCanvasSize;}
    // 初始化状态访问器
    inline bool getIsInitialized(){ return mIsInitialized;}
    inline void setIsInitialized(const bool &isInitialized=true){ mIsInitialized=isInitialized;}
    // 最后文件路径访问器
    inline QString getLastFilePath() { return mLastFilePath; }
    inline void setLastFilePath(const QString &lastFilePath) {mLastFilePath = lastFilePath;}
    // 窗口尺寸访问器
    inline QSize getWindowSize(){ return mWindowSize;}
    inline void setWindowSize(const QSize &winSize){ mWindowSize=winSize;}
    // 文本字体访问器
    inline QFont getTextFont(){ return mTextFont;}
    inline void setTextFont(const QFont textFont){ mTextFont=textFont; }
    // 文件快捷键映射访问器
    inline QMap<QString,QKeySequence> getFileShortcuts(){ return mFileShortcuts;}
    inline QKeySequence getFileShortcutByKey(const QString &key){return mFileShortcuts[key];}
    inline void setFileShortcutByKey(const QString &key,const QKeySequence &value){mFileShortcuts[key]=value;}
    // 编辑快捷键映射访问器
    inline QMap<QString,QKeySequence> getEditShortcuts(){ return mEditShortcuts; }
    inline QKeySequence getEditShortcutByKey(const QString &key){return mEditShortcuts[key];}
    inline void setEditShortcutByKey(const QString &key,const QKeySequence &value){mEditShortcuts[key]=value;}
    // 工具快捷键映射访问器
    inline QMap<QString, QKeySequence> getInstrumentsShortcuts() { return mInstrumentsShortcuts; }
    inline QKeySequence getInstrumentShortcutByKey(const QString &key) {return mInstrumentsShortcuts[key];}
    inline void setInstrumentShortcutByKey(const QString &key, const QKeySequence &value) {mInstrumentsShortcuts[key] = value;}
    // 其他工具快捷键映射访问器
    inline QMap<QString, QKeySequence> getToolsShortcuts() { return mToolsShortcuts; }
    inline QKeySequence getToolShortcutByKey(const QString &key) {return mToolsShortcuts[key];}
    inline void setToolShortcutByKey(const QString &key, const QKeySequence &value) {mToolsShortcuts[key] = value;}
    // 贝塞尔曲线工具专用重置方法
    inline void setResetCurve(bool b) { mIsResetCurve=b;}
    inline bool isResetCurve(){ return mIsResetCurve;}
    // 配置读写方法
    void readSetting();     // 从持久化存储读取设置
    void writeSettings();   // 写入设置到持久化存储
    void readState();       // 读取应用程序状态
    void writeState();      // 保存（写入）应用程序状态

private:
    // 私有构造函数（单例模式）
    DataSingleton();

    // 禁用拷贝构造函数
    DataSingleton(DataSingleton const&){}

    static DataSingleton* m_pInstance;      // 单例实例指针

    // 成员变量
    QColor mPrimaryColor,mSecondaryColor;       // 主要颜色（前景色），次要颜色（背景色）
    int mPenSize;                               // 画笔尺寸（像素）

    // 当前选中的工具；先前选中的工具
    InstrumentsEnum mCurrentInstrument,mPreviousInstrument;

    QSize mBaseSize,            // 基础尺寸（可能是默认画布尺寸）
        mWindowSize;        // 容器尺寸

    bool mIsAutoSave,           // 是否启用自动保存
        mIsRestoreWindowSize,   // 是否恢复窗口尺寸
        mIsAskCanvasSize,           // 新建时是否底部画面尺寸
        mIsInitialized;               // 是否已完成初始化

    bool mIsResetCurve;         // 贝塞尔曲线工具重置标志

    int mAutoSaveInterval,  // 自动保存间隔（秒）
        mHistoryDepth;      // 历史刻录深度（撤销步数）

    QString mAppLanguage;  // 应用程序语言
    QString mLastFilePath;  // 最后打开的文件路径
    QFont mTextFont;        // 文本工具使用的字体

    // 快捷键映射（按功能分类）
    QMap<QString,QKeySequence> mFileShortcuts,      // 文件操作快捷键
        mEditShortcuts,     // 编辑操作快捷键
        mInstrumentsShortcuts,  // 工具快捷键
        mToolsShortcuts;        // 其他工具快捷键

};

#endif // DATASINGLETON_H


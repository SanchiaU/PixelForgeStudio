#ifndef DATASINGLETON_H
#define DATASINGLETON_H

#include<QColor>
#include<QtCore/QSize>
#include<QtCore/QString>
#include<QtCore/QMap>  //映射容器
#include<QKeySequence>
#include<QFont>

#include "easypaintenums.h"
//单例类：用来存储程序运行所需的全局变量
//此类管理应用程序的核心状态、配置和用户设置

class DataSingleton
{
public:
    //获取单例实例（静态方法），返回单例对象指针
    static DataSingleton* Instance();

    //主要颜色访问器
    inline QColor getPrimaryColor(){return mPrimaryColor;}
    inline void setPrimaryColor(const QColor &color){mPrimaryColor = color;}
    //次要颜色访问器
    inline QColor getSecondaryColor(){return mSecondaryColor;}
    inline void setSecondaryColor(const QColor &color){mSecondaryColor = color;}
    //画笔尺寸访问器
    inline int getPenSize(){return mPenSize;}
    inline void setPenSize(const int &size){mPenSize = size;}
    //当前工具访问器（设置时会重置曲线状态）
    inline InstrumentsEnum getInstrument(){return mCurrentInstrument;}
    inline void setInstrument(const InstrumentsEnum &instrument){
        mCurrentInstrument = instrument;
        mIsResetCurve = true;
    }
    //先前工具访问器
    inline InstrumentsEnum getPreviousInstrument(){return mPreviousInstrument;}
    inline void setPreviousInstrument(const InstrumentsEnum &instrument){
        mPreviousInstrument = instrument;
    }
    //基础尺寸访问器
    inline QSize getBaseSize(){return mBaseSize;}
    inline void setBaseSize(const QSize &baseSize){mBaseSize = baseSize;}
    //自动保存设置访问器
    inline bool getIsAutoSave(){return mIsAutoSave;}
    inline void setIsAutoSave(const bool &isAutoSave){mIsAutoSave = isAutoSave;}
    //自动保存间隔访问器
    inline int getAutoSaveInterval(){return mAutoSaveInterval;}
    inline int setAutoSaveInterval(const int &interval){mAutoSaveInterval = interval;}
    //历史刻录深度访问器
    inline int getHistoryDepth(){return mHistoryDepth;}
    inline int setHistoryDepth(const int &historyDepth){mHistoryDepth = historyDepth;}
    //应用语言访问器
    inline QString getAppLanguage(){return mApppLanguage;}
    inline void setAppLanguage(const bool &isRestoreWindowSize){
        mIsRestoreWindowSize = isRestoreWindowSize;
    }
    //画布尺寸访问设置访问器（新建时是否询问）
    inline bool getIsAskCanvasSize(){return mIsAskVanvasSize;}
    inline bool setIsAskCanvasSize(const bool &isAskCanvasSize){mIsAskVanvasSize = isAskCanvasSize;}
    //初始化状态访问器
    inline bool getIsInitialized(){return mIsInitialized;}
    inline bool setIsInitialized(const bool &isInitialized){mIsInitialized = isInitialized;}
    //最后文件路径访问器
    inline QString getLastFilePath(){return mLastFilePath;}
    inline void SetLastFilePath(const QString &lastFilePath){mLastFilePath = lastFilePath;}
    //窗口尺寸访问器
    inline QSize getWindowSize(){return mWindowSize;}
    inline void setWindowSize(const QSize &winSize){mWindowSize = winSize;}
private:
    //私有构造函数（单例模式）
    DataSingleton();
    //禁用拷贝构造函数
    DataSingleton(DataSingleton const&){}

    static DataSingleton* m_pInstance;  //单例实例指针
    //成员变量
    QColor mPrimaryColor,mSecondaryColor;
    int mPenSize;

    InstrumentsEnum mCurrentInstrument,mPreviousInstrument;//当前选中的工具，先前选中的工具

    QSize mBaseSize,mWindowSize;  //基础尺寸（默认画布尺寸）,容器尺寸

    bool mIsAutoSave,           //是否启用自动保存
        mIsRestoreWindowSize,   //是否恢复窗口尺寸
        mIsAskVanvasSize,       //新建时是否设置了底部画面尺寸
        mIsInitialized;         //是否已经完成了初始化

    bool mIsResetCurve;  //贝塞尔曲线工具重置标志
    int mAutoSaveInterval, //自动保存间隔
        mHistoryDepth;      //历史刻度深度
    QString mApppLanguage;  //应用程序语言
    QString mLastFilePath;  //最后打开的文件路径
    QFont mTextFont;        //文本工具使用的字体17

    //快捷键映射（按功能分类）
    QMap<QString,QKeySequence>mFileShortcuts,
        mEditShorcuts,
        mInstrumentsShortcuts,
        mToolsShortcuts;
};

#endif // DATASINGLETON_H

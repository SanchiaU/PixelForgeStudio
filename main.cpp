#include "mainwindow.h"

#include <QApplication>
#include<QtCore/QStringList>
#include<QtCore/QDebug>
#include<QtCore/QFile>
#include<QRegExp>  //正则表达式


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("PixmapForgeStudio");
    app.setApplicationVersion("2.0");

    QStringList args = app.arguments(); //包含程序名和所有参数

    QRegExp rxArgHelp("--help");
    QRegExp rxArgH("-h");
    QRegExp rxArgVersion("--version");
    QRegExp rxArgV("-v");

    bool isHelp(false);
    bool isVer(false);
    QStringList filePaths;//存储要打开文件的路径

    for(int i(1);i<args.size();++i){
        if(rxArgHelp.indexIn(args.at(i))!=-1 || rxArgH.indexIn(args.at(i))!=-1){
            isHelp = true;
        }else if(rxArgVersion.indexIn(args.at(i))!=-1 || rxArgV.indexIn(args.at(i))!=-1){
            isVer = true;
        }else{
            if(QFile::exists(args.at(i))){
                filePaths.append(args.at(i));
            }else{
                //输出文件没有找到的警告
                qDebug()<<QString("File %1 not found").arg(args.at(i));
            }
        }
    }
    //处理帮助和版本请求
    if(isHelp){
        qDebug()<<"Usage:"<<app.applicationName()<<"[options][files]";
        qDebug()<<"Options";
        qDebug()<<"  -h,--help\t\tDisplay this help.";
        qDebug()<<"  -v,--version\t\tDisplay version information.";
        return 0;
    }
    if(isVer){
        qDebug()<<app.applicationName()<<app.applicationVersion();
        return 0;
    }

    //创建主窗口的实例，传入要打开的文件路径


    MainWindow window(filePaths);

    window.resize(800,500);
    window.setMinimumSize(800,500);

    window.setWindowIcon(QIcon(":/new/prefix1/resourcesfile/logo/logo.ico"));

    window.show();

    return app.exec();
}

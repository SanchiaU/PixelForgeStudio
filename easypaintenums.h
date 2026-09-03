#ifndef EASYPAINTENUMS_H
#define EASYPAINTENUMS_H

/*
工具枚举类型：定义绘图软件支持的所有绘图工具

*/
typedef enum{
    NONE_INSTRUMENT = 0,
    CURSOR,
    ERASER,
    PEN,
    LINE,
    COLORPICKER,//取色工具
    MAGNIFIER,  //放大镜
    SPRAY,  //喷枪
    FILL,  //填充工具
    RECTANGLE,
    ELLIPSE,  //椭圆工具
    CURVELINE,//曲线工具
    TEXT,
    INSTRUMENTS_COUNT//13、工具总数计数标记

}InstrumentsEnum;  //枚举类型

typedef enum{
    NONE_EFFECT = 0,
    NEGATIVE,
    GRAY,
    BINARIZATION,
    GAUSSIANBLUR,
    GAMMA,
    SHARPEN,
    CUSTOM,
    EFFECTS_COUNT // 特效总数计数标记
}EffectsEnum;


#endif // EASYPAINTENUMS_H

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
/* 特效枚举：枚举值直接作为 mEffectsHandlers 的下标 */
typedef enum{
    NONE_EFFECT = 0,           // 0: 无特效
    GRAY,                      // 1: 灰度
    NEGATIVE,                  // 2: 负片
    BINARIZATION,              // 3: 二值化（预留，未实现）
    GAUSSIANBLUR,              // 4: 高斯模糊（预留，未实现）
    EFFECTS_COUNT              // 5: 特效总数（必须是最后一个）
}EffectsEnum;
#endif // EASYPAINTENUMS_H

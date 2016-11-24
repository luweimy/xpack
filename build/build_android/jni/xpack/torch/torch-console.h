//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__CONSOLE__
#define __TORCH__CONSOLE__

#include <stdio.h>

namespace torch {
    
    class Console {
    public:
        enum class Color {
            Default,    // 默认
            Black,      // 黑色
            Red,        // 红色
            Green,      // 绿色
            Yellow,     // 黄色
            Blue,       // 蓝色
            Magenta,    // 品红
            Cyan,       // 青色
            White       // 白色
        };
        
        enum class DisplayMode {
            Default     = 0,        // 默认
            Highlight   = 1,        // 高亮
            NonBold     = 22,       // 非粗体
            Underline   = 4,        // 下划线
            NonUnderline= 24,       // 非下划线
            Blink       = 5,        // 闪烁
            NonBlink    = 25,       // 非闪烁
            ReverseDisplay   = 7,   // 反显
            NonReverseDisplay= 27,  // 非反显
        };
        
        /*
         * 设置前景(背景)颜色和显示模式，但是必须调用[Apply]设置才会生效
         * 此三者可以同时调用然后调用一次[Applay]接口
         */
        static void SetForegroundColor(Color color);
        static void SetBackgroundColor(Color color);
        static void SetDisplayMode(DisplayMode mode);
        
        /*
         * 应用格式，若设置完SetForegroundColor, SetBackgroundColor, SetDisplayMode
         * 则必须调用此接口，让设置生效
         */
        static void Apply();
        
        /*
         * 清除所有格式，回到系统默认的格式
         */
        static void Clear();
        
        /*
         * 设置颜色的输出开关，某些打印不支持shell的颜色代码，使用开关可以方便的将其关掉
         * 参数：
         *  - toggle: 颜色开关，默认为true
         */
        static void SetColorEnabled(bool toggle);
        
        /*
         * 获取颜色的编号，若无需求不需要调用此接口
         */
        static int GetForegroundColorCode(Color color);
        static int GetBackgroundColorCode(Color color);
        
    private:
        Console();
        
        static DisplayMode m_displayMode;
        static Color m_foregroundColor;
        static Color m_backgroundColor;
        
        static bool  m_colorEnabled;
        
        static int s_foregroundColorCode[9];
        static int s_backgroundColorCode[9];
        
    };
    
}

#endif

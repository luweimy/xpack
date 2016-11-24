//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include "torch-console.h"
#include "torch-string.h"

using namespace torch;

int Console::s_foregroundColorCode[9] = {0,30,31,32,33,34,35,36,37};
int Console::s_backgroundColorCode[9] = {0,40,41,42,43,44,45,46,47};

Console::DisplayMode Console::m_displayMode = DisplayMode::Default;
Console::Color Console::m_foregroundColor = Color::Default;
Console::Color Console::m_backgroundColor = Color::Default;

bool Console::m_colorEnabled = true;

void Console::SetForegroundColor(Color color)
{
    m_foregroundColor = color;
}

void Console::SetBackgroundColor(Color color)
{
    m_backgroundColor = color;
}

void Console::SetDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;
}

int Console::GetForegroundColorCode(Color color)
{
    return s_foregroundColorCode[(int)color];
}

int Console::GetBackgroundColorCode(Color color)
{
    return s_backgroundColorCode[(int)color];
}

void Console::Apply()
{
    if (!m_colorEnabled) {
        return;
    }
    
    int displayMode = (int)m_displayMode;
    int foregroundColorCode = Console::GetForegroundColorCode(m_foregroundColor);
    int backgroundColorCode = Console::GetBackgroundColorCode(m_backgroundColor);
    
    std::string output;
    
    if (displayMode > 0) {
        output += String::ToCppString(displayMode);
    }
    if (foregroundColorCode > 0) {
        output += (output.length()>0?";":"") + String::ToCppString(foregroundColorCode);
    }
    if (backgroundColorCode > 0) {
        output += (output.length()>0?";":"") + String::ToCppString(backgroundColorCode);
    }
    
    output += output.length()==0?"0":"";
    printf("\033[0m\033[%sm", output.c_str());
    
}

void Console::Clear()
{
    m_foregroundColor = Color::Default;
    m_backgroundColor = Color::Default;
    m_displayMode = DisplayMode::Default;
    Console::Apply();
}

void Console::SetColorEnabled(bool toggle)
{
    m_colorEnabled = toggle;
}


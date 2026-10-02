#pragma once

#include <QString>

namespace settings
{

enum class ThemeMode
{
    System,
    Light,
    Dark
};

enum class AccentColor
{
    Blue,
    Green,
    Red,
    Yellow
};

struct AppSettings
{
    ThemeMode theme = ThemeMode::System;
    QString fontFamily;
    AccentColor accent = AccentColor::Blue;

    static AppSettings load();
    void save() const;

    static QString defaultFontFamily();
};

}

#pragma once

#include <QString>

#include "settings/AppSettings.h"

namespace settings
{

struct ThemeManagerTokens
{
    QString accent;
    QString accentHover;
    QString accentPressed;
    QString accentSoft;
    QString bg;
    QString surface;
    QString border;
    QString text;
    QString textMuted;
    QString hover;
    QString pressed;
};

class ThemeManager
{
public:
    static void apply(const AppSettings& settings);

    static bool isDarkEffective(const AppSettings& settings);

    static ThemeManagerTokens tokens(bool dark, AccentColor accent);

    static QString accentColor(const AccentColor accent);
    static QString accentHover(const AccentColor accent);
    static QString accentPressed(const AccentColor accent);
};

}

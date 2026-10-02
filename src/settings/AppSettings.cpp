#include "settings/AppSettings.h"

#include <QFontDatabase>
#include <QSettings>
#include <QStringList>

namespace settings
{
namespace
{

QString themeToKey(ThemeMode mode)
{
    switch (mode)
    {
    case ThemeMode::Light:
        return QStringLiteral("light");
    case ThemeMode::Dark:
        return QStringLiteral("dark");
    case ThemeMode::System:
    default:
        return QStringLiteral("system");
    }
}

ThemeMode themeFromKey(const QString& key)
{
    if (key == QStringLiteral("light"))
    {
        return ThemeMode::Light;
    }
    if (key == QStringLiteral("dark"))
    {
        return ThemeMode::Dark;
    }
    return ThemeMode::System;
}

QString accentToKey(AccentColor color)
{
    switch (color)
    {
    case AccentColor::Green:
        return QStringLiteral("green");
    case AccentColor::Red:
        return QStringLiteral("red");
    case AccentColor::Yellow:
        return QStringLiteral("yellow");
    case AccentColor::Blue:
    default:
        return QStringLiteral("blue");
    }
}

AccentColor accentFromKey(const QString& key)
{
    if (key == QStringLiteral("green"))
    {
        return AccentColor::Green;
    }
    if (key == QStringLiteral("red"))
    {
        return AccentColor::Red;
    }
    if (key == QStringLiteral("yellow"))
    {
        return AccentColor::Yellow;
    }
    return AccentColor::Blue;
}

}

QString AppSettings::defaultFontFamily()
{
    const QStringList families = QFontDatabase::families();
    if (families.contains(QStringLiteral("Segoe UI Variable")))
    {
        return QStringLiteral("Segoe UI Variable");
    }
    if (families.contains(QStringLiteral("Segoe UI")))
    {
        return QStringLiteral("Segoe UI");
    }
    return QString();
}

AppSettings AppSettings::load()
{
    QSettings store;
    AppSettings result;

    result.theme = themeFromKey(store.value(QStringLiteral("ui/theme")).toString());
    result.accent = accentFromKey(store.value(QStringLiteral("ui/accentColor")).toString());
    result.fontFamily = store.value(QStringLiteral("ui/fontFamily")).toString();

    if (result.fontFamily.isEmpty())
    {
        result.fontFamily = defaultFontFamily();
    }

    return result;
}

void AppSettings::save() const
{
    QSettings store;
    store.setValue(QStringLiteral("ui/theme"), themeToKey(theme));
    store.setValue(QStringLiteral("ui/accentColor"), accentToKey(accent));
    store.setValue(QStringLiteral("ui/fontFamily"), fontFamily);
}

}

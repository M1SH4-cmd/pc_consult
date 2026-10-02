#include "settings/ThemeManager.h"

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>
#include <QString>

namespace settings
{
namespace
{

QString readResource(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

QString tokensFor(bool dark, AccentColor accent)
{
    QString base = readResource(QStringLiteral(":/styles/app.qss"));
    if (base.isEmpty())
    {
        return base;
    }

    const ThemeManagerTokens tokens = ThemeManager::tokens(dark, accent);
    base.replace(QStringLiteral("@accent"), tokens.accent);
    base.replace(QStringLiteral("@accentHover"), tokens.accentHover);
    base.replace(QStringLiteral("@accentPressed"), tokens.accentPressed);
    base.replace(QStringLiteral("@accentSoft"), tokens.accentSoft);
    base.replace(QStringLiteral("@bg"), tokens.bg);
    base.replace(QStringLiteral("@surface"), tokens.surface);
    base.replace(QStringLiteral("@border"), tokens.border);
    base.replace(QStringLiteral("@text"), tokens.text);
    base.replace(QStringLiteral("@textMuted"), tokens.textMuted);
    base.replace(QStringLiteral("@hover"), tokens.hover);
    base.replace(QStringLiteral("@pressed"), tokens.pressed);
    return base;
}

}

bool ThemeManager::isDarkEffective(const AppSettings& settings)
{
    switch (settings.theme)
    {
    case ThemeMode::Light:
        return false;
    case ThemeMode::Dark:
        return true;
    case ThemeMode::System:
    default:
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        const Qt::ColorScheme scheme = QGuiApplication::styleHints()->colorScheme();
        if (scheme == Qt::ColorScheme::Dark)
        {
            return true;
        }
        if (scheme == Qt::ColorScheme::Light)
        {
            return false;
        }
#endif
        const QPalette palette = QApplication::palette();
        const QColor window = palette.color(QPalette::Window);
        return window.lightness() < 128;
    }
    }
}

void ThemeManager::apply(const AppSettings& settings)
{
    const bool dark = isDarkEffective(settings);

    if (QApplication* app = qobject_cast<QApplication*>(QCoreApplication::instance()))
    {
        if (!settings.fontFamily.isEmpty())
        {
            QFont font = app->font();
            font.setFamily(settings.fontFamily);
            app->setFont(font);
        }

        QPalette palette = app->palette();
        if (dark)
        {
            palette.setColor(QPalette::Window, QColor("#18181B"));
            palette.setColor(QPalette::WindowText, QColor("#F4F4F5"));
            palette.setColor(QPalette::Base, QColor("#27272A"));
            palette.setColor(QPalette::Text, QColor("#F4F4F5"));
            palette.setColor(QPalette::Button, QColor("#27272A"));
            palette.setColor(QPalette::ButtonText, QColor("#F4F4F5"));
            palette.setColor(QPalette::Highlight, QColor(accentColor(settings.accent)));
            palette.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
        }
        else
        {
            palette.setColor(QPalette::Window, QColor("#F5F6F8"));
            palette.setColor(QPalette::WindowText, QColor("#18181B"));
            palette.setColor(QPalette::Base, QColor("#FFFFFF"));
            palette.setColor(QPalette::Text, QColor("#18181B"));
            palette.setColor(QPalette::Button, QColor("#FFFFFF"));
            palette.setColor(QPalette::ButtonText, QColor("#18181B"));
            palette.setColor(QPalette::Highlight, QColor(accentColor(settings.accent)));
            palette.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
        }
        app->setPalette(palette);
        app->setStyleSheet(tokensFor(dark, settings.accent));
    }
}

QString ThemeManager::accentColor(const AccentColor accent)
{
    switch (accent)
    {
    case AccentColor::Green:
        return QStringLiteral("#16A34A");
    case AccentColor::Red:
        return QStringLiteral("#DC2626");
    case AccentColor::Yellow:
        return QStringLiteral("#D6A500");
    case AccentColor::Blue:
    default:
        return QStringLiteral("#4F6BED");
    }
}

QString ThemeManager::accentHover(const AccentColor accent)
{
    switch (accent)
    {
    case AccentColor::Green:
        return QStringLiteral("#15803D");
    case AccentColor::Red:
        return QStringLiteral("#B91C1C");
    case AccentColor::Yellow:
        return QStringLiteral("#B45309");
    case AccentColor::Blue:
    default:
        return QStringLiteral("#3B5BDB");
    }
}

QString ThemeManager::accentPressed(const AccentColor accent)
{
    switch (accent)
    {
    case AccentColor::Green:
        return QStringLiteral("#166534");
    case AccentColor::Red:
        return QStringLiteral("#991B1B");
    case AccentColor::Yellow:
        return QStringLiteral("#92400E");
    case AccentColor::Blue:
    default:
        return QStringLiteral("#2F4BC7");
    }
}

ThemeManagerTokens ThemeManager::tokens(bool dark, AccentColor accent)
{
    ThemeManagerTokens t;
    t.accent = accentColor(accent);
    t.accentHover = accentHover(accent);
    t.accentPressed = accentPressed(accent);
    t.accentSoft = dark ? QStringLiteral("#1E293B") : QStringLiteral("#EEF2FF");

    if (dark)
    {
        t.bg = QStringLiteral("#18181B");
        t.surface = QStringLiteral("#27272A");
        t.border = QStringLiteral("#3F3F46");
        t.text = QStringLiteral("#F4F4F5");
        t.textMuted = QStringLiteral("#A1A1AA");
        t.hover = QStringLiteral("#3F3F46");
        t.pressed = QStringLiteral("#52525B");
    }
    else
    {
        t.bg = QStringLiteral("#F5F6F8");
        t.surface = QStringLiteral("#FFFFFF");
        t.border = QStringLiteral("#E4E4E7");
        t.text = QStringLiteral("#18181B");
        t.textMuted = QStringLiteral("#71717A");
        t.hover = QStringLiteral("#F4F4F5");
        t.pressed = QStringLiteral("#E4E4E7");
    }
    return t;
}

}

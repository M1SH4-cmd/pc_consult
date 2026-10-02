#include "settings/SettingsDialog.h"

#include <QComboBox>
#include <QFontComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include "settings/ThemeManager.h"

namespace settings
{

namespace
{

constexpr int kSidebarWidth = 200;
constexpr int kPagePadding = 32;

QToolButton* createAccentChip(const QString& name, const QString& color,
                            QWidget* parent)
{
    auto* chip = new QToolButton(parent);
    chip->setObjectName(QStringLiteral("accentChip"));
    chip->setText(name);
    chip->setProperty("color", color);
    chip->setStyleSheet(
        QStringLiteral("QToolButton#accentChip { background-color: %1; }").arg(color));
    return chip;
}

}

SettingsDialog::SettingsDialog(const core::ConsultationSession& session,
                             const core::DecisionTree& tree,
                             AppSettings& settings,
                             QWidget* parent)
    : QDialog(parent)
    , m_session(session)
    , m_tree(tree)
    , m_settings(settings)
{
    setObjectName(QStringLiteral("settingsDialog"));
    setWindowTitle(QStringLiteral("Настройки"));
    setMinimumSize(760, 500);
    resize(900, 600);

    buildUi();
    applySettings();
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::buildUi()
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(24, 24, 24, 24);
    rootLayout->setSpacing(24);

    // Main layout: sidebar + pages
    auto* mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(24);

    // Sidebar
    m_sidebar = new QFrame(this);
    m_sidebar->setObjectName(QStringLiteral("settingsSidebar"));
    m_sidebar->setFixedWidth(kSidebarWidth);

    auto* sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    m_nav = new QListWidget(m_sidebar);
    m_nav->setObjectName(QStringLiteral("settingsNav"));
    m_nav->setFrameShape(QFrame::NoFrame);
    m_nav->setSelectionMode(QAbstractItemView::SingleSelection);
    m_nav->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_nav->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_nav->addItem(QStringLiteral("Внешний вид"));
    m_nav->addItem(QStringLiteral("История ответов"));
    m_nav->setCurrentRow(0);

    sidebarLayout->addWidget(m_nav);
    mainLayout->addWidget(m_sidebar);

    // Pages
    m_pages = new QStackedWidget(this);

    // Appearance page
    auto* appearancePage = new QFrame(m_pages);
    appearancePage->setObjectName(QStringLiteral("settingsContent"));
    auto* appearanceLayout = new QVBoxLayout(appearancePage);
    appearanceLayout->setContentsMargins(kPagePadding, kPagePadding, kPagePadding, kPagePadding);
    appearanceLayout->setSpacing(24);

    auto* pageTitle = new QLabel(QStringLiteral("ВНЕШНИЙ ВИД"), appearancePage);
    pageTitle->setObjectName(QStringLiteral("pageTitle"));
    appearanceLayout->addWidget(pageTitle);

    // Theme
    auto* themeRow = new QWidget(appearancePage);
    auto* themeLayout = new QVBoxLayout(themeRow);
    themeLayout->setContentsMargins(0, 0, 0, 0);
    themeLayout->setSpacing(8);

    auto* themeLabel = new QLabel(QStringLiteral("Тема"), themeRow);
    themeLabel->setObjectName(QStringLiteral("fieldLabel"));
    themeLayout->addWidget(themeLabel);

    m_themeCombo = new QComboBox(themeRow);
    m_themeCombo->addItem(QStringLiteral("Системная"));
    m_themeCombo->addItem(QStringLiteral("Светлая"));
    m_themeCombo->addItem(QStringLiteral("Тёмная"));
    themeLayout->addWidget(m_themeCombo);

    auto* themeHint = new QLabel(
        QStringLiteral("Тема применяется сразу ко всему приложению."),
        themeRow);
    themeHint->setObjectName(QStringLiteral("fieldHint"));
    themeLayout->addWidget(themeHint);

    appearanceLayout->addWidget(themeRow);

    // Font
    auto* fontRow = new QWidget(appearancePage);
    auto* fontLayout = new QVBoxLayout(fontRow);
    fontLayout->setContentsMargins(0, 0, 0, 0);
    fontLayout->setSpacing(8);

    auto* fontLabel = new QLabel(QStringLiteral("Шрифт"), fontRow);
    fontLabel->setObjectName(QStringLiteral("fieldLabel"));
    fontLayout->addWidget(fontLabel);

    m_fontCombo = new QFontComboBox(fontRow);
    m_fontCombo->setWritingSystem(QFontDatabase::Latin);
    fontLayout->addWidget(m_fontCombo);

    auto* fontHint = new QLabel(
        QStringLiteral("Шрифт применяется сразу ко всему приложению."),
        fontRow);
    fontHint->setObjectName(QStringLiteral("fieldHint"));
    fontLayout->addWidget(fontHint);

    appearanceLayout->addWidget(fontRow);

    // Accent color
    auto* accentRow = new QWidget(appearancePage);
    auto* accentLayout = new QVBoxLayout(accentRow);
    accentLayout->setContentsMargins(0, 0, 0, 0);
    accentLayout->setSpacing(8);

    auto* accentLabel = new QLabel(QStringLiteral("Акцентный цвет"), accentRow);
    accentLabel->setObjectName(QStringLiteral("fieldLabel"));
    accentLayout->addWidget(accentLabel);

    m_accentContainer = new QWidget(accentRow);
    auto* accentGrid = new QHBoxLayout(m_accentContainer);
    accentGrid->setContentsMargins(0, 0, 0, 0);
    accentGrid->setSpacing(12);

    auto* blueChip = createAccentChip(
        QStringLiteral("Синий"), ThemeManager::accentColor(AccentColor::Blue), m_accentContainer);
    auto* greenChip = createAccentChip(
        QStringLiteral("Зелёный"), ThemeManager::accentColor(AccentColor::Green), m_accentContainer);
    auto* redChip = createAccentChip(
        QStringLiteral("Красный"), ThemeManager::accentColor(AccentColor::Red), m_accentContainer);
    auto* yellowChip = createAccentChip(
        QStringLiteral("Жёлтый"), ThemeManager::accentColor(AccentColor::Yellow), m_accentContainer);

    accentGrid->addWidget(blueChip);
    accentGrid->addWidget(greenChip);
    accentGrid->addWidget(redChip);
    accentGrid->addWidget(yellowChip);
    accentGrid->addStretch(1);

    accentLayout->addWidget(m_accentContainer);

    auto* accentHint = new QLabel(
        QStringLiteral("Акцентный цвет применяется сразу ко всему приложению."),
        accentRow);
    accentHint->setObjectName(QStringLiteral("fieldHint"));
    accentLayout->addWidget(accentHint);

    appearanceLayout->addWidget(accentRow);
    appearanceLayout->addStretch(1);

    m_pages->addWidget(appearancePage);

    // History page
    m_historyView = new HistoryView(m_session, m_tree, m_pages);
    m_pages->addWidget(m_historyView);

    mainLayout->addWidget(m_pages, 1);
    rootLayout->addLayout(mainLayout);

    // Footer
    auto* footer = new QWidget(this);
    auto* footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(0, 0, 0, 0);
    footerLayout->setSpacing(0);

    footerLayout->addStretch(1);

    m_closeButton = new QPushButton(QStringLiteral("Закрыть"), footer);
    m_closeButton->setObjectName(QStringLiteral("closeSettingsButton"));
    footerLayout->addWidget(m_closeButton);

    rootLayout->addWidget(footer);

    // Connections
    connect(m_nav, &QListWidget::currentRowChanged, this, [this](int index) {
        onNavClicked(m_nav, index);
    });
    connect(m_themeCombo, &QComboBox::currentIndexChanged, this, &SettingsDialog::onThemeChanged);
    connect(m_fontCombo, &QFontComboBox::currentFontChanged, this, &SettingsDialog::onFontChanged);
    connect(blueChip, &QToolButton::clicked, this, [this] { onAccentSelected(0); });
    connect(greenChip, &QToolButton::clicked, this, [this] { onAccentSelected(1); });
    connect(redChip, &QToolButton::clicked, this, [this] { onAccentSelected(2); });
    connect(yellowChip, &QToolButton::clicked, this, [this] { onAccentSelected(3); });
    connect(m_closeButton, &QPushButton::clicked, this, &SettingsDialog::onCloseClicked);
}

void SettingsDialog::applySettings()
{
    m_themeCombo->setCurrentIndex(static_cast<int>(m_settings.theme));
    m_fontCombo->setCurrentFont(QFont(m_settings.fontFamily));

    // Update accent chips
    for (auto* chip : m_accentContainer->findChildren<QToolButton*>("", Qt::FindDirectChildrenOnly))
    {
        const bool selected = chip->property("color").toString() ==
            ThemeManager::accentColor(m_settings.accent);
        chip->setProperty("selected", selected);
        chip->setStyle(chip->style());
    }

    ThemeManager::apply(m_settings);
}

void SettingsDialog::onNavClicked(QListWidget* list, int index)
{
    if (index >= 0 && index < m_pages->count())
    {
        m_pages->setCurrentIndex(index);
        m_currentPage = index;

        if (index == 1)
        {
            m_historyView->refresh();
        }
    }
}

void SettingsDialog::onThemeChanged(int index)
{
    if (index >= 0 && index < 3)
    {
        m_settings.theme = static_cast<ThemeMode>(index);
        m_settings.save();
        ThemeManager::apply(m_settings);
    }
}

void SettingsDialog::onFontChanged(const QFont& font)
{
    m_settings.fontFamily = font.family();
    m_settings.save();
    ThemeManager::apply(m_settings);
}

void SettingsDialog::onAccentSelected(int index)
{
    if (index >= 0 && index < 4)
    {
        m_settings.accent = static_cast<AccentColor>(index);
        m_settings.save();
        ThemeManager::apply(m_settings);
        applySettings();
    }
}

void SettingsDialog::onCloseClicked()
{
    accept();
}

}

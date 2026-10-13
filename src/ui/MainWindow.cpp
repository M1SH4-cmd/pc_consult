#include "ui/MainWindow.h"

#include <QFile>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include <utility>

#include "core/ConsultationSession.h"
#include "settings/AppSettings.h"
#include "settings/SettingsDialog.h"
#include "settings/ThemeManager.h"

namespace
{

constexpr int kFadeOutMs = 120;
constexpr int kFadeInMs = 180;

QString toQString(const std::string& utf8)
{
    return QString::fromUtf8(utf8.data(), static_cast<qsizetype>(utf8.size()));
}

QString loadStyleSheet()
{
    QFile file(QStringLiteral(":/styles/app.qss"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

}

MainWindow::MainWindow(core::ConsultationSession& session,
                         const core::DecisionTree& tree,
                         settings::AppSettings& settings,
                         QWidget* parent)
    : QMainWindow(parent)
    , m_session(session)
    , m_tree(tree)
    , m_settings(settings)
{
    setObjectName(QStringLiteral("mainWindow"));
    setWindowTitle(QStringLiteral("PC Consultant"));
    setMinimumSize(700, 500);
    resize(1200, 900);

    settings::ThemeManager::apply(m_settings);

    buildUi();
    refreshUi();
    startFadeIn();
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi()
{
    QWidget* central = new QWidget(this);
    central->setObjectName(QStringLiteral("centralWidget"));

    QVBoxLayout* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(40, 24, 40, 40);
    rootLayout->setSpacing(24);

    // Header
    QWidget* header = new QWidget(central);
    header->setObjectName(QStringLiteral("headerBar"));
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(4, 0, 4, 0);

    QLabel* title = new QLabel(QStringLiteral("PC Consultant"), header);
    title->setObjectName(QStringLiteral("appTitle"));
    headerLayout->addWidget(title);
    headerLayout->addStretch(1);
    rootLayout->addWidget(header);

    rootLayout->addStretch(1);

    // Content card
    m_card = new QFrame(central);
    m_card->setObjectName(QStringLiteral("contentCard"));
    m_card->setMaximumWidth(800);

    QVBoxLayout* cardLayout = new QVBoxLayout(m_card);
    cardLayout->setContentsMargins(36, 36, 36, 36);
    cardLayout->setSpacing(20);

    m_contextLabel = new QLabel(m_card);
    m_contextLabel->setObjectName(QStringLiteral("contextLabel"));
    m_contextLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    cardLayout->addWidget(m_contextLabel);

    m_textArea = new QScrollArea(m_card);
    m_textArea->setObjectName(QStringLiteral("textArea"));
    m_textArea->setWidgetResizable(true);
    m_textArea->setFrameShape(QFrame::NoFrame);
    m_textArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_textArea->setMinimumHeight(120);

    m_textLabel = new QLabel(m_textArea);
    m_textLabel->setObjectName(QStringLiteral("questionLabel"));
    m_textLabel->setWordWrap(true);
    m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_textLabel->setTextFormat(Qt::PlainText);
    m_textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_textArea->setWidget(m_textLabel);
    cardLayout->addWidget(m_textArea, 1);

    // Decision buttons
    QHBoxLayout* answersLayout = new QHBoxLayout();
    answersLayout->setSpacing(14);

    m_yesButton = new QPushButton(QStringLiteral("Да"), m_card);
    m_yesButton->setObjectName(QStringLiteral("yesButton"));
    m_yesButton->setCursor(Qt::PointingHandCursor);

    m_noButton = new QPushButton(QStringLiteral("Нет"), m_card);
    m_noButton->setObjectName(QStringLiteral("noButton"));
    m_noButton->setCursor(Qt::PointingHandCursor);

    answersLayout->addWidget(m_yesButton, 1);
    answersLayout->addWidget(m_noButton, 1);
    cardLayout->addLayout(answersLayout);

    // Navigation buttons
    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->setSpacing(12);

    m_backButton = new QPushButton(QStringLiteral("Назад"), m_card);
    m_backButton->setObjectName(QStringLiteral("backButton"));
    m_backButton->setCursor(Qt::PointingHandCursor);

    m_restartButton = new QPushButton(QStringLiteral("В начало"), m_card);
    m_restartButton->setObjectName(QStringLiteral("restartButton"));
    m_restartButton->setCursor(Qt::PointingHandCursor);

    navLayout->addWidget(m_backButton);
    navLayout->addStretch(1);
    navLayout->addWidget(m_restartButton);
    cardLayout->addLayout(navLayout);

    QHBoxLayout* cardRow = new QHBoxLayout();
    cardRow->setContentsMargins(0, 0, 0, 0);
    cardRow->addStretch(1);
    cardRow->addWidget(m_card, 12);
    cardRow->addStretch(1);
    rootLayout->addLayout(cardRow);

    // Settings button
    QWidget* footer = new QWidget(central);
    QHBoxLayout* footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(0, 0, 0, 0);
    footerLayout->setSpacing(0);

    footerLayout->addStretch(1);

    m_settingsButton = new QToolButton(footer);
    m_settingsButton->setObjectName(QStringLiteral("settingsButton"));
    m_settingsButton->setText(QStringLiteral("Настройки"));
    m_settingsButton->setIcon(QIcon(QStringLiteral(":/icons/settings.svg")));
    m_settingsButton->setIconSize(QSize(18, 18));
    m_settingsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_settingsButton->setCursor(Qt::PointingHandCursor);

    footerLayout->addWidget(m_settingsButton);
    footerLayout->addStretch(1);
    rootLayout->addWidget(footer);

    rootLayout->addStretch(1);
    setCentralWidget(central);

    // Fade animation setup
    m_fadeEffect = new QGraphicsOpacityEffect(m_card);
    m_fadeEffect->setOpacity(1.0);
    m_card->setGraphicsEffect(m_fadeEffect);

    m_fadeAnimation = new QPropertyAnimation(m_fadeEffect, "opacity", this);
    m_fadeAnimation->setEasingCurve(QEasingCurve::InOutCubic);

    connect(m_yesButton, &QPushButton::clicked, this, [this] {
        performTransition([this] { static_cast<void>(m_session.answerYes()); });
    });
    connect(m_noButton, &QPushButton::clicked, this, [this] {
        performTransition([this] { static_cast<void>(m_session.answerNo()); });
    });
    connect(m_backButton, &QPushButton::clicked, this, [this] {
        performTransition([this] { static_cast<void>(m_session.goBack()); });
    });
    connect(m_restartButton, &QPushButton::clicked, this, [this] {
        performTransition([this] { m_session.restart(); });
    });
    connect(m_settingsButton, &QToolButton::clicked, this, &MainWindow::openSettings);
}

void MainWindow::refreshUi()
{
    const bool question = m_session.isQuestion();

    m_contextLabel->setText(question ? QStringLiteral("ВОПРОС")
                                     : QStringLiteral("РЕКОМЕНДАЦИЯ"));
    m_textLabel->setText(toQString(m_session.currentText()));

    m_yesButton->setVisible(question);
    m_noButton->setVisible(question);

    m_backButton->setEnabled(m_session.canGoBack());
    m_restartButton->setEnabled(m_session.canGoBack());
}

void MainWindow::setInteractionEnabled(bool enabled)
{
    m_yesButton->setEnabled(enabled && m_session.isQuestion());
    m_noButton->setEnabled(enabled && m_session.isQuestion());
    m_backButton->setEnabled(enabled && m_session.canGoBack());
    m_restartButton->setEnabled(enabled && m_session.canGoBack());
}

void MainWindow::performTransition(const std::function<void()>& stateChange)
{
    if (m_transitionRunning)
    {
        return;
    }
    m_transitionRunning = true;
    setInteractionEnabled(false);

    m_fadeAnimation->stop();
    m_fadeAnimation->setDuration(kFadeOutMs);
    m_fadeAnimation->setStartValue(m_fadeEffect->opacity());
    m_fadeAnimation->setEndValue(0.0);

    disconnect(m_fadeAnimation, nullptr, this, nullptr);
    connect(m_fadeAnimation, &QPropertyAnimation::finished, this,
            [this, stateChange] {
                stateChange();
                refreshUi();

                m_fadeAnimation->setDuration(kFadeInMs);
                m_fadeAnimation->setStartValue(0.0);
                m_fadeAnimation->setEndValue(1.0);
                disconnect(m_fadeAnimation, nullptr, this, nullptr);
                connect(m_fadeAnimation, &QPropertyAnimation::finished, this, [this] {
                    m_transitionRunning = false;
                    setInteractionEnabled(true);
                });
                m_fadeAnimation->start();
            });

    m_fadeAnimation->start();
}

void MainWindow::startFadeIn()
{
    m_fadeEffect->setOpacity(0.0);
    m_fadeAnimation->stop();
    m_fadeAnimation->setDuration(kFadeInMs);
    m_fadeAnimation->setStartValue(0.0);
    m_fadeAnimation->setEndValue(1.0);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_fadeAnimation->start();
}

void MainWindow::openSettings()
{
    settings::SettingsDialog dialog(m_session, m_tree, m_settings, this);
    dialog.exec();
}

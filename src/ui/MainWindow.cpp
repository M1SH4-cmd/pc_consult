#include "ui/MainWindow.h"

#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "core/ConsultationSession.h"

namespace
{

QString toQString(const std::string& utf8)
{
    return QString::fromUtf8(utf8.data(), static_cast<qsizetype>(utf8.size()));
}

void configureButton(QPushButton* button, int minHeight)
{
    button->setMinimumHeight(minHeight);
    button->setFocusPolicy(Qt::StrongFocus);
}

}

MainWindow::MainWindow(core::ConsultationSession& session, QWidget* parent)
    : QMainWindow(parent)
    , m_session(session)
{
    setWindowTitle(QStringLiteral("PC Consultant"));
    setMinimumSize(700, 500);
    resize(900, 600);

    buildUi();
    refreshUi();
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi()
{
    QWidget* central = new QWidget(this);

    QVBoxLayout* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(48, 48, 48, 48);
    rootLayout->setSpacing(24);
    rootLayout->addStretch(1);

    QGroupBox* card = new QGroupBox(central);
    card->setObjectName(QStringLiteral("card"));
    card->setMaximumWidth(760);

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 32, 32, 32);
    cardLayout->setSpacing(24);

    m_textArea = new QScrollArea(card);
    m_textArea->setObjectName(QStringLiteral("textArea"));
    m_textArea->setWidgetResizable(true);
    m_textArea->setFrameShape(QFrame::NoFrame);
    m_textArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_textArea->setMinimumHeight(140);

    m_textLabel = new QLabel(m_textArea);
    m_textLabel->setObjectName(QStringLiteral("questionLabel"));
    m_textLabel->setWordWrap(true);
    m_textLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setTextFormat(Qt::PlainText);
    m_textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QFont questionFont = m_textLabel->font();
    questionFont.setPointSize(16);
    questionFont.setWeight(QFont::DemiBold);
    m_textLabel->setFont(questionFont);

    m_textArea->setWidget(m_textLabel);

    cardLayout->addWidget(m_textArea, 1);

    QHBoxLayout* answersLayout = new QHBoxLayout();
    answersLayout->setSpacing(16);

    m_yesButton = new QPushButton(QStringLiteral("Да"), card);
    m_yesButton->setObjectName(QStringLiteral("yesButton"));
    m_yesButton->setDefault(true);
    configureButton(m_yesButton, 48);

    m_noButton = new QPushButton(QStringLiteral("Нет"), card);
    m_noButton->setObjectName(QStringLiteral("noButton"));
    configureButton(m_noButton, 48);

    answersLayout->addWidget(m_yesButton, 1);
    answersLayout->addWidget(m_noButton, 1);
    cardLayout->addLayout(answersLayout);

    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->setSpacing(16);

    m_backButton = new QPushButton(QStringLiteral("Назад"), card);
    m_backButton->setObjectName(QStringLiteral("backButton"));
    configureButton(m_backButton, 44);

    m_restartButton = new QPushButton(QStringLiteral("Вернуться в начало"), card);
    m_restartButton->setObjectName(QStringLiteral("restartButton"));
    configureButton(m_restartButton, 44);

    navLayout->addWidget(m_backButton, 1);
    navLayout->addWidget(m_restartButton, 1);
    cardLayout->addLayout(navLayout);

    QHBoxLayout* cardRowLayout = new QHBoxLayout();
    cardRowLayout->setContentsMargins(0, 0, 0, 0);
    cardRowLayout->addStretch(1);
    cardRowLayout->addWidget(card, 10);
    cardRowLayout->addStretch(1);
    rootLayout->addLayout(cardRowLayout);

    rootLayout->addStretch(1);

    setCentralWidget(central);

    connect(m_yesButton, &QPushButton::clicked, this, [this] {
        static_cast<void>(m_session.answerYes());
        refreshUi();
    });

    connect(m_noButton, &QPushButton::clicked, this, [this] {
        static_cast<void>(m_session.answerNo());
        refreshUi();
    });

    connect(m_backButton, &QPushButton::clicked, this, [this] {
        static_cast<void>(m_session.goBack());
        refreshUi();
    });

    connect(m_restartButton, &QPushButton::clicked, this, [this] {
        m_session.restart();
        refreshUi();
    });
}

void MainWindow::refreshUi()
{
    m_textLabel->setText(toQString(m_session.currentText()));

    const bool question = m_session.isQuestion();
    m_yesButton->setVisible(question);
    m_noButton->setVisible(question);

    m_backButton->setEnabled(m_session.canGoBack());
    m_restartButton->setEnabled(true);
}

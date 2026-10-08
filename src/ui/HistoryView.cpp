#include "ui/HistoryView.h"

#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include <string>

#include "core/Answer.h"
#include "core/ConsultationSession.h"
#include "core/DecisionTree.h"
#include "core/HistoryEntry.h"
#include "core/NodeId.h"
#include "ui/HistoryConnector.h"

namespace
{

QString toQString(const std::string& s)
{
    return QString::fromUtf8(s.data(), static_cast<qsizetype>(s.size()));
}

QString answerBadgeText(core::Answer a)
{
    return a == core::Answer::Yes ? QStringLiteral("Да") : QStringLiteral("Нет");
}

}

HistoryView::HistoryView(const core::ConsultationSession& session,
                         const core::DecisionTree& tree,
                         QWidget* parent)
    : QScrollArea(parent)
    , m_session(session)
    , m_tree(tree)
{
    setObjectName(QStringLiteral("historyArea"));
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFrameShape(QFrame::NoFrame);

    m_widget = new QWidget(this);
    m_layout = new QVBoxLayout(m_widget);
    m_layout->setContentsMargins(8, 12, 24, 12);
    m_layout->setSpacing(0);
    setWidget(m_widget);

    refresh();
}

void HistoryView::refresh()
{
    while (m_layout->count())
    {
        auto* item = m_layout->takeAt(0);
        if (item->widget())
        {
            item->widget()->deleteLater();
        }
        delete item;
    }

    buildTimeline();
}

void HistoryView::buildTimeline()
{
    const auto& hist = m_session.history();
    const std::size_t n = hist.size();

    // For each history entry: create Question row Q(i+1) and Answer row A(i+1)
    for (std::size_t i = 0; i < n; ++i)
    {
        const auto& entry = hist[i];

        // Question: visual ID Q(i+1), text from entry.nodeId (the source question)
        createQuestionRow(static_cast<int>(i) + 1, entry.nodeId, i == 0, false);

        // Answer: visual ID A(i+1), text from entry.answer
        createAnswerRow(static_cast<int>(i) + 1, entry.answer, false, i == n - 1 && !m_session.isQuestion());
    }

    // Current node (after all history entries)
    if (m_session.isQuestion())
    {
        // Question Q(n+1) - current question, no answer yet
        createQuestionRow(static_cast<int>(n) + 1, m_session.currentId(), n == 0, true);
    }
    else if (m_session.isResult())
    {
        // Result R1
        createResultRow(m_session.currentId(), n == 0);
    }

    m_layout->addStretch(1);
}

void HistoryView::createQuestionRow(int visualId, const core::NodeId& nodeId, bool first, bool last)
{
    auto* row = new QWidget(m_widget);
    auto* rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 8, 0, 8);
    rowLayout->setSpacing(10);

    // Left rail: connector + marker
    auto* idLabel = new QLabel(QStringLiteral("Q%1").arg(visualId), row);
    idLabel->setObjectName(QStringLiteral("historyItemId"));
    idLabel->setProperty("marker", "question");

    auto* connector = new HistoryConnector(HistoryConnector::Marker::Question, first, last, idLabel, row);
    connector->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    rowLayout->addWidget(connector, 0, Qt::AlignTop);

    // Right: ID + text
    auto* rightCol = new QWidget(row);
    auto* rightLayout = new QVBoxLayout(rightCol);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(4);

    rightLayout->addWidget(idLabel);

    const auto* node = m_tree.find(nodeId);
    QString text;
    if (node)
    {
        text = toQString(node->text);
    }

    auto* textLabel = new QLabel(text, rightCol);
    textLabel->setObjectName(QStringLiteral("historyQuestion"));
    textLabel->setWordWrap(true);
    rightLayout->addWidget(textLabel);

    rowLayout->addWidget(rightCol, 1);
    m_layout->addWidget(row);
}

void HistoryView::createAnswerRow(int visualId, core::Answer answer, bool first, bool last)
{
    auto* row = new QWidget(m_widget);
    auto* rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 2, 0, 2);
    rowLayout->setSpacing(10);

    // Left rail: connector + marker
    auto* idLabel = new QLabel(QStringLiteral("A%1").arg(visualId), row);
    idLabel->setObjectName(QStringLiteral("historyItemId"));
    idLabel->setProperty("marker", "answer");

    auto* connector = new HistoryConnector(HistoryConnector::Marker::Answer, first, last, idLabel, row);
    connector->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    rowLayout->addWidget(connector, 0, Qt::AlignTop);

    // Right: ID + badge
    auto* rightCol = new QWidget(row);
    auto* rightLayout = new QVBoxLayout(rightCol);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(4);

    rightLayout->addWidget(idLabel);

    auto* badge = new QLabel(answerBadgeText(answer), rightCol);
    badge->setObjectName(
        answer == core::Answer::Yes
            ? QStringLiteral("historyBadgeYes")
            : QStringLiteral("historyBadgeNo"));
    rightLayout->addWidget(badge);

    rowLayout->addWidget(rightCol, 1);
    m_layout->addWidget(row);
}

void HistoryView::createResultRow(const core::NodeId& nodeId, bool first)
{
    auto* row = new QWidget(m_widget);
    auto* rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 8, 0, 8);
    rowLayout->setSpacing(10);

    // Left rail: connector + marker
    auto* idLabel = new QLabel(QStringLiteral("R1"), row);
    idLabel->setObjectName(QStringLiteral("historyItemId"));
    idLabel->setProperty("marker", "result");

    auto* connector = new HistoryConnector(HistoryConnector::Marker::Result, first, true, idLabel, row);
    connector->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    rowLayout->addWidget(connector, 0, Qt::AlignTop);

    // Right: ID + title + text
    auto* rightCol = new QWidget(row);
    auto* rightLayout = new QVBoxLayout(rightCol);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(4);

    rightLayout->addWidget(idLabel);

    auto* resultTitle = new QLabel(QStringLiteral("РЕКОМЕНДАЦИЯ"), rightCol);
    resultTitle->setObjectName(QStringLiteral("historyResultTitle"));
    rightLayout->addWidget(resultTitle);

    const auto* node = m_tree.find(nodeId);
    QString text;
    if (node)
    {
        text = toQString(node->text);
    }

    auto* textLabel = new QLabel(text, rightCol);
    textLabel->setObjectName(QStringLiteral("historyResultText"));
    textLabel->setWordWrap(true);
    rightLayout->addWidget(textLabel);

    rowLayout->addWidget(rightCol, 1);
    m_layout->addWidget(row);
}

#include "ui/HistoryView.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include <string>

#include "core/Answer.h"
#include "core/ConsultationSession.h"
#include "core/DecisionTree.h"
#include "core/HistoryEntry.h"
#include "core/NodeId.h"

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
    m_layout->setContentsMargins(0, 0, 0, 0);
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

    const auto& hist = m_session.history();
    if (hist.empty())
    {
        buildEmpty();
        return;
    }

    buildTimeline();
}

void HistoryView::buildEmpty()
{
    auto* emptyTitle = new QLabel(QStringLiteral("История пока пуста"), m_widget);
    emptyTitle->setObjectName(QStringLiteral("historyEmptyTitle"));
    emptyTitle->setAlignment(Qt::AlignCenter);

    auto* emptyHint = new QLabel(
        QStringLiteral("Ответьте хотя бы на один вопрос, "
                       "чтобы здесь появился путь консультации."),
        m_widget);
    emptyHint->setObjectName(QStringLiteral("historyEmptyHint"));
    emptyHint->setAlignment(Qt::AlignCenter);
    emptyHint->setWordWrap(true);

    m_layout->addStretch(1);
    m_layout->addWidget(emptyTitle);
    m_layout->addSpacing(8);
    m_layout->addWidget(emptyHint);
    m_layout->addStretch(1);
}

void HistoryView::buildTimeline()
{
    const auto& hist = m_session.history();

    // Build visual path from root through history to current node
    struct Step
    {
        core::NodeId nodeId;
        core::Answer answer;
        core::NodeId targetId;
        bool isCurrent = false;
        bool isResult = false;
    };

    std::vector<Step> path;
    path.reserve(static_cast<std::size_t>(hist.size()) + 1);

    core::NodeId prev = m_session.currentId();
    for (const auto& entry : hist)
    {
        Step s;
        s.nodeId = prev;
        s.answer = entry.answer;
        s.targetId = entry.targetId;
        path.push_back(std::move(s));
        prev = entry.targetId;
    }

    Step current;
    current.nodeId = prev;
    current.isCurrent = true;
    current.isResult = m_session.isResult();
    path.push_back(current);

    bool showLine = path.size() > 1;

    for (std::size_t i = 0; i < path.size(); ++i)
    {
        const auto& step = path[i];

        auto* row = new QWidget(m_widget);
        auto* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(12, 6, 12, 6);
        rowLayout->setSpacing(12);

        // Left column: connector line + node indicator
        auto* leftCol = new QWidget(row);
        auto* leftLayout = new QVBoxLayout(leftCol);
        leftLayout->setContentsMargins(0, 0, 0, 0);
        leftLayout->setSpacing(0);

        // Vertical connector above node
        if (i > 0)
        {
            auto* line = new QWidget(leftCol);
            line->setFixedWidth(2);
            line->setFixedHeight(24);
            line->setStyleSheet(
                QStringLiteral("background-color: %1; border-radius: 1px;").arg(
                    QStringLiteral("#4F6BED")));
            leftLayout->addWidget(line, 0, Qt::AlignCenter);
        }

        // Node indicator
        auto* indicator = new QLabel(leftCol);
        indicator->setFixedSize(16, 16);
        if (step.isResult)
        {
            indicator->setText(QStringLiteral("◆"));
            indicator->setStyleSheet(
                QStringLiteral("color: #4F6BED; font-size: 11px;"));
        }
        else if (step.isCurrent)
        {
            indicator->setText(QStringLiteral("●"));
            indicator->setStyleSheet(
                QStringLiteral("color: #4F6BED; font-size: 13px; font-weight: bold;"));
        }
        else
        {
            indicator->setText(QStringLiteral("●"));
            indicator->setStyleSheet(
                QStringLiteral("color: #A1A1AA; font-size: 10px;"));
        }
        indicator->setAlignment(Qt::AlignCenter);
        leftLayout->addWidget(indicator, 0, Qt::AlignCenter);

        // Vertical connector below node
        if (i < path.size() - 1)
        {
            auto* line2 = new QWidget(leftCol);
            line2->setFixedWidth(2);
            line2->setFixedHeight(24);
            line2->setStyleSheet(
                QStringLiteral("background-color: %1; border-radius: 1px;").arg(
                    QStringLiteral("#4F6BED")));
            leftLayout->addWidget(line2, 1, Qt::AlignCenter);
        }

        rowLayout->addWidget(leftCol, 0, Qt::AlignTop);

        // Right column: question text + answer badge
        auto* rightCol = new QWidget(row);
        auto* rightLayout = new QVBoxLayout(rightCol);
        rightLayout->setContentsMargins(0, 0, 0, 0);
        rightLayout->setSpacing(4);

        const auto* node = m_tree.find(step.nodeId);
        std::string text;
        if (node && node->isQuestion())
        {
            text = node->text;
        }
        else if (node && node->isResult())
        {
            text = node->text;
        }

        auto* textLabel = new QLabel(toQString(text), rightCol);
        if (step.isResult)
        {
            textLabel->setObjectName(QStringLiteral("historyResultText"));
            textLabel->setWordWrap(true);
        }
        else
        {
            textLabel->setObjectName(QStringLiteral("historyQuestion"));
            textLabel->setWordWrap(true);
        }
        rightLayout->addWidget(textLabel);

        if (!step.isResult && !step.isCurrent && step.answer != core::Answer::Yes)
        {
            auto* badge = new QLabel(
                answerBadgeText(step.answer), rightCol);
            badge->setObjectName(
                step.answer == core::Answer::Yes
                    ? QStringLiteral("historyBadgeYes")
                    : QStringLiteral("historyBadgeNo"));
            rightLayout->addWidget(badge);
        }
        else if (step.isCurrent && !step.isResult)
        {
            auto* badge = new QLabel(
                answerBadgeText(step.answer), rightCol);
            badge->setObjectName(
                step.answer == core::Answer::Yes
                    ? QStringLiteral("historyBadgeYes")
                    : QStringLiteral("historyBadgeNo"));
            rightLayout->addWidget(badge);
        }
        else if (step.isResult)
        {
            auto* resultTitle = new QLabel(
                QStringLiteral("РЕКОМЕНДАЦИЯ"), rightCol);
            resultTitle->setObjectName(QStringLiteral("historyResultTitle"));
            rightLayout->addWidget(resultTitle);
        }

        rowLayout->addWidget(rightCol, 1);
        m_layout->addWidget(row);
    }

    m_layout->addStretch(1);
}

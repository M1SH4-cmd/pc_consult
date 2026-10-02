#include "core/ConsultationSession.h"

#include <utility>

namespace core
{

ConsultationSession::ConsultationSession(const DecisionTree& tree)
    : m_tree(tree)
    , m_currentId(tree.rootId())
{
}

const DecisionNode& ConsultationSession::currentNode() const
{
    return m_tree.at(m_currentId);
}

const std::string& ConsultationSession::currentText() const
{
    return currentNode().text;
}

bool ConsultationSession::isQuestion() const
{
    return currentNode().isQuestion();
}

bool ConsultationSession::isResult() const
{
    return currentNode().isResult();
}

bool ConsultationSession::answer(Answer answer)
{
    const DecisionNode* node = m_tree.find(m_currentId);
    if (node == nullptr || !node->isQuestion())
    {
        return false;
    }

    const std::optional<NodeId>& target = node->target(answer);
    if (!target.has_value() || target->empty() || !m_tree.contains(*target))
    {
        return false;
    }

    HistoryEntry entry;
    entry.nodeId = m_currentId;
    entry.answer = answer;
    entry.targetId = *target;
    m_history.push_back(std::move(entry));

    m_currentId = *target;
    return true;
}

bool ConsultationSession::goBack()
{
    if (m_history.empty())
    {
        return false;
    }

    m_currentId = m_history.back().nodeId;
    m_history.pop_back();
    return true;
}

void ConsultationSession::restart()
{
    m_currentId = m_tree.rootId();
    m_history.clear();
}

}

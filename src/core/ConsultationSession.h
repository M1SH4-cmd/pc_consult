#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "core/Answer.h"
#include "core/DecisionNode.h"
#include "core/DecisionTree.h"
#include "core/HistoryEntry.h"
#include "core/NodeId.h"

namespace core
{

class ConsultationSession
{
public:
    explicit ConsultationSession(const DecisionTree& tree);

    ConsultationSession(const ConsultationSession&) = default;
    ConsultationSession& operator=(const ConsultationSession&) = delete;

    [[nodiscard]] const NodeId& currentId() const noexcept
    {
        return m_currentId;
    }

    [[nodiscard]] const DecisionNode& currentNode() const;

    [[nodiscard]] const std::string& currentText() const;

    [[nodiscard]] bool isQuestion() const;
    [[nodiscard]] bool isResult() const;

    [[nodiscard]] bool canGoBack() const noexcept
    {
        return !m_history.empty();
    }

    [[nodiscard]] std::size_t historySize() const noexcept
    {
        return m_history.size();
    }

    [[nodiscard]] const std::vector<HistoryEntry>& history() const noexcept
    {
        return m_history;
    }

    [[nodiscard]] bool answer(Answer answer);

    [[nodiscard]] bool answerYes()
    {
        return answer(Answer::Yes);
    }

    [[nodiscard]] bool answerNo()
    {
        return answer(Answer::No);
    }

    [[nodiscard]] bool goBack();

    void restart();

private:
    const DecisionTree& m_tree;
    NodeId m_currentId;
    std::vector<HistoryEntry> m_history;
};

}

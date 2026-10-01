#pragma once

#include <optional>
#include <string>

#include "core/Answer.h"
#include "core/NodeId.h"
#include "core/NodeKind.h"

namespace core
{

struct DecisionNode
{
    NodeId id;
    NodeKind kind = NodeKind::Question;
    std::string text;
    std::optional<NodeId> yesTarget;
    std::optional<NodeId> noTarget;

    [[nodiscard]] bool isQuestion() const noexcept
    {
        return kind == NodeKind::Question;
    }

    [[nodiscard]] bool isResult() const noexcept
    {
        return kind == NodeKind::Result;
    }

    [[nodiscard]] bool hasTarget(Answer answer) const noexcept
    {
        return target(answer).has_value();
    }

    [[nodiscard]] const std::optional<NodeId>& target(Answer answer) const noexcept
    {
        return (answer == Answer::Yes) ? yesTarget : noTarget;
    }
};

}

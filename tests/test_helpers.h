#pragma once

#include <utility>
#include <vector>

#include "core/DecisionNode.h"
#include "core/DecisionTree.h"

namespace testing_helpers
{

inline core::DecisionNode makeQuestion(const core::NodeId& id, std::string text,
                                       std::optional<core::NodeId> yes = std::nullopt,
                                       std::optional<core::NodeId> no = std::nullopt)
{
    core::DecisionNode node;
    node.id = id;
    node.kind = core::NodeKind::Question;
    node.text = std::move(text);
    node.yesTarget = std::move(yes);
    node.noTarget = std::move(no);
    return node;
}

inline core::DecisionNode makeResult(const core::NodeId& id, std::string text)
{
    core::DecisionNode node;
    node.id = id;
    node.kind = core::NodeKind::Result;
    node.text = std::move(text);
    return node;
}

inline core::DecisionTree makeTree(const core::NodeId& rootId,
                                   std::vector<core::DecisionNode> nodes)
{
    core::DecisionTree::NodeMap map;
    for (auto& node : nodes)
    {
        map.emplace(node.id, std::move(node));
    }
    return core::DecisionTree(rootId, std::move(map));
}

}

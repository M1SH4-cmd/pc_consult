#pragma once

#include <cstddef>
#include <unordered_map>

#include "core/DecisionNode.h"
#include "core/NodeId.h"

namespace core
{

class DecisionTree
{
public:
    using NodeMap = std::unordered_map<NodeId, DecisionNode>;

    DecisionTree(NodeId rootId, NodeMap nodes);

    [[nodiscard]] const NodeId& rootId() const noexcept
    {
        return m_rootId;
    }

    [[nodiscard]] const DecisionNode* find(const NodeId& id) const noexcept;

    [[nodiscard]] const DecisionNode& at(const NodeId& id) const;

    [[nodiscard]] bool contains(const NodeId& id) const noexcept;

    [[nodiscard]] std::size_t size() const noexcept
    {
        return m_nodes.size();
    }

    [[nodiscard]] const NodeMap& nodes() const noexcept
    {
        return m_nodes;
    }

private:
    NodeId m_rootId;
    NodeMap m_nodes;
};

}

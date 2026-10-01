#include "core/DecisionTree.h"

#include <stdexcept>
#include <utility>

namespace core
{

DecisionTree::DecisionTree(NodeId rootId, NodeMap nodes)
    : m_rootId(std::move(rootId))
    , m_nodes(std::move(nodes))
{
    if (m_nodes.find(m_rootId) == m_nodes.end())
    {
        throw std::invalid_argument("DecisionTree: root id is not present in nodes");
    }
}

const DecisionNode* DecisionTree::find(const NodeId& id) const noexcept
{
    const auto it = m_nodes.find(id);
    return (it == m_nodes.end()) ? nullptr : &it->second;
}

const DecisionNode& DecisionTree::at(const NodeId& id) const
{
    return m_nodes.at(id);
}

bool DecisionTree::contains(const NodeId& id) const noexcept
{
    return m_nodes.find(id) != m_nodes.end();
}

}

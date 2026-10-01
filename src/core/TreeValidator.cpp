#include "core/TreeValidator.h"

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace core
{
namespace
{

enum class VisitColor
{
    White,
    Gray,
    Black
};

class CycleDetector
{
public:
    explicit CycleDetector(const DecisionTree& tree)
        : m_tree(tree)
    {
    }

    bool hasCycle()
    {
        for (const auto& entry : m_tree.nodes())
        {
            if (colorOf(entry.first) == VisitColor::White)
            {
                if (visit(entry.first))
                {
                    return true;
                }
            }
        }
        return false;
    }

private:
    VisitColor colorOf(const NodeId& id)
    {
        const auto it = m_colors.find(id);
        return (it == m_colors.end()) ? VisitColor::White : it->second;
    }

    bool visit(const NodeId& id)
    {
        m_colors[id] = VisitColor::Gray;

        const DecisionNode* node = m_tree.find(id);
        if (node != nullptr)
        {
            for (Answer answer : {Answer::Yes, Answer::No})
            {
                const std::optional<NodeId>& target = node->target(answer);
                if (!target.has_value() || m_tree.find(*target) == nullptr)
                {
                    continue;
                }

                const VisitColor color = colorOf(*target);
                if (color == VisitColor::Gray)
                {
                    return true;
                }
                if (color == VisitColor::White && visit(*target))
                {
                    return true;
                }
            }
        }

        m_colors[id] = VisitColor::Black;
        return false;
    }

    const DecisionTree& m_tree;
    std::unordered_map<NodeId, VisitColor> m_colors;
};

void validateQuestion(const DecisionTree& tree, const DecisionNode& node,
                      const std::string& where, ValidationResult& result)
{
    const std::optional<NodeId>& yes = node.yesTarget;
    if (!yes.has_value())
    {
        result.errors.push_back(where + " is missing target for answer 'yes'");
    }
    else if (yes->empty())
    {
        result.errors.push_back(where + " has empty target for answer 'yes'");
    }
    else if (tree.find(*yes) == nullptr)
    {
        result.errors.push_back(where + " target for answer 'yes' ('" + *yes
                                + "') does not exist");
    }

    const std::optional<NodeId>& no = node.noTarget;
    if (!no.has_value())
    {
        result.errors.push_back(where + " is missing target for answer 'no'");
    }
    else if (no->empty())
    {
        result.errors.push_back(where + " has empty target for answer 'no'");
    }
    else if (tree.find(*no) == nullptr)
    {
        result.errors.push_back(where + " target for answer 'no' ('" + *no
                                + "') does not exist");
    }
}

void collectReachable(const DecisionTree& tree, std::unordered_set<NodeId>& reachable)
{
    std::vector<NodeId> stack{tree.rootId()};
    while (!stack.empty())
    {
        const NodeId id = stack.back();
        stack.pop_back();
        if (!reachable.insert(id).second)
        {
            continue;
        }

        const DecisionNode* node = tree.find(id);
        if (node == nullptr)
        {
            continue;
        }
        for (Answer answer : {Answer::Yes, Answer::No})
        {
            const std::optional<NodeId>& target = node->target(answer);
            if (target.has_value() && tree.find(*target) != nullptr
                && reachable.find(*target) == reachable.end())
            {
                stack.push_back(*target);
            }
        }
    }
}

}

ValidationResult TreeValidator::validate(const DecisionTree& tree)
{
    ValidationResult result;

    const DecisionNode* root = tree.find(tree.rootId());
    if (root == nullptr)
    {
        result.errors.push_back("Root '" + tree.rootId() + "' does not exist");
        return result;
    }
    if (!root->isQuestion())
    {
        result.errors.push_back("Root '" + tree.rootId() + "' must be a question");
    }

    for (const auto& entry : tree.nodes())
    {
        const DecisionNode& node = entry.second;
        const std::string where =
            (node.kind == NodeKind::Question ? "Question " : "Result ") + node.id;

        if (node.text.empty())
        {
            result.errors.push_back(where + " has empty text");
        }

        if (node.isQuestion())
        {
            validateQuestion(tree, node, where, result);
        }
        else if (node.isResult())
        {
            if (node.yesTarget.has_value())
            {
                result.errors.push_back(where + " must not have target for answer 'yes'");
            }
            if (node.noTarget.has_value())
            {
                result.errors.push_back(where + " must not have target for answer 'no'");
            }
        }
    }

    std::unordered_set<NodeId> reachable;
    collectReachable(tree, reachable);
    for (const auto& entry : tree.nodes())
    {
        if (reachable.find(entry.first) == reachable.end())
        {
            result.errors.push_back("Node '" + entry.first
                                    + "' is not reachable from the root");
        }
    }

    if (CycleDetector(tree).hasCycle())
    {
        result.errors.push_back("Tree contains a cycle");
    }

    return result;
}

}

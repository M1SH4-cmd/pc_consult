#pragma once

#include <optional>
#include <string>

#include "core/DecisionTree.h"

namespace core
{

struct TreeLoadError
{
    std::string message;
};

struct TreeLoadResult
{
    std::optional<DecisionTree> tree;
    std::optional<TreeLoadError> error;

    [[nodiscard]] bool ok() const noexcept
    {
        return tree.has_value();
    }
};

class TreeDataLoader
{
public:
    [[nodiscard]] static TreeLoadResult loadFromJson(const std::string& jsonUtf8);

    [[nodiscard]] static TreeLoadResult loadFromResource(const std::string& resourcePath);
};

}

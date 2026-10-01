#pragma once

#include <string>
#include <vector>

#include "core/DecisionTree.h"

namespace core
{

struct ValidationResult
{
    std::vector<std::string> errors;

    [[nodiscard]] bool valid() const noexcept
    {
        return errors.empty();
    }
};

class TreeValidator
{
public:
    [[nodiscard]] static ValidationResult validate(const DecisionTree& tree);
};

}

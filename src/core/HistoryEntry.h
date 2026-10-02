#pragma once

#include "core/Answer.h"
#include "core/NodeId.h"

namespace core
{

struct HistoryEntry
{
    NodeId nodeId;
    Answer answer = Answer::Yes;
    NodeId targetId;
};

}

#include <branching/debug_branching.h>

namespace PaceVC {

void EmptyLogger::onBoundLeaf(int)
{ }

void EmptyLogger::onDisconnectedGraph(const std::vector<int>&)
{ }

void EmptyLogger::onInnerVertex(int, int, int)
{ }

void EmptyLogger::onSolution(int)
{ }

void EmptyLogger::toChild()
{ }

void EmptyLogger::retFromChild()
{ }

}

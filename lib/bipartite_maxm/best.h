#pragma once

#include <bipartite_maxm/augmenting_path.h>
#include <bipartite_maxm/matching.h>
#include <bipartite_maxm/min_vc.h>

namespace PaceVC {

using BestMatchingFinder = HopcroftKarpMatchingFinder;
using BestMinVCFinder = MinVCFinder<HopcroftKarpMatchingFinder, KuhnAugmentingPathFinder>;

}

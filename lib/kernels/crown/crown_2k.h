#pragma once

#include <graph/graph.h>

#include <memory>

namespace PaceVC {
namespace Kernels {

class Crown2kKernel {
public:
    Crown2kKernel(Graph& g);
    ~Crown2kKernel();

    void reduce();

    int lowerBound = 0;
private:
    struct TImpl;
    std::unique_ptr<TImpl> impl_;
};

}
}

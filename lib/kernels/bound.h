#pragma once

#include <type_traits>

namespace PaceVC {
namespace Kernels {

namespace {

template<class K, class U = int>
struct IsBoundedKernel : std::false_type {};

template<class K>
struct IsBoundedKernel<K, decltype((void)K::bound, 0)> : std::true_type {};

template<class K>
typename std::enable_if<IsBoundedKernel<K>::value, decltype(K::bound)>::type getKernelBound(const K& kernel) {
    return kernel.bound;
}

template<class K>
typename std::enable_if<!IsBoundedKernel<K>::value, int>::type getKernelBound(const K&) {
    return -1;
}

}

}
}

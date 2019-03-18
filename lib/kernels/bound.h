#pragma once

#include <type_traits>

namespace PaceVC {
namespace Kernels {

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

template<class K, class U = int>
struct IsLowerBoundedKernel : std::false_type {};

template<class K>
struct IsLowerBoundedKernel<K, decltype((void)K::lowerBound, 0)> : std::true_type {};

template<class K>
typename std::enable_if<IsLowerBoundedKernel<K>::value, decltype(K::lowerBound)>::type getKernelLowerBound(const K& kernel) {
    return kernel.lowerBound;
}

template<class K>
typename std::enable_if<!IsLowerBoundedKernel<K>::value, int>::type getKernelLowerBound(const K&) {
    return 0;
}

template<class K, class U = int>
struct IsSpanningKernel : std::false_type {};

template<class K>
struct IsSpanningKernel<K, decltype((void)K::spans, 0)> : std::true_type {};

template<class K>
typename std::enable_if<IsSpanningKernel<K>::value, decltype(K::spans)>::type getKernelSpans(const K& kernel) {
    return kernel.spans;
}

template<class K>
typename std::enable_if<!IsSpanningKernel<K>::value, int>::type getKernelSpans(const K&) {
    return 0;
}

}
}

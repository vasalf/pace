#include <gtest/gtest.h>

#include <util/cow.h>

using namespace PaceVC;

namespace {

struct NonCopyable {
    NonCopyable() = default;

    NonCopyable(const NonCopyable&) {
        throw std::runtime_error("Attempt to copy an uncopyable structure");
    }

    NonCopyable& operator=(const NonCopyable&) {
        throw std::runtime_error("Attempt to copy an uncopyable structure");
    }

    void constMethod() const {}

    void nonConstMethod() {}
};

}

TEST(TestCow, testExactlyOneCopy) {
    auto ptr = makeCow<NonCopyable>();
    auto sptr = ptr;
    const auto& sptrr = sptr;
    auto tptr = sptr;
    sptrr->constMethod();
    auto fptr = sptr;
}

TEST(TestCow, testCopyOnNonConstMethod) {
    auto ptr = makeCow<NonCopyable>();
    auto sptr = ptr;
    ASSERT_THROW(ptr->nonConstMethod(), std::runtime_error);
}

TEST(TestCow, testNoCopyInCaseOfOneInstance) {
    auto ptr = makeCow<NonCopyable>();
    
    {
        auto sptr = ptr;
    }

    ptr->nonConstMethod();
}

namespace {

struct Holder {
    CowPtr<NonCopyable> x;

    void constMethod() const {
        x->constMethod();
    }
};

}

TEST(TestCow, testNoCopyInInnerObject) {
    Holder().constMethod();
}

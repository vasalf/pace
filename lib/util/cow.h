#pragma once

#include <memory>

namespace PaceVC {

template<class W>
class CowPtr {
public:
    CowPtr() = default;
    ~CowPtr() = default;
    
    CowPtr(const CowPtr<W>&) = default;
    CowPtr<W>& operator=(const CowPtr<W>&) = default;
    CowPtr(CowPtr<W>&&) = default;
    CowPtr<W>& operator=(CowPtr<W>&&) = default;

    const W& operator*() const {
        return *wrapped_;   
    }

    W& operator*() {
        if (wrapped_.use_count() > 1) {
            copy();
        }
        return *wrapped_;
    }

    const W* operator->() const {
        return wrapped_.get();
    }

    W* operator->() {
        if (wrapped_.use_count() > 1) {
            copy();
        }
        return wrapped_.get();
    }

private:
    void copy() {
        wrapped_ = std::make_shared<W>(*wrapped_);
    }

    std::shared_ptr<W> wrapped_;

    template<class U, class... Args>
    friend CowPtr<U> makeCow(Args&&...);
};

template<class W, class... Args>
CowPtr<W> makeCow(Args&&... args) {
    CowPtr<W> ret;
    ret.wrapped_ = std::make_shared<W>(std::forward<Args>(args)...);
    return ret; 
}

}

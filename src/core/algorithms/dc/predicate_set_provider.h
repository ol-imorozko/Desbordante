#pragma once

#include <memory>
#include "predicate_set.h"

namespace model {

class PredicateSetProvider {
private:
    static std::shared_ptr<PredicateSetProvider> instance_;
    PredicateSetProvider() = default;

public:
    PredicateSetProvider(PredicateSetProvider const&) = delete;
    PredicateSetProvider& operator=(PredicateSetProvider const&) = delete;

    static std::shared_ptr<PredicateSetProvider> GetInstance();

    std::shared_ptr<PredicateSet> Create() const;
    std::shared_ptr<PredicateSet> Create(PredicateSet const& ps) const;
};

}  // namespace model

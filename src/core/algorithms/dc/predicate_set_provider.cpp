#include "predicate_set_provider.h"

namespace model {

std::shared_ptr<PredicateSetProvider> PredicateSetProvider::GetInstance() {
    if (!instance_) {
        instance_ = std::make_shared<PredicateSetProvider>();
    }
    return instance_;
}

std::shared_ptr<PredicateSet> PredicateSetProvider::Create() const {
    return std::make_shared<PredicateSet>();
}

std::shared_ptr<PredicateSet> PredicateSetProvider::Create(PredicateSet const& ps) const {
    return std::make_shared<PredicateSet>(ps);
}

std::shared_ptr<PredicateSetProvider> PredicateSetProvider::instance_ = nullptr;

}  // namespace model

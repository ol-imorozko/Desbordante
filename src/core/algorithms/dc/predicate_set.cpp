#include "predicate_set.h"

namespace model {

/* PredicateSet::PredicateSet(PredicateSet const& other) : bitset_(other.bitset_) {} */

/* PredicateSet& PredicateSet::operator=(PredicateSet const& other) { */
/*     if (this != &other) { */
/*         bitset_ = other.bitset_; */
/*     } */
/*     return *this; */
/* } */

bool PredicateSet::Add(PredicatePtr predicate) {
    auto index = PredicateIndexProvider::GetInstance()->GetIndex(predicate);
    if (index >= bitset_.size()) {
        bitset_.resize(index + 1);
    }

    bool just_added = !bitset_.test(index);
    bitset_.set(index);

    return just_added;
}

bool PredicateSet::Contains(PredicatePtr predicate) const {
    auto index = PredicateIndexProvider::GetInstance()->GetIndex(predicate);

    return index < bitset_.size() ? bitset_.test(index) : false;
}

PredicateSet::Iterator PredicateSet::begin() const {
    return Iterator(this, 0);
}

PredicateSet::Iterator PredicateSet::end() const {
    return Iterator(this, bitset_.size());
}

/* std::shared_ptr<PredicateSet> PredicateSet::GetInvTS() const { */
/*     if (!inv_set_TS_) { */
/*         inv_set_TS_ = PredicateSetProvider::GetInstance()->Create(); */

/*         for (auto pred : *this) inv_set_TS_->Add(pred->GetInvTS()); */
/*     } */

/*     return inv_set_TS_; */
/* } */

}  // namespace model

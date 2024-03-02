#pragma once

#include <iterator>
#include <memory>

#include <boost/dynamic_bitset.hpp>

#include "index_provider.h"
#include "predicate.h"

namespace model {

/**
 * TODO: add proper documentation
 *
 * It's a wrapper for dynamic_bitset, stores predicates
 * in bits, corresponding to their indexes in PredicateIndexProvider.
 *
 * TODO: implement inv_set_TS_
 *
 */
class PredicateSet {
private:
    boost::dynamic_bitset<> bitset_;
    /* mutable std::shared_ptr<PredicateSet> inv_set_TS_; */

public:
    /* PredicateSet() : bitset_(), inv_set_TS_(nullptr) {} */
    /* PredicateSet() : bitset_() {} */

    /* PredicateSet(PredicateSet const& other); */
    /* PredicateSet& operator=(PredicateSet const& other); */

    // Returns true if this predicate was newly addeed
    bool Add(PredicatePtr predicate);
    bool Contains(PredicatePtr predicate) const;

    /* TODO: Proper documentation
     *
     * Dangerous method.
     * Leaves an object in consistent, but unspecified state.
     */
    boost::dynamic_bitset<>&& GetBitset() {
        return std::move(bitset_);
    }

    std::shared_ptr<PredicateSet> GetInvTS() const;

    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = PredicatePtr;
        using difference_type = std::ptrdiff_t;
        using pointer = PredicatePtr;
        using reference = PredicatePtr;

    private:
        PredicateSet const* set_;
        size_t current_index_;

    public:
        Iterator(PredicateSet const* set, size_t start) : set_(set), current_index_(start) {
            // Initialize to point to the first valid predicate
            if (current_index_ < set_->bitset_.size() && !set_->bitset_.test(current_index_)) {
                ++(*this);
            }
        }

        reference operator*() const {
            return PredicateIndexProvider::GetInstance()->GetObject(current_index_);
        }

        pointer operator->() const {
            return operator*();
        }

        // Prefix increment
        Iterator& operator++() {
            do {
                ++current_index_;
            } while (current_index_ < set_->bitset_.size() && !set_->bitset_.test(current_index_));
            return *this;
        }

        // Postfix increment
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(Iterator const& a, Iterator const& b) {
            return a.current_index_ == b.current_index_ && a.set_ == b.set_;
        }

        friend bool operator!=(Iterator const& a, Iterator const& b) {
            return !(a == b);
        }
    };

    // NOLINTBEGIN(readability-identifier-naming)
    Iterator begin() const;
    Iterator end() const;
    // NOLINTEND(readability-identifier-naming)
};

}  // namespace model

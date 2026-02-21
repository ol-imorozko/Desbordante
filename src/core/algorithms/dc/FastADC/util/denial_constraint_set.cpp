#include "core/algorithms/dc/FastADC/util/denial_constraint_set.h"

#include "core/algorithms/dc/FastADC/providers/predicate_provider.h"

namespace algos::fastadc {

namespace {

// Helper function to check if all predicates in 'ps' are contained in 'other_ps',
// considering both the predicate itself and its symmetric version
bool ContainedIn(PredicateSet const& ps, PredicateSet const& other_ps,
                 PredicateProvider* provider) {
    for (PredicatePtr p : ps) {
        bool found = other_ps.Contains(p);
        if (!found && provider) {
            PredicatePtr symmetric = p->GetSymmetric(provider);
            found = other_ps.Contains(symmetric);
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

}  // anonymous namespace

// Implementation of DCHash::operator()
size_t DenialConstraintSet::DCHash::operator()(DenialConstraint const& dc) const {
    // Java logic:
    // int result1 = 0;
    // for (Predicate p : predicateSet)
    //     result1 += Math.max(p.hashCode(), p.getSymmetric().hashCode());
    //
    // int result2 = 0;
    // if (getInvT1T2DC() != null)
    //     for (Predicate p : getInvT1T2DC().predicateSet)
    //         result2 += Math.max(p.hashCode(), p.getSymmetric().hashCode());
    //
    // return Math.max(result1, result2);

    size_t result1 = 0;
    PredicateSet const& predicate_set = dc.GetPredicateSet();

    for (PredicatePtr p : predicate_set) {
        size_t p_hash = std::hash<Predicate>{}(*p);
        size_t sym_hash = p_hash;

        if (provider) {
            PredicatePtr symmetric = p->GetSymmetric(provider);
            sym_hash = std::hash<Predicate>{}(*symmetric);
        }

        result1 += std::max(p_hash, sym_hash);
    }

    size_t result2 = 0;
    if (provider) {
        DenialConstraint inv_dc = dc.GetInvT1T2DC(provider);
        PredicateSet const& inv_predicate_set = inv_dc.GetPredicateSet();

        if (inv_predicate_set.Size() > 0) {
            for (PredicatePtr p : inv_predicate_set) {
                size_t p_hash = std::hash<Predicate>{}(*p);
                PredicatePtr symmetric = p->GetSymmetric(provider);
                size_t sym_hash = std::hash<Predicate>{}(*symmetric);

                result2 += std::max(p_hash, sym_hash);
            }
        }
    }

    return std::max(result1, result2);
}

// Implementation of DCEqual::operator()
bool DenialConstraintSet::DCEqual::operator()(DenialConstraint const& lhs,
                                              DenialConstraint const& rhs) const {
    // Java logic:
    // if (this == obj) return true;
    // if (obj == null || getClass() != obj.getClass()) return false;
    //
    // DenialConstraint other = (DenialConstraint) obj;
    // if (predicateSet == null) {
    //     return other.predicateSet == null;
    // } else if (predicateSet.size() != other.predicateSet.size()) {
    //     return false;
    // } else {
    //     PredicateSet otherPS = other.predicateSet;
    //     return containedIn(otherPS) || getInvT1T2DC().containedIn(otherPS) ||
    //            containedIn(other.getInvT1T2DC().predicateSet);
    // }

    if (&lhs == &rhs) {
        return true;
    }

    PredicateSet const& lhs_ps = lhs.GetPredicateSet();
    PredicateSet const& rhs_ps = rhs.GetPredicateSet();

    if (lhs_ps.Size() != rhs_ps.Size()) {
        return false;
    }

    // Check if lhs is contained in rhs
    if (ContainedIn(lhs_ps, rhs_ps, provider)) {
        return true;
    }

    if (!provider) {
        return false;
    }

    // Check if lhs's inverse is contained in rhs
    DenialConstraint lhs_inv = lhs.GetInvT1T2DC(provider);
    if (lhs_inv.GetPredicateSet().Size() > 0 &&
        ContainedIn(lhs_inv.GetPredicateSet(), rhs_ps, provider)) {
        return true;
    }

    // Check if lhs is contained in rhs's inverse
    DenialConstraint rhs_inv = rhs.GetInvT1T2DC(provider);
    if (rhs_inv.GetPredicateSet().Size() > 0 &&
        ContainedIn(lhs_ps, rhs_inv.GetPredicateSet(), provider)) {
        return true;
    }

    return false;
}

}  // namespace algos::fastadc

#pragma once

#include <vector>

#include "predicate_builder.h"

namespace model {

/*
 * Maximum supported number of bits in clue is 64
 * (TODO: explain why more is probably unnecessary)
 */
using ClueBitset = std::bitset<64>;

/** Predicate EQ (and GT) of one column pair */
struct PredicatePack {
    PredicatePtr eq;
    PredicatePtr gt;
    size_t left_idx, right_idx;
    ClueBitset eq_mask, gt_mask;

    PredicatePack(PredicatePtr eq, size_t eq_pos)
        : eq(eq),
          gt(nullptr),
          left_idx(eq->GetLeftOperand().GetColumn()->GetIndex()),
          right_idx(eq->GetRightOperand().GetColumn()->GetIndex()),
          eq_mask(0),
          gt_mask(0) {
        eq_mask.set(eq_pos);
    }

    PredicatePack(PredicatePtr eq, size_t eq_pos, PredicatePtr gt, size_t gt_pos)
        : eq(eq),
          gt(gt),
          left_idx(eq->GetLeftOperand().GetColumn()->GetIndex()),
          right_idx(eq->GetRightOperand().GetColumn()->GetIndex()),
          eq_mask(0),
          gt_mask(0) {
        eq_mask.set(eq_pos);
        gt_mask.set(gt_pos);
    }
};

/* просто класс со статическими методами и полями. Они не привязаны к классу, просто
 * типо как в его неймспейсе. Эти поля будут использовать наследники в функции BuildClueSet() */
class ClueSetBuilder {
public:
    ClueSetBuilder(ClueSetBuilder const& other) = delete;
    ClueSetBuilder& operator=(ClueSetBuilder const& other) = delete;
    ClueSetBuilder(ClueSetBuilder&& other) noexcept = default;
    ClueSetBuilder& operator=(ClueSetBuilder&& other) noexcept = default;
    ClueSetBuilder();

    virtual ~ClueSetBuilder() = default;
    virtual std::vector<ClueBitset> BuildClueSet() = 0;

    // TODO: I don't understand how it works
    static void ConfigureOnce(PredicateBuilder const& pbuilder) {
        static bool is_configured [[maybe_unused]] = [&]() {
            BuildPredicatePacksAndCorrectionMap(pbuilder);
            return true;
        }();
    }

    /* For tests */
    size_t GetNumberOfBitsInClue() const {
        return correction_map_.size();
    }

    /* For tests */
    std::vector<PredicatePack> GetPredicatePacks() const {
        std::vector<PredicatePack> packs;
        packs.insert(packs.end(), str_single_packs_.begin(), str_single_packs_.end());
        packs.insert(packs.end(), str_cross_packs_.begin(), str_cross_packs_.end());
        packs.insert(packs.end(), num_single_packs_.begin(), num_single_packs_.end());
        packs.insert(packs.end(), num_cross_packs_.begin(), num_cross_packs_.end());
        return packs;
    }

    /* For tests */
    std::vector<ClueBitset> GetCorrectionMap() const {
        return correction_map_;
    }

private:
    static void BuildPredicatePacksAndCorrectionMap(PredicateBuilder const& pBuilder);

    static ClueBitset DynamicBitsetToClueBitset(boost::dynamic_bitset<>&& dynamic_bitset) {
        if (dynamic_bitset.size() > 64) {
            throw std::runtime_error(
                    "boost::dynamic_bitset<> is larger than 64 bits and cannot be converted to "
                    "std::bitset<64> without data loss.");
        }

        return dynamic_bitset.to_ulong();
    }

    static ClueBitset BuildCorrectionMask(PredicatesSpan group,
                                          std::initializer_list<OperatorType> types);

    using PackAction = std::function<void(PredicatesSpan, std::vector<PredicatePack>&, size_t&)>;

    /**
     * Splits vector @predicates by groups of @group_size, applies @action to them
     * to add freshly created pack to @pack.
     *
     * Also populates correction_map_.
     */
    static void BuildPacksAndCorrectionMap(PredicatesVector const& predicates, size_t group_size,
                                           PackAction action, std::vector<PredicatePack>& pack,
                                           size_t& count);

    /**
     * Processes numerical single-column predicates from a flat list, where each group
     * of six predicates for one column pair (EQUAL, UNEQUAL, GREATER, LESS, GREATER_EQUAL,
     * LESS_EQUAL) is stored consecutively one after another, like
     * "flatten([[6 preds], [6 preds], ..., [6 preds]])"
     */
    static void BuildNumPacks(PredicatesVector const& predicates, std::vector<PredicatePack>& pack,
                              size_t& count);

    /**
     * Processes categorical single-column predicates from a flat list, where each group
     * of two predicates for one column pair (EQUAL, UNEQUAL) is stored consecutively
     * one after another, like "flatten([[2 preds], [2 preds], ..., [2 preds]])"
     */
    // TODO: Cat типо categorical, а мы используем str вместо этого. По идее cat лучше
    static void BuildCatPacks(PredicatesVector const& predicates, std::vector<PredicatePack>& pack,
                              size_t& count);

protected:
    /** String single-column predicate packs */
    static std::vector<PredicatePack> str_single_packs_;
    /** String cross-column predicate packs */
    static std::vector<PredicatePack> str_cross_packs_;
    /** Numerical single-column predicate packs */
    static std::vector<PredicatePack> num_single_packs_;
    /** Numerical cross-column predicate packs */
    static std::vector<PredicatePack> num_cross_packs_;
    /** Predicate id -> its correction mask */
    static std::vector<ClueBitset> correction_map_;
};

}  // namespace model

#include "cross_clue_set_builder.h"

namespace model {

CrossClueSetBuilder::CrossClueSetBuilder(PredicateBuilder const& pbuilder, PliShard const& shard1,
                                         PliShard const& shard2)
    : plis1_(shard1.plis),
      plis2_(shard2.plis),
      evidence_count_((shard1.end - shard1.beg) * (shard2.end - shard2.beg)) {
    ConfigureOnce(pbuilder);
}

ClueSet CrossClueSetBuilder::BuildClueSet() {
    std::vector<ClueBitset> forward_clues(evidence_count_, 0);
    std::vector<ClueBitset> reverse_clues(evidence_count_, 0);

    for (auto const& cat_pack : str_single_packs_) {
        size_t eq_mask = cat_pack.eq_mask.to_ulong();
        CorrectStrSingle(forward_clues, reverse_clues, plis1_[cat_pack.left_idx],
                         plis2_[cat_pack.right_idx], eq_mask);
    }

    for (auto const& cat_pack : str_cross_packs_) {
        size_t eq_mask = cat_pack.eq_mask.to_ulong();
        CorrectStrCross(forward_clues, plis1_[cat_pack.left_idx], plis2_[cat_pack.right_idx],
                        eq_mask);
        CorrectStrCross(reverse_clues, plis2_[cat_pack.left_idx], plis1_[cat_pack.right_idx],
                        eq_mask);
    }

    for (auto const& num_pack : num_single_packs_) {
        size_t eq_mask = num_pack.eq_mask.to_ulong();
        size_t gt_mask = num_pack.gt_mask.to_ulong();
        CorrectNumSingle(forward_clues, reverse_clues, plis1_[num_pack.left_idx],
                         plis2_[num_pack.right_idx], eq_mask, gt_mask);
    }

    for (auto const& num_pack : num_cross_packs_) {
        size_t eq_mask = num_pack.eq_mask.to_ulong();
        size_t gt_mask = num_pack.gt_mask.to_ulong();
        CorrectNumCross(forward_clues, plis1_[num_pack.left_idx], plis2_[num_pack.right_idx],
                        eq_mask, gt_mask);
        CorrectNumCross(reverse_clues, plis2_[num_pack.left_idx], plis1_[num_pack.right_idx],
                        eq_mask, gt_mask);
    }

    return AccumulateClues(forward_clues, reverse_clues);
}

void CrossClueSetBuilder::SetSingleEQ(std::vector<ClueBitset>& clues1,
                                      std::vector<ClueBitset>& clues2, Pli const& pli1, size_t i,
                                      Pli const& pli2, size_t j, size_t mask) {
    size_t beg1 = pli1.pli_shard_->beg, range1 = pli1.pli_shard_->end - beg1;
    size_t beg2 = pli2.pli_shard_->beg, range2 = pli2.pli_shard_->end - beg2;

    for (size_t tid1 : pli1.GetClusters()[i]) {
        size_t t1 = tid1 - beg1, r1 = t1 * range2 - beg2;
        for (size_t tid2 : pli2.GetClusters()[j]) {
            clues1[r1 + tid2] |= mask;
            clues2[(tid2 - beg2) * range1 + t1] |= mask;
        }
    }
}

void CrossClueSetBuilder::CorrectStrSingle(std::vector<ClueBitset>& clues1,
                                           std::vector<ClueBitset>& clues2, Pli const& pivotPli,
                                           Pli const& probePli, size_t mask) {
    auto const& pivot_keys = pivotPli.GetKeys();

    for (size_t i = 0; i < pivot_keys.size(); ++i) {
        try {
            size_t j = probePli.GetClusterIdByKey(pivot_keys[i]);
            SetSingleEQ(clues1, clues2, pivotPli, i, probePli, j, mask);
        } catch (std::runtime_error const&) {
            // Ignore and continue if no cluster is found by key
        }
    }
}

void CrossClueSetBuilder::SetCrossEQ(std::vector<ClueBitset>& clues, Pli const& pli1, size_t i,
                                     Pli const& pli2, size_t j, size_t mask) {
    size_t tid_beg1 = pli1.pli_shard_->beg;
    size_t tid_beg2 = pli2.pli_shard_->beg, tid_range2 = pli2.pli_shard_->end - tid_beg2;

    for (size_t tid1 : pli1.GetClusters()[i]) {
        size_t r1 = (tid1 - tid_beg1) * tid_range2 - tid_beg2;
        for (size_t tid2 : pli2.GetClusters()[j]) {
            clues[r1 + tid2] |= mask;
        }
    }
}

void CrossClueSetBuilder::CorrectStrCross(std::vector<ClueBitset>& clues, Pli const& pivotPli,
                                          Pli const& probePli, size_t mask) {
    auto const& pivot_keys = pivotPli.GetKeys();

    for (size_t i = 0; i < pivot_keys.size(); ++i) {
        try {
            auto j = probePli.GetClusterIdByKey(pivot_keys[i]);
            SetCrossEQ(clues, pivotPli, i, probePli, j, mask);
        } catch (std::runtime_error const&) {
            // Ignore and continue if no cluster is found by key
        }
    }
}

void CrossClueSetBuilder::SetReverseGT(std::vector<ClueBitset>& reverseArray, Pli const& probePli,
                                       size_t to, Pli const& pivotPli, size_t i, size_t mask) {
    size_t probe_beg = probePli.pli_shard_->beg;
    size_t pivot_beg = pivotPli.pli_shard_->beg, pivot_range = pivotPli.pli_shard_->end - pivot_beg;

    for (size_t j = 0; j < to; ++j) {
        for (size_t probe_tid : probePli.GetClusters()[j]) {
            size_t r2 = (probe_tid - probe_beg) * pivot_range - pivot_beg;
            for (size_t pivot_tid : pivotPli.GetClusters()[i]) {
                reverseArray[r2 + pivot_tid] |= mask;
            }
        }
    }
}

void CrossClueSetBuilder::SetForwardGT(std::vector<ClueBitset>& forwardArray, Pli const& pivotPli,
                                       size_t i, Pli const& probePli, size_t from, size_t mask) {
    size_t pivot_beg = pivotPli.pli_shard_->beg;
    size_t probe_beg = probePli.pli_shard_->beg, probe_range = probePli.pli_shard_->end - probe_beg;

    for (size_t pivot_tid : pivotPli.GetClusters()[i]) {
        size_t r1 = (pivot_tid - pivot_beg) * probe_range - probe_beg;
        for (size_t j = from; j < probePli.GetClusters().size(); ++j) {
            for (size_t probe_tid : probePli.GetClusters()[j]) {
                forwardArray[r1 + probe_tid] |= mask;
            }
        }
    }
}

void CrossClueSetBuilder::CorrectNumSingle(std::vector<ClueBitset>& forwardArray,
                                           std::vector<ClueBitset>& reverseArray,
                                           Pli const& pivotPli, Pli const& probePli, size_t eqMask,
                                           size_t gtMask) {
    auto const& pivot_keys = pivotPli.GetKeys();
    auto const& probe_keys = probePli.GetKeys();

    for (size_t i = 0, j = 0; i < pivot_keys.size(); ++i) {
        size_t pivot_key = pivot_keys[i];
        j = probePli.GetFirstIndexWhereKeyIsLTE(pivot_key, j);  // current position in probePli
        size_t reverse_to = j;

        if (j == probe_keys.size()) {  // pivotKey is less than all probeKeys
            for (size_t ii = i; ii < pivot_keys.size(); ++ii) {
                SetReverseGT(reverseArray, probePli, j, pivotPli, ii, gtMask);
            }
            break;
        } else if (pivot_key == probe_keys[j]) {
            SetSingleEQ(forwardArray, reverseArray, pivotPli, i, probePli, j, eqMask);
            j++;
        }

        SetForwardGT(forwardArray, pivotPli, i, probePli, j, gtMask);
        SetReverseGT(reverseArray, probePli, reverse_to, pivotPli, i, gtMask);
    }
}

void CrossClueSetBuilder::CorrectNumCross(std::vector<ClueBitset>& forwardArray,
                                          Pli const& pivotPli, Pli const& probePli, size_t eqMask,
                                          size_t gtMask) {
    auto const& pivot_keys = pivotPli.GetKeys();
    auto const& probe_keys = probePli.GetKeys();

    for (size_t i = 0, j = 0; i < pivot_keys.size(); ++i) {
        size_t key = pivot_keys[i];
        j = probePli.GetFirstIndexWhereKeyIsLTE(key, j);  // current position in probePli

        if (j == probe_keys.size()) {
            break;
        } else if (key == probe_keys[j]) {
            SetCrossEQ(forwardArray, pivotPli, i, probePli, j, eqMask);
            j++;
        }

        SetForwardGT(forwardArray, pivotPli, i, probePli, j, gtMask);
    }
}

}  // namespace model

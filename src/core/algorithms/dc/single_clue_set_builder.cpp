#include "single_clue_set_builder.h"

#include "dc/common_clue_set_builder.h"

namespace model {

SingleClueSetBuilder::SingleClueSetBuilder(PredicateBuilder const& pbuilder, PliShard const& shard)
    : plis_(shard.plis),  // TODO: copying. Maybe move?
      tid_beg_(shard.beg),
      tid_range_(shard.end - shard.beg),
      evidence_count_(tid_range_ * tid_range_) {
    ConfigureOnce(pbuilder);
}

std::vector<ClueBitset> SingleClueSetBuilder::BuildClueSet() {
    std::vector<uint64_t> clues(evidence_count_, 0);

    for (auto const& cat_pack : str_single_packs_) {
        CorrectStrSingle(clues, plis_[cat_pack.left_idx], cat_pack.eq_mask.to_ulong());
    }

    for (auto const& cat_pack : str_cross_packs_) {
        CorrectStrCross(clues, plis_[cat_pack.left_idx], plis_[cat_pack.right_idx],
                        cat_pack.eq_mask.to_ulong());
    }

    for (auto const& num_pack : num_single_packs_) {
        CorrectNumSingle(clues, plis_[num_pack.left_idx], num_pack.eq_mask.to_ulong(),
                         num_pack.gt_mask.to_ulong());
    }

    for (auto const& num_pack : num_cross_packs_) {
        CorrectNumCross(clues, plis_[num_pack.left_idx], plis_[num_pack.right_idx],
                        num_pack.eq_mask.to_ulong(), num_pack.gt_mask.to_ulong());
    }

    std::vector<ClueBitset> clue_set;
    clue_set.reserve(clues.size());
    for (auto clue : clues) {
        if (clue != 0) {
            clue_set.push_back(clue);
        }
    }
    return clue_set;
}

void SingleClueSetBuilder::SetSingleEQ(std::vector<uint64_t>& clues, Pli::Cluster const& cluster,
                                       size_t mask) {
    for (size_t i = 0; i < cluster.size() - 1; ++i) {
        size_t t1 = cluster[i] - tid_beg_;
        size_t r1 = t1 * tid_range_;
        for (size_t j = i + 1; j < cluster.size(); ++j) {
            size_t t2 = cluster[j] - tid_beg_;
            clues[r1 + t2] |= mask;
            clues[t2 * tid_range_ + t1] |= mask;
        }
    }
}

void SingleClueSetBuilder::CorrectStrSingle(std::vector<uint64_t>& clues, Pli const& pli,
                                            size_t mask) {
    for (auto const& cluster : pli.GetClusters()) {
        if (cluster.size() > 1) {
            SetSingleEQ(clues, cluster, mask);
        }
    }
}

void SingleClueSetBuilder::SetCrossEQ(std::vector<uint64_t>& clues,
                                      Pli::Cluster const& pivotCluster,
                                      Pli::Cluster const& probeCluster, size_t mask) {
    for (size_t tid1 : pivotCluster) {
        size_t r1 = (tid1 - tid_beg_) * tid_range_;
        for (size_t tid2 : probeCluster) {
            if (tid1 != tid2) {
                clues[r1 + tid2] |= mask;
            }
        }
    }
}

void SingleClueSetBuilder::CorrectStrCross(std::vector<uint64_t>& clues, Pli const& pivotPli,
                                           Pli const& probePli, size_t mask) {
    auto const& pivot_clusters = pivotPli.GetClusters();
    auto const& probe_clusters = probePli.GetClusters();
    for (size_t i = 0; i < pivot_clusters.size(); ++i) {
        SetCrossEQ(clues, pivot_clusters[i], probe_clusters[i], mask);
    }
}

void SingleClueSetBuilder::SetGT(std::vector<uint64_t>& clues, Pli::Cluster const& pivotCluster,
                                 Pli const& probePli, int from, size_t mask) {
    for (size_t pivot_tid : pivotCluster) {
        size_t r1 = (pivot_tid - tid_beg_) * tid_range_;
        for (size_t j = from; j < probePli.GetClusters().size(); ++j) {
            for (size_t probe_tid : probePli.GetClusters()[j]) {
                if (pivot_tid != probe_tid) {
                    clues[r1 + probe_tid] |= mask;
                }
            }
        }
    }
}

void SingleClueSetBuilder::CorrectNumSingle(std::vector<uint64_t>& clues, Pli const& pli,
                                            size_t eqMask, size_t gtMask) {
    for (size_t i = 0; i < pli.GetClusters().size(); ++i) {
        auto const& cluster = pli.GetClusters()[i];
        if (cluster.size() > 1) {
            SetSingleEQ(clues, cluster, eqMask);
        }
        if (i < pli.GetClusters().size() - 1) {
            SetGT(clues, cluster, pli, i + 1, gtMask);
        }
    }
}

void SingleClueSetBuilder::CorrectNumCross(std::vector<uint64_t>& clues, Pli const& pivotPli,
                                           Pli const& probePli, size_t eqMask, size_t gtMask) {
    for (size_t i = 0; i < pivotPli.GetKeys().size(); ++i) {
        size_t pivot_key = pivotPli.GetKeys()[i];
        auto j = probePli.GetFirstIndexWhereKeyIsLTE(pivot_key);
        if (j < probePli.GetKeys().size() && pivot_key == probePli.GetKeys()[j]) {
            SetCrossEQ(clues, pivotPli.GetClusters()[i], probePli.GetClusters()[j], eqMask);
            j++;
        }
        SetGT(clues, pivotPli.GetClusters()[i], probePli, j, gtMask);
    }
}

}  // namespace model

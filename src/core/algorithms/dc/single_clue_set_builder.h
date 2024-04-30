#pragma once

#include <vector>

#include "dc/common_clue_set_builder.h"
#include "dc/pli_shard.h"

namespace model {
class SingleClueSetBuilder : public ClueSetBuilder {
public:
    SingleClueSetBuilder(PredicateBuilder const& pbuilder, PliShard const& shard);

    std::vector<ClueBitset> BuildClueSet() override;

private:
    std::vector<Pli> plis_;
    size_t tid_beg_;
    size_t tid_range_;
    size_t evidence_count_;

    void SetSingleEQ(std::vector<uint64_t>& clues, Pli::Cluster const& cluster, size_t mask);
    void CorrectStrSingle(std::vector<uint64_t>& clues, Pli const& pli, size_t mask);
    void SetCrossEQ(std::vector<uint64_t>& clues, Pli::Cluster const& pivotCluster,
                    Pli::Cluster const& probeCluster, size_t mask);
    void CorrectStrCross(std::vector<uint64_t>& clues, Pli const& pivotPli, Pli const& probePli,
                         size_t mask);
    void SetGT(std::vector<uint64_t>& clues, Pli::Cluster const& pivotCluster, Pli const& probePli,
               int from, size_t mask);
    void CorrectNumSingle(std::vector<uint64_t>& clues, Pli const& pli, size_t eqMask,
                          size_t gtMask);
    void CorrectNumCross(std::vector<uint64_t>& clues, Pli const& pivotPli, Pli const& probePli,
                         size_t eqMask, size_t gtMask);
};

}  // namespace model

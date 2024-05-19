#pragma once

#include <vector>

#include "dc/common_clue_set_builder.h"
#include "dc/pli_shard.h"

namespace model {
class SingleClueSetBuilder : public CommonClueSetBuilder {
public:
    SingleClueSetBuilder(PredicateBuilder const& pbuilder, PliShard const& shard);

    ClueSet BuildClueSet() override;

private:
    std::vector<Pli> plis_;
    int tid_beg_;
    int tid_range_;
    size_t evidence_count_;

    void SetSingleEQ(std::vector<ClueBitset>& clues, Pli::Cluster const& cluster, size_t mask);
    void CorrectStrSingle(std::vector<ClueBitset>& clues, Pli const& pli, size_t mask);
    void SetCrossEQ(std::vector<ClueBitset>& clues, Pli::Cluster const& pivotCluster,
                    Pli::Cluster const& probeCluster, size_t mask);
    void CorrectStrCross(std::vector<ClueBitset>& clues, Pli const& pivotPli, Pli const& probePli,
                         size_t mask);
    void SetGT(std::vector<ClueBitset>& clues, Pli::Cluster const& pivotCluster,
               Pli const& probePli, int from, size_t mask);
    void CorrectNumSingle(std::vector<ClueBitset>& clues, Pli const& pli, size_t eqMask,
                          size_t gtMask);
    void CorrectNumCross(std::vector<ClueBitset>& clues, Pli const& pivotPli, Pli const& probePli,
                         size_t eqMask, size_t gtMask);
};

}  // namespace model

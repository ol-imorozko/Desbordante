#pragma once

#include <vector>

#include "dc/common_clue_set_builder.h"
#include "dc/pli_shard.h"

namespace model {

class CrossClueSetBuilder : public CommonClueSetBuilder {
public:
    CrossClueSetBuilder(PredicateBuilder const& pbuilder, PliShard const& shard1,
                        PliShard const& shard2);

    ClueSet BuildClueSet() override;

private:
    std::vector<Pli> plis1_, plis2_;
    size_t evidence_count_;

    void SetSingleEQ(std::vector<ClueBitset>& clues1, std::vector<ClueBitset>& clues2,
                     Pli const& pli1, size_t i, Pli const& pli2, size_t j, size_t mask);
    void CorrectStrSingle(std::vector<ClueBitset>& clues1, std::vector<ClueBitset>& clues2,
                          Pli const& pivotPli, Pli const& probePli, size_t mask);
    void SetCrossEQ(std::vector<ClueBitset>& clues, Pli const& pli1, size_t i, Pli const& pli2,
                    size_t j, size_t mask);
    void CorrectStrCross(std::vector<ClueBitset>& clues, Pli const& pivotPli, Pli const& probePli,
                         size_t mask);
    void SetReverseGT(std::vector<ClueBitset>& reverseArray, Pli const& probePli, size_t to,
                      Pli const& pivotPli, size_t i, size_t mask);
    void SetForwardGT(std::vector<ClueBitset>& forwardArray, Pli const& pivotPli, size_t i,
                      Pli const& probePli, size_t from, size_t mask);
    void CorrectNumSingle(std::vector<ClueBitset>& forwardArray,
                          std::vector<ClueBitset>& reverseArray, Pli const& pivotPli,
                          Pli const& probePli, size_t eqMask, size_t gtMask);
    void CorrectNumCross(std::vector<ClueBitset>& forwardArray, Pli const& pivotPli,
                         Pli const& probePli, size_t eqMask, size_t gtMask);
};

}  // namespace model

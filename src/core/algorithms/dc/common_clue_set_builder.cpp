#include "common_clue_set_builder.h"

#include "operator.h"
#include "predicate_set.h"

namespace model {

// Define static members
std::vector<PredicatePack> CommonClueSetBuilder::str_single_packs_;
std::vector<PredicatePack> CommonClueSetBuilder::str_cross_packs_;
std::vector<PredicatePack> CommonClueSetBuilder::num_single_packs_;
std::vector<PredicatePack> CommonClueSetBuilder::num_cross_packs_;
std::vector<ClueBitset> CommonClueSetBuilder::correction_map_;

template <typename... Vectors>
ClueSet CommonClueSetBuilder::AccumulateClues(Vectors const&... vectors) const {
    ClueSet clue_set;
    auto insert_clues = [&clue_set](std::vector<ClueBitset> const& clues) {
        for (auto const& clue : clues) {
            if (clue.any()) {
                clue_set.insert(clue);
            }
        }
    };
    (insert_clues(vectors), ...);
    return clue_set;
}

ClueBitset CommonClueSetBuilder::BuildCorrectionMask(PredicatesSpan group,
                                                     std::initializer_list<OperatorType> types) {
    PredicateSet mask;

    for (auto p : group) {
        if (std::any_of(types.begin(), types.end(),
                        [p](OperatorType type) { return p->GetOperator() == type; })) {
            mask.Add(p);
        }
    }

    return DynamicBitsetToClueBitset(mask.GetBitset());
}

void CommonClueSetBuilder::BuildPacksAndCorrectionMap(PredicatesVector const& predicates,
                                                      size_t group_size, PackAction action,
                                                      std::vector<PredicatePack>& pack,
                                                      size_t& count) {
    assert(predicates.size() % group_size == 0);
    size_t num_groups = predicates.size() / group_size;

    for (size_t i = 0; i < num_groups; ++i) {
        size_t base_index = i * group_size;
        PredicatesSpan group_span(predicates.begin() + base_index, group_size);

        action(group_span, pack, count);
    }
}

void CommonClueSetBuilder::BuildNumPacks(PredicatesVector const& predicates,
                                         std::vector<PredicatePack>& pack, size_t& count) {
    PackAction action = [](PredicatesSpan group_span, std::vector<PredicatePack>& pack,
                           size_t& count) {
        PredicatePtr eq = GetPredicateByType(group_span, OperatorType::kEqual);
        PredicatePtr gt = GetPredicateByType(group_span, OperatorType::kGreater);
        std::initializer_list<OperatorType> eq_list = {OperatorType::kEqual, OperatorType::kUnequal,
                                                       OperatorType::kLess,
                                                       OperatorType::kGreaterEqual};
        std::initializer_list<OperatorType> gt_list = {OperatorType::kLess, OperatorType::kGreater,
                                                       OperatorType::kLessEqual,
                                                       OperatorType::kGreaterEqual};

        correction_map_[count] = BuildCorrectionMask(group_span, eq_list);
        correction_map_[count + 1] = BuildCorrectionMask(group_span, gt_list);

        pack.emplace_back(eq, count, gt, count + 1);
        count += 2;
    };

    BuildPacksAndCorrectionMap(predicates, 6, action, pack, count);
}

void CommonClueSetBuilder::BuildCatPacks(PredicatesVector const& predicates,
                                         std::vector<PredicatePack>& pack, size_t& count) {
    PackAction action = [](PredicatesSpan group_span, std::vector<PredicatePack>& pack,
                           size_t& count) {
        PredicatePtr eq = GetPredicateByType(group_span, OperatorType::kEqual);
        std::initializer_list<OperatorType> eq_list = {OperatorType::kEqual,
                                                       OperatorType::kUnequal};

        correction_map_[count] = BuildCorrectionMask(group_span, eq_list);

        pack.emplace_back(eq, count);
        count++;
    };

    BuildPacksAndCorrectionMap(predicates, 2, action, pack, count);
}

void CommonClueSetBuilder::BuildPredicatePacksAndCorrectionMap(PredicateBuilder const& pBuilder) {
    auto const& cat_single = pBuilder.GetStrSingleColumnPredicates();
    auto const& cat_cross = pBuilder.GetStrCrossColumnPredicates();
    auto const& num_single = pBuilder.GetNumSingleColumnPredicates();
    auto const& num_cross = pBuilder.GetNumCrossColumnPredicates();
    size_t count = 0;

    // TODO: описать по нормальному. Типо категориальные предикаты кодируются одним битом, а
    // числовые двумя Поэтому в изначальном коде размер битсета это "число групп, где в каждой
    // группе 2 предиката + число групп, где в каждой группе 6 предикатов * 2"
    correction_map_.resize(cat_single.size() / 2 + cat_cross.size() / 2 +
                           2 * num_single.size() / 6 + 2 * num_cross.size() / 6);

    BuildCatPacks(cat_single, str_single_packs_, count);
    BuildCatPacks(cat_cross, str_cross_packs_, count);
    BuildNumPacks(num_single, num_single_packs_, count);
    BuildNumPacks(num_cross, num_cross_packs_, count);

    assert(count <= 64);
}

}  // namespace model

#pragma once

#include "dc/cross_clue_set_builder.h"
#include "dc/pli_shard.h"
#include "dc/predicate_builder.h"
#include "dc/single_clue_set_builder.h"

namespace model {

class ClueSetBuilder {
public:
    ClueSetBuilder(PredicateBuilder const& pbuilder) : predicate_builder_(pbuilder){};

    ClueSet BuildClueSet(std::vector<PliShard> const& pliShards) {
        ClueSet clue_set;
        size_t task_count = (pliShards.size() * (pliShards.size() + 1)) / 2;
        std::cout << "  [CLUE] task count: " << task_count << std::endl;

        auto add = [&clue_set](ClueBitset const& clue) { clue_set.insert(clue); };
        auto print = [](ClueBitset const& clue) { std::cout << "  Clue: " << clue << std::endl; };

        for (size_t i = 0; i < pliShards.size(); i++) {
            for (size_t j = i; j < pliShards.size(); j++) {
                std::unique_ptr<CommonClueSetBuilder> builder;
                if (i == j) {
                    builder = std::make_unique<SingleClueSetBuilder>(predicate_builder_,
                                                                     pliShards[i]);
                } else {
                    builder = std::make_unique<CrossClueSetBuilder>(predicate_builder_,
                                                                    pliShards[i], pliShards[j]);
                }

                ClueSet partial_clue_set = builder->BuildClueSet();
                std::cout << "Partial clue set for [" << i << ", " << j << "]:" << std::endl;
                for (auto const& clue : partial_clue_set) {
                    print(clue);
                    add(clue);
                }
            }
        }

        std::cout << "Final clue set:" << std::endl;
        for (auto const& clue : clue_set) {
            print(clue);
        }
        return clue_set;
    }

private:
    PredicateBuilder const& predicate_builder_;
};

}  // namespace model

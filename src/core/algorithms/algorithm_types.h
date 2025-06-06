#pragma once

#include <enum.h>

#include "algorithms/algorithms.h"

namespace algos {

using AlgorithmTypes =
        std::tuple<Depminer, DFD, FastFDs, FDep, FdMine, Pyro, Tane, PFDTane, FUN, hyfd::HyFD, Aid,
                   EulerFD, Apriori, des::DES, metric::MetricVerifier, DataStats,
                   fd_verifier::FDVerifier, HyUCC, PyroUCC, HPIValid, cfd::FDFirstAlgorithm,
                   ACAlgorithm, UCCVerifier, Faida, Spider, Mind, INDVerifier, Fastod, GfdValidator,
                   EGfdValidator, NaiveGfdValidator, order::Order, dd::Split, Cords, hymd::HyMD,
                   PFDVerifier, cfd_verifier::CFDVerifier>;

// clang-format off
/* Enumeration of all supported non-pipeline algorithms. If you implement a new
 * algorithm please add its corresponding value to this enum and to the type
 * tuple above.
 * NOTE: algorithm string name representation is taken from the value in this
 * enum, so name it appropriately (lowercase and without additional symbols).
 */
BETTER_ENUM(AlgorithmType, char,
/* Functional dependency mining algorithms */
    depminer = 0,
    dfd,
    fastfds,
    fdep,
    fdmine,
    pyro,
    tane,
    pfdtane,
    fun,
    hyfd,
    aidfd,
    eulerfd,

/* Association rules mining algorithms */
    apriori,

/* Numerical association rules mining algorithms*/
    des,

/* Metric verifier algorithm */
    metric,

/* Statistic algorithms */
    stats,

/* FD verifier algorithm */
    fd_verifier,

/* Unique Column Combination mining algorithms */
    hyucc,
    pyroucc,
    hpivalid,

/* CFD mining algorithms */
    fd_first_dfs,

/* Algebraic constraints mining algorithm*/
    ac,

/* UCC verifier algorithm */
    ucc_verifier,

/* Inclusion dependency mining algorithms */
    faida,
    spider,
    mind,

/* IND verifier algorithm */
    ind_verifier,

/* Order dependency mining algorithms */
    fastod,

/* Graph functional dependency mining algorithms */
    gfdvalid,
    egfdvalid,
    naivegfdvalid,

/* Order dependency mining algorithms */
    order,

/* Differential dependencies mining algorithm */
    split,

/* SFD mining algorithm */
    cords,

/* MD mining algorithms */
    hymd,

/* PFD verifier algorithm */
    pfd_verifier,

/* CFD verifier algorithm */
    cfd_verifier
)
// clang-format on

static_assert(std::tuple_size_v<AlgorithmTypes> == AlgorithmType::_size(),
              "The AlgorithmTypes tuple and the AlgorithmType enum sizes must be the same. Did you "
              "forget to add your new algorithm to either of those?");

}  // namespace algos

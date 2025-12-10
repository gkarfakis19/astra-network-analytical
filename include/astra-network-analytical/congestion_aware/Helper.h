/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#pragma once

#include "common/NetworkParser.h"
#include "congestion_aware/Topology.h"
#include <memory>

using namespace NetworkAnalytical;

namespace NetworkAnalyticalCongestionAware {

/**
 * Construct a topology from a NetworkParser.
 *
 * @param network_parser NetworkParser to parse the network input file
 * @return pointer to the constructed topology
 */
[[nodiscard]] std::shared_ptr<Topology> construct_topology(const NetworkParser& network_parser) noexcept;

[[nodiscard]] std::vector<std::pair<MultiDimAddress, MultiDimAddress>> generateAddressPairs(
    const MultiDimAddress& upper, const ConnectionPolicy& policy, int dim) noexcept;

[[nodiscard]] std::vector<std::pair<MultiDimAddress, MultiDimAddress>> generateAddressPairs_only_first_nodes(
    const std::vector<int>& npus_count_per_dim, const ConnectionPolicy& policy, int dim) noexcept;

void generateFreeComb(const MultiDimAddress& upper,
                      int dim,
                      const ConnectionPolicy& policy,
                      std::vector<int>& current,
                      int index,
                      std::vector<std::pair<MultiDimAddress, MultiDimAddress>>& result) noexcept;

void generateFreeComb_only_first_nodes(
    const MultiDimAddress& upper,
    int dim,
    const ConnectionPolicy& policy,
    MultiDimAddress& current,
    int index,
    std::vector<std::pair<MultiDimAddress, MultiDimAddress>>& result) noexcept;

}  // namespace NetworkAnalyticalCongestionAware

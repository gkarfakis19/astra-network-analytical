/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/HyperCube.h"
#include <cassert>
#include <cmath>


using namespace NetworkAnalyticalCongestionAware;

HyperCube::HyperCube(const int npus_count,
           const Bandwidth bandwidth,
           const Latency latency,
           const bool bidirectional,
           const bool is_multi_dim,
           const int non_recursive_topo,
           const std::vector<std::tuple<int, int, double>>& faulty_links) noexcept
    : bidirectional(bidirectional),
      BasicTopology(npus_count, npus_count, bandwidth, latency, is_multi_dim),
      non_recursive_topo(non_recursive_topo),
      faulty_links(faulty_links) 
{   // initialize faulty links
    assert(npus_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    HyperCube::basic_topology_type = TopologyBuildingBlock::HyperCube;

    if (!is_multi_dim) {
        double scale_factor = 2.0;
        //connect npus in a HyperCube
        int num_bits = std::ceil(std::log2(npus_count));

        for (int u = 0; u < npus_count; u++) {
            for (int d = 0; d < num_bits; d++) {
                int v = u ^ (1 << d);      // flip bit d

                if (v < npus_count) {      // ensure within range
                    double derate = fault_derate(u, v);

                    double bw = (derate == 0) ?
                        bandwidth * scale_factor :                       // no fault
                        bandwidth * derate * scale_factor;               // derated link

                    if (derate != 1)
                        std::cout << "HERE IS FAULTY_LINK" << std::endl;

                    connect(u, v, bw, latency, bidirectional);
                }
            }
        }
    }
    // this also works
    // std::vector<ConnectionPolicy> policies = get_connection_policies();
    // for (const auto& policy : policies) {
    //     connect(policy.src, policy.dst, bandwidth, latency, /*bidirectional=*/false);
    // }
}

Route HyperCube::route(DeviceId src, DeviceId dest) const noexcept {
    // assert npus are in valid range
    assert(0 <= src && src < npus_count);
    assert(0 <= dest && dest < npus_count);

    Route route;
    DeviceId current = src;

    // always include start
    route.push_back(devices.at(current));

    // Continue until we reach destination
    while (current != dest) {
        // Find the lowest differing bit
        DeviceId diff = current ^ dest;   // bits that differ

        // Pick the lowest-order differing bit
        int bit_to_flip = __builtin_ctz(diff);  
        // ^ portable on gcc/clang; I can rewrite if you need MSVC support

        // Compute next hop by flipping bit
        DeviceId next = current ^ (1 << bit_to_flip);

        // Safety check
        assert(0 <= next && next < npus_count);

        // Append next device
        route.push_back(devices.at(next));

        // Move forward
        current = next;
    }

    return route;
}


std::vector<ConnectionPolicy> HyperCube::get_connection_policies() const noexcept {
    std::vector<ConnectionPolicy> policies;

    // dimension count: npus_count must be power of 2
    int dims = static_cast<int>(std::log2(npus_count));

    for (int i = 0; i < npus_count; i++) {
        for (int d = 0; d < dims; d++) {

            int neighbor = i ^ (1 << d);   // flip bit d to get neighbor
            policies.emplace_back(i, neighbor);

            if (bidirectional) {
                policies.emplace_back(neighbor, i);
            }
        }
    }

    return policies;
}


double HyperCube::fault_derate(int src, int dst) const {
    for (const auto& link : faulty_links) {
        int a = std::get<0>(link);
        int b = std::get<1>(link);
        double health = std::get<2>(link);

        if ((a == src && b == dst) || (a == dst && b == src)) {
            return health;
        }
    }
    return 1.0;  // ✅ default: healthy link
}


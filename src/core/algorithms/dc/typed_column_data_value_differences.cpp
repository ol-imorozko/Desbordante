#include "typed_column_data_value_differences.h"

#include <easylogging++.h>

namespace model {

template <typename T>
static double GetSharedPercentageTyped(std::vector<std::byte const*> const& data_vec1,
                                       std::vector<std::byte const*> const& data_vec2,
                                       Type const& type) {
    if (data_vec1 == data_vec2) return 1.;

    std::unordered_map<T, size_t> freq_map1;
    std::unordered_map<T, size_t> freq_map2;

    for (std::byte const* ptr : data_vec1) freq_map1[type.GetValue<T>(ptr)]++;
    for (std::byte const* ptr : data_vec2) freq_map2[type.GetValue<T>(ptr)]++;

    size_t shared_count = 0;
    size_t total_count = 0;

    for (auto const& [data, frequency1] : freq_map1) {
        auto it = freq_map2.find(data);
        size_t frequency2 = (it == freq_map2.end()) ? 0 : it->second;

        shared_count += std::min(frequency1, frequency2);
        total_count += std::max(frequency1, frequency2);
    }

    return total_count > 0 ? static_cast<double>(shared_count) / total_count : 0.0;
}

template <typename T>
static double CalculateAverageTyped(std::vector<std::byte const*> const& data, Type const& type) {
    if (data.empty()) {
        return 0.0;
    }

    double sum = 0.0;
    for (std::byte const* ptr : data) sum += static_cast<double>(type.GetValue<T>(ptr));

    return sum / data.size();
}

double GetSharedPercentage(TypedColumnData const& c1, TypedColumnData const& c2) {
    switch (c1.GetTypeId()) {
        case TypeId::kInt:
            return GetSharedPercentageTyped<int>(c1.GetData(), c2.GetData(), c1.GetType());
        case TypeId::kDouble:
            return GetSharedPercentageTyped<double>(c1.GetData(), c2.GetData(), c1.GetType());
        case TypeId::kString:
            return GetSharedPercentageTyped<std::string>(c1.GetData(), c2.GetData(), c1.GetType());
        default:
            LOG(DEBUG) << "Column type " << c1.GetType().ToString()
                       << " is not supported for shared percentage calculation";
            return -1;
    }
}

double GetAverageRatio(TypedColumnData const& c1, TypedColumnData const& c2) {
    double avg1 = 0.0, avg2 = 0.0;

    switch (c1.GetTypeId()) {
        case TypeId::kInt:
            avg1 = CalculateAverageTyped<int>(c1.GetData(), c1.GetType());
            avg2 = CalculateAverageTyped<int>(c2.GetData(), c2.GetType());
            break;
        case TypeId::kDouble:
            avg1 = CalculateAverageTyped<double>(c1.GetData(), c1.GetType());
            avg2 = CalculateAverageTyped<double>(c2.GetData(), c2.GetType());
            break;
        default:
            LOG(DEBUG) << "Column type  " << c1.GetType().ToString() << " is not numeric";
            return -1;
    }

    return std::min(avg1, avg2) / std::max(avg1, avg2);
}

}  // namespace model

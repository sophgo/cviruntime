#include <algorithm>
#include <cmath>
#include <cpu_function/topk.hpp>
#include <iostream>
#include <runtime/neuron.hpp>
#include <string.h>
#include <vector>

template <typename T>
void topk_indices(std::vector<std::pair<int, T>> &result, const T *items,
                  int num_elem, int k, bool largest) {
  using pair_t = std::pair<int, T>;
  auto cmp_large = [](pair_t const &item1, pair_t const &item2) {
    return (item1.second > item2.second) ||
           (item1.second == item2.second && item1.first < item2.first);
  };
  auto cmp_small = [](pair_t const &item1, pair_t const &item2) {
    return (item1.second < item2.second) ||
           (item1.second == item2.second && item1.first > item2.first);
  };
  auto cmp = largest ? cmp_large : cmp_small;
  std::vector<pair_t> topk;
  for (int i = 0; i < num_elem; i++)
    topk.emplace_back(i, items[i]);
  result.resize(k);
  std::partial_sort_copy(topk.begin(), topk.end(), result.begin(), result.end(),
                         cmp);
}

namespace cvi {
namespace runtime {
TopKFunc::~TopKFunc() {}

void TopKFunc::setup(std::vector<std::shared_ptr<Neuron>> &inputs,
                     std::vector<std::shared_ptr<Neuron>> &outputs,
                     OpParam &param) {
  _bottoms = inputs;
  _tops = outputs;
  _kvalue = param.get<int32_t>("K");
  _axis = param.get<int32_t>("axis");
  _sorted = (int)param.get<bool>("sorted");
  _largest = (int)param.get<bool>("largest");
}

void TopKFunc::run() {
  auto bottom_data = _bottoms[0]->cpu_data<float>();
  auto top_data0 = _tops[0]->cpu_data<float>();
  std::vector<int> input_shape = _bottoms[0]->shape;

  assert(_sorted != false);
  int axis_dim = input_shape[_axis];
  int outer_dim = 1;
  for (int i = 0; i < _axis; i++) {
    outer_dim *= input_shape[i];
  }
  for (int i = 0; i < outer_dim; i++) {
    auto *ptr = bottom_data + i * axis_dim;
    std::vector<std::pair<int, float>> result;
    topk_indices(result, ptr, axis_dim, _kvalue, _largest);
    for (int k = 0; k < _kvalue; k++) {
      auto values_ptr = top_data0 + i * _kvalue + k;
      *values_ptr = result[k].second;
    }
  }
}
} // namespace runtime
} // namespace cvi

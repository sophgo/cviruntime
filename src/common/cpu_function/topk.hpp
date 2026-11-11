#include <iostream>
#include <runtime/cpu_function.hpp>
#include <runtime/neuron.hpp>
#include <vector>

namespace cvi {

namespace runtime {
class TopKFunc : public ICpuFunction {
public:
  TopKFunc() {}
  ~TopKFunc();
  void setup(std::vector<std::shared_ptr<Neuron>> &inputs,
             std::vector<std::shared_ptr<Neuron>> &outputs, OpParam &param);
  void run();
  static ICpuFunction *open() { return new TopKFunc(); }
  static void close(ICpuFunction *func) { delete func; }

private:
  std::vector<std::shared_ptr<Neuron>> _bottoms;
  std::vector<std::shared_ptr<Neuron>> _tops;
  int _kvalue;
  int _axis;
  int _sorted;
  int _largest;
};
} // namespace runtime
} // namespace cvi
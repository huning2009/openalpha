#ifndef OPENALPHA_ALPHA_H_
#define OPENALPHA_ALPHA_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "data.h"

namespace openalpha {

inline const std::string KNeutralizationByMarket = "market";
inline const std::string KNeutralizationBySector = "sector";
inline const std::string KNeutralizationByIndustry = "industry";
inline const std::string KNeutralizationBySubIndustry = "subindustry";

class Alpha {
 public:
  typedef std::unordered_map<std::string, std::string> ParamMap;
  Alpha* Initialize(const std::string& name, ParamMap&& params);
  const std::string& name() const { return name_; }
  int delay() const { return delay_; }
  size_t num_dates() const { return num_dates_; }
  size_t num_symbols() const { return num_symbols_; }
  const std::string& GetParam(const std::string& name) const {
    return FindInMap(params_, name);
  }
  const ParamMap& params() const { return params_; }
  DataRegistry& dr() { return dr_; }
  virtual void Generate(int di, double* alpha) = 0;

 private:
  DataRegistry& dr_ = DataRegistry::Instance();
  std::string name_;
  ParamMap params_;
  int universe_ = 3000;
  int lookback_days_ = 256;
  int delay_ = 1;
  int decay_ = 4;
  double max_stock_weight_ = 0.1;
  double book_size_ = 2e6;
  std::string neutralization_ = KNeutralizationBySubIndustry;
  double** alpha_ = nullptr;
  bool** valid_ = nullptr;
  size_t num_dates_ = 0;
  size_t num_symbols_ = 0;
  friend class AlphaRegistry;
  friend class PyAlpha;
};

class PyAlpha : public Alpha {
 public:
  Alpha* Initialize(const std::string& name, ParamMap&& params);
  void Generate(int di, double* alpha) override;

 private:
  bp::object generate_func_;
};

class AlphaRegistry : public Singleton<AlphaRegistry> {
 public:
  typedef std::unordered_map<std::string, Alpha*> AlphaMap;
  void Add(Alpha* alpha) { alphas_[alpha->name()] = alpha; }
  void Run();

 private:
  DataRegistry& dr_ = DataRegistry::Instance();
  AlphaMap alphas_;
};

}  // namespace openalpha

#endif  // OPENALPHA_ALPHA_H_
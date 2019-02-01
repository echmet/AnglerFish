#ifndef CURVEUTILITY_H
#define CURVEUTILITY_H

#include <string>
#include <vector>

namespace gearbox {

class MobilityCurveModel;

class CurveUtility {
public:
  class Block {
  public:
    Block(const double _pH, std::vector<double> _exps, const double _fit, const double _res);
    bool operator>(const Block &other) const;
    bool operator<(const Block &other) const;

    double pH;
    std::vector<double> exps;
    double fit;
    double res;
  };

  CurveUtility() = delete ;

  static std::vector<Block> blockify(const MobilityCurveModel &model);
  static std::string blocksToCSV(const std::vector<Block> &blocks, const char delim);
};

} // namespace gearbox

#endif // CURVEUTILITY_H

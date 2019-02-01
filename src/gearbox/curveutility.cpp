#include "curveutility.h"

#include "doubletostringconvertor.h"
#include "mobilitycurvemodel.h"
#include <QTextStream>
#include <limits>

#define DBL_NAN std::numeric_limits<double>::quiet_NaN()

namespace gearbox {

inline
std::vector<CurveUtility::Block>::iterator contains(std::vector<CurveUtility::Block> &vec, const double pH)
{
  for (auto it = vec.begin(); it != vec.end(); ++it) {
    if (it->pH == pH)
      return it;
  }

  return vec.end();
}

CurveUtility::Block::Block(const double _pH, std::vector<double> _exps, const double _fit, const double _res) :
  pH{_pH},
  exps{std::move(_exps)},
  fit{_fit},
  res{_res}
{
}

bool CurveUtility::Block::operator>(const Block &other) const
{
  return pH > other.pH;
}

bool CurveUtility::Block::operator<(const Block &other) const
{
  return pH < other.pH;
}

std::vector<CurveUtility::Block> CurveUtility::blockify(const MobilityCurveModel &model)
{
  std::vector<Block> blocks{};

  double prevX = DBL_NAN;
  std::vector<double> v{};
  for (const auto &pt : model.experimental()) {
    if (pt.x() != prevX) {
      if (v.size() > 0)
        blocks.emplace_back(prevX, std::move(v), DBL_NAN, DBL_NAN);

      v = {pt.y()};
      prevX = pt.x();
    } else
      v.emplace_back(pt.y());
  }
  if (v.size() > 0)
    blocks.emplace_back(prevX, std::move(v), DBL_NAN, DBL_NAN);

  /* Yes, this is nasty, horrible, dull and slow
   * staircase-behaving stinking pile of garbage */
  auto bind = [&blocks](const QVector<QPointF> &vec, double Block::* field) {
    for (const auto &pt : vec) {
      auto it = contains(blocks, pt.x());
      if (it != blocks.end())
        (*it).*field = pt.y();
      else {
        Block b{pt.x(), {}, DBL_NAN, DBL_NAN};
        b.*field = pt.y();
        blocks.emplace_back(std::move(b));
      }
    }
  };

  bind(model.fitted(), &Block::fit);
  bind(model.residuals(), &Block::res);

  std::sort(blocks.begin(), blocks.end());

  return blocks;
}

std::string CurveUtility::blocksToCSV(const std::vector<Block> &blocks, const char delim)
{
  static const QChar SEP{delim};
  QString data{};

  QTextStream str{&data, QIODevice::WriteOnly};
  str.setCodec("UTF-8");

  str << "pH" << SEP
      << QObject::tr("Experimental") << SEP
      << QObject::tr("Fitted") << SEP
      << QObject::tr("Residual")
      << "\n";

  for (const auto &b : blocks) {
    str << DoubleToStringConvertor::convert(b.pH) << SEP;
        if (!b.exps.empty())
          str << DoubleToStringConvertor::convert(b.exps.front()) << SEP;
        else
          str << SEP;
    str << DoubleToStringConvertor::convert(b.fit) << SEP
        << DoubleToStringConvertor::convert(b.res)
        << "\n";

    for (size_t idx = 1; idx < b.exps.size(); idx++) {
      str << DoubleToStringConvertor::convert(b.pH) << SEP
          << DoubleToStringConvertor::convert(b.exps.at(idx)) << SEP
          << SEP
          << "\n";
    }
  }

  return data.toStdString();
}

} // namespace gearbox

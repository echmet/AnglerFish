#include "curvetoclipboardexporter.h"

#include <trstr.h>
#include <gearbox/gearbox.h>
#include <gearbox/doubletostringconvertor.h>
#include <gearbox/mobilitycurvemodel.h>
#include <QApplication>
#include <QClipboard>
#include <QTextStream>
#include <algorithm>
#include <map>
#include <limits>
#include <cassert>

#define DBL_INF std::numeric_limits<double>::infinity()

namespace gearbox {

class Block {
public:
  Block(const double _pH, std::vector<double> _exps, const double _fit, const double _res) :
    pH{_pH},
    exps{std::move(_exps)},
    fit{_fit},
    res{_res}
  {
  }

  double pH;
  std::vector<double> exps;
  double fit;
  double res;

  bool operator>(const Block &other) const
  {
    return pH > other.pH;
  }

  bool operator<(const Block &other) const
  {
    return pH < other.pH;
  }
};

inline
void writeClipboard(const std::vector<Block> &blocks)
{
  static const QChar SEP{';'};
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

  QApplication::clipboard()->setText(data);
}

inline
std::vector<Block>::iterator contains(std::vector<Block> &vec, const double pH)
{
  for (auto it = vec.begin(); it != vec.end(); ++it) {
    if (it->pH == pH)
      return it;
  }

  return vec.end();
}

void CurveToClipboardExporter::write(const Gearbox &gbox)
{
  const auto &model = gbox.mobilityCurveModel();

  std::vector<Block> blocks{};

  double prevX = DBL_INF;
  std::vector<double> v{};
  for (const auto &pt : model.experimental()) {
    if (pt.x() != prevX) {
      if (v.size() > 0)
        blocks.emplace_back(prevX, std::move(v), DBL_INF, DBL_INF);

      v = {pt.y()};
      prevX = pt.x();
    } else
      v.emplace_back(pt.y());
  }
  if (v.size() > 0)
    blocks.emplace_back(prevX, std::move(v), DBL_INF, DBL_INF);

  /* Yes, this is nasty, horrible, dull and slow
   * staircase-behaving stinking pile of garbage */
  auto bind = [&blocks](const QVector<QPointF> &vec, double Block::* field) {
    for (const auto &pt : vec) {
      auto it = contains(blocks, pt.x());
      if (it != blocks.end())
        (*it).*field = pt.y();
      else {
        Block b{pt.x(), {}, DBL_INF, DBL_INF};
        b.*field = pt.y();
        blocks.emplace_back(std::move(b));
      }
    }
  };

  bind(model.fitted(), &Block::fit);
  bind(model.residuals(), &Block::res);

  std::sort(blocks.begin(), blocks.end());

  writeClipboard(blocks);
}

} // namespace gearbox

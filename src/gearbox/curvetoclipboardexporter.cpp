#include "curvetoclipboardexporter.h"

#include <trstr.h>
#include <gearbox/gearbox.h>
#include <gearbox/doubletostringconvertor.h>
#include <QApplication>
#include <QClipboard>
#include <QTextStream>
#include <algorithm>
#include <map>
#include <limits>
#include <cassert>

#define DBL_INF std::numeric_limits<double>::infinity()

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
    return pH > other.pH;
  }
};

inline
Block & findNearest(std::vector<Block> &blocks, const double pH, double Block::* M)
{
  for (auto &b : blocks) {
    if (qFuzzyCompare(b.pH, pH)) {
      if (b.*M != DBL_INF)
        throw CurveToClipboardExporter::Exception{"Ambiguous datapoint"};

      return b;
    }

    /* This assumes a vector sorted in descending order - which we have */
    if (pH > b.pH)
      return b;
  }

  return blocks.back();
}

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
    assert(b.exps.size() > 0);

    str << DoubleToStringConvertor::convert(b.pH) << SEP
        << DoubleToStringConvertor::convert(b.exps.front()) << SEP
        << DoubleToStringConvertor::convert(b.fit) << SEP
        << DoubleToStringConvertor::convert(b.res)
        << "\n";

    for (auto it = b.exps.cbegin() + 1; it != b.exps.cend(); ++it) {
      str << DoubleToStringConvertor::convert(b.pH) << SEP
          << DoubleToStringConvertor::convert(*it) << SEP
          << SEP
          << "\n";
    }
  }

  QApplication::clipboard()->setText(data);
}

void CurveToClipboardExporter::write()
{
  const auto &model = Gearbox::instance()->mobilityCurveModel();

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

  /* Sort in descending order */
  std::sort(blocks.begin(), blocks.end());

  for (const auto &pt : model.fitted()) {
    const double pH = pt.x();

    auto &b  = findNearest(blocks, pH, &Block::fit);
    b.fit = pt.y();
  }

  for (const auto &pt : model.residuals()) {
    const double pH = pt.x();

    auto &b  = findNearest(blocks, pH, &Block::res);
    b.res = pt.y();
  }

  writeClipboard(blocks);
}

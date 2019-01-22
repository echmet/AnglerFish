#include "curvetoclipboardexporter.h"

#include <gearbox/gearbox.h>
#include <gearbox/doubletostringconvertor.h>
#include <QApplication>
#include <QClipboard>
#include <QTextStream>
#include <map>
#include <limits>


class Block {
public:
  Block(std::vector<double> _exps, const double _fit, const double _res) :
    exps{std::move(_exps)},
    fit{_fit},
    res{_res}
  {
  }

  std::vector<double> exps;
  double fit;
  double res;
};

using Aggregate = std::map<double, Block>;

inline
void writeClipboard(const Aggregate &aggr)
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

  for (const auto &item : aggr) {
    const auto &c = item.second;

    assert(c.exps.size() > 0);

    str << DoubleToStringConvertor::convert(item.first) << SEP
        << DoubleToStringConvertor::convert(c.exps.front()) << SEP
        << DoubleToStringConvertor::convert(c.fit) << SEP
        << DoubleToStringConvertor::convert(c.res)
        << "\n";

    for (auto it = c.exps.cbegin() + 1; it != c.exps.cend(); ++it) {
      str << DoubleToStringConvertor::convert(item.first) << SEP
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


  Aggregate aggr{};

  double prevX = std::numeric_limits<double>::infinity();
  std::vector<double> v{};
  for (const auto &pt: model.experimental()) {
    if (pt.x() != prevX) {
      if (v.size() > 0) {
        Block b{std::move(v), 0.0, 0.0};
        aggr.emplace(prevX, std::move(b));
      }
      v = {pt.y()};
      prevX = pt.x();
    } else
      v.emplace_back(pt.y());
  }
  if (v.size() > 0) {
    Block b{std::move(v), 0.0, 0.0};
    aggr.emplace(prevX, std::move(b));
  }

  for (const auto &pt : model.fitted()) {
    auto it = aggr.find(pt.x());
    assert(it != aggr.end());

    it->second.fit = pt.y();
  }

  for (const auto &pt : model.residuals()) {
    auto it = aggr.find(pt.x());
    assert(it != aggr.end());

    it->second.fit = pt.y();
  }

  writeClipboard(aggr);
}

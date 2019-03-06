#include "mobilitycurvemodel.h"

#include <limits>

namespace gearbox {

MobilityCurveModel::MobilityCurveModel() :
  QObject{nullptr}
{
}

QVector<QPointF> MobilityCurveModel::compact(const QVector<QPointF> &src)
{
  QVector<QPointF> compacted{};

  double xPrev = std::numeric_limits<double>::quiet_NaN();
  double ySum{0.0};
  size_t ySumCtr{0};
  for (const auto &pt : src) {
    const double x = pt.x();

    if (xPrev != x) {
      if (ySumCtr > 0)
        compacted.push_back({xPrev, ySum / ySumCtr});

      ySum = pt.y();
      ySumCtr = 0;
      xPrev = x;
    } else
      ySum += pt.y();

    ySumCtr++;
  }
  if (ySumCtr > 0)
    compacted.push_back({xPrev, ySum / ySumCtr});

  return compacted;
}

MobilityCurveModel::ExperimentalPack MobilityCurveModel::experimental() const
{
  return { m_experimentalIncl, m_experimentalExcl };
}

const QVector<QPointF> & MobilityCurveModel::fitted() const
{
  return m_fitted;
}

const QVector<QPointF> & MobilityCurveModel::residuals() const
{
  return m_residuals;
}

void MobilityCurveModel::invalidateAll()
{
  setExperimental({}, {});
  setFitted({}, {});
}

void MobilityCurveModel::invalidateResults()
{
  setFitted({}, {});
}

void MobilityCurveModel::setExperimental(QVector<QPointF> included, QVector<QPointF> excluded)
{
  m_experimentalIncl = std::move(included);
  m_experimentalExcl = std::move(excluded);

  emit experimentalChanged();
}

void MobilityCurveModel::setFitted(QVector<QPointF> fitted, QVector<QPointF> residuals)
{
  m_fitted = std::move(fitted);
  m_residuals = std::move(residuals);

  emit fittedChanged();
}

} // namespace gearbox

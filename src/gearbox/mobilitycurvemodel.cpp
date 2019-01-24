#include "mobilitycurvemodel.h"

#include <limits>

MobilityCurveModel::MobilityCurveModel() :
  QObject{nullptr}
{
}

QVector<QPointF> MobilityCurveModel::compact(const QVector<QPointF> &src)
{
  QVector<QPointF> compacted{};

  double xPrev = std::numeric_limits<double>::infinity();
  double ySum{0.0};
  size_t ySumCtr{0};
  for (const auto &pt : src) {
    const double x = pt.x();

    if (xPrev != x) {
      if (ySumCtr > 0) {
        compacted.push_back({xPrev, ySum / ySumCtr});
        ySum = pt.y();
        ySumCtr = 0;
      }

      xPrev = x;
    } else
      ySum += pt.y();

    ySumCtr++;
  }
  if (ySumCtr > 0)
    compacted.push_back({xPrev, ySum / ySumCtr});

  return compacted;
}

const QVector<QPointF> & MobilityCurveModel::experimental() const
{
  return m_experimental;
}

const QVector<QPointF> & MobilityCurveModel::fitted() const
{
  return m_fitted;
}

const QVector<QPointF> & MobilityCurveModel::residuals() const
{
  return m_residuals;
}

void MobilityCurveModel::invalidate()
{
  setExperimental({});
  setFitted({});
  setResiduals({});
}

void MobilityCurveModel::setExperimental(QVector<QPointF> data)
{
  m_experimental = std::move(data);

  emit experimentalChanged();
}

void MobilityCurveModel::setFitted(QVector<QPointF> data)
{
  m_fitted = std::move(data);

  emit fittedChanged();
}

void MobilityCurveModel::setResiduals(QVector<QPointF> data)
{
  m_residuals = std::move(data);

  emit residualsChanged();
}

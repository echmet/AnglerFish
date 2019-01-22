#include "mobilitycurvemodel.h"

MobilityCurveModel::MobilityCurveModel() :
  QObject{nullptr}
{
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
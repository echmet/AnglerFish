#ifndef MOBILITYCURVEMODEL_H
#define MOBILITYCURVEMODEL_H

#include <QObject>
#include <QPointF>
#include <QVector>

class MobilityCurveModel : public QObject {
  Q_OBJECT
public:
  explicit MobilityCurveModel();

  const QVector<QPointF> &experimental() const;
  const QVector<QPointF> &fitted() const ;
  const QVector<QPointF> &residuals() const;

  void setExperimental(QVector<QPointF> data);
  void setFitteed(QVector<QPointF> data);
  void setResiduals(QVector<QPointF> data);

private:
  QVector<QPointF> m_experimental;
  QVector<QPointF> m_fitted;
  QVector<QPointF> m_residuals;

signals:
  void experimentalChanged();
  void fittedChanged();
  void residualsChanged();
};

#endif // MOBILITYCURVEMODEL_H

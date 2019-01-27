#ifndef MOBILITYCURVEMODEL_H
#define MOBILITYCURVEMODEL_H

#include <QObject>
#include <QPointF>
#include <QVector>

namespace gearbox {

class MobilityCurveModel : public QObject {
  Q_OBJECT
public:
  explicit MobilityCurveModel();

  const QVector<QPointF> &experimental() const;
  const QVector<QPointF> &fitted() const ;
  const QVector<QPointF> &residuals() const;

  void invalidateAll();
  void invalidateResults();

  void setExperimental(QVector<QPointF> data);
  void setFitted(QVector<QPointF> fitted, QVector<QPointF> residuals);

  static QVector<QPointF> compact(const QVector<QPointF> &src);

private:
  QVector<QPointF> m_experimental;
  QVector<QPointF> m_fitted;
  QVector<QPointF> m_residuals;

signals:
  void experimentalChanged();
  void fittedChanged();
};

} // namespace gearbox

#endif // MOBILITYCURVEMODEL_H

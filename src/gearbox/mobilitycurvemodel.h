#ifndef MOBILITYCURVEMODEL_H
#define MOBILITYCURVEMODEL_H

#include <QObject>
#include <QPointF>
#include <QVector>

namespace gearbox {

class MobilityCurveModel : public QObject {
  Q_OBJECT
public:
  using ExperimentalPack = std::tuple<const QVector<QPointF> &, const QVector<QPointF> &>;
  explicit MobilityCurveModel();

  ExperimentalPack experimental() const;
  const QVector<QPointF> &fitted() const ;
  const QVector<QPointF> &residuals() const;

  void invalidateAll();
  void invalidateResults();

  void setExperimental(QVector<QPointF> included, QVector<QPointF> excluded);
  void setFitted(QVector<QPointF> fitted, QVector<QPointF> residuals);

  static QVector<QPointF> compact(const QVector<QPointF> &src);

private:
  QVector<QPointF> m_experimentalIncl;
  QVector<QPointF> m_experimentalExcl;
  QVector<QPointF> m_fitted;
  QVector<QPointF> m_residuals;

signals:
  void experimentalChanged();
  void fittedChanged();
};

} // namespace gearbox

#endif // MOBILITYCURVEMODEL_H

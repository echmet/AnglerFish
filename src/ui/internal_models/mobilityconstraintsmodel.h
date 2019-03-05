#ifndef MOBILITYCONSTRAINTSMODEL_H
#define MOBILITYCONSTRAINTSMODEL_H

#include <QAbstractTableModel>

#include <QVector>

namespace gearbox {
class LimitMobilityConstraintsModel;
} // namespace gearbox

class MobilityConstraintsModel : public QAbstractTableModel {
  Q_OBJECT

  class MobInfo {
  public:
    MobInfo() noexcept;
    MobInfo(const int chg, const double mob, const double lower, const double upper) noexcept;

    int charge;
    double mobility;
    double lowerBound;
    double upperBound;
  };

public:
  class EstimatedMobility {
  public:
    int charge;
    double mobility;
  };

  explicit MobilityConstraintsModel(const gearbox::LimitMobilityConstraintsModel &backend,
                                    QObject *parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // Basic functionality:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  void updateConstraints(const int chargeLow, const int chargeHigh,
                         const QVector<EstimatedMobility> &estimates);
private:
  enum Item {
    IT_MOBILITY,
    IT_LOW_CONSTR,
    IT_UP_CONSTR
  };

  QVector<MobInfo> m_data;

  const gearbox::LimitMobilityConstraintsModel &h_backend;

};

#endif // MOBILITYCONSTRAINTSMODEL_H

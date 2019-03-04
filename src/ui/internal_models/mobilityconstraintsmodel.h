#ifndef MOBILITYCONSTRAINTSMODEL_H
#define MOBILITYCONSTRAINTSMODEL_H

#include <QAbstractTableModel>

#include <tuple>
#include <QVector>

namespace gearbox {
class LimitMobilityConstraintsModel;
} // namespace gearbox

class MobilityConstraintsModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit MobilityConstraintsModel(const gearbox::LimitMobilityConstraintsModel &backend,
                                    QObject *parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // Basic functionality:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
  enum Item {
    IT_MOBILITY,
    IT_LOW_CONSTR,
    IT_UP_CONSTR
  };

  QVector<std::tuple<int, double, double, double>> m_data;

  const gearbox::LimitMobilityConstraintsModel &h_backend;

public slots:
  void estimatesUpdated(const int chargeLow, const int chargeHigh);
};

#endif // MOBILITYCONSTRAINTSMODEL_H

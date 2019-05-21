#ifndef MOBILITYCONSTRAINTSMODEL_H
#define MOBILITYCONSTRAINTSMODEL_H

#include <QAbstractTableModel>

#include <QBrush>
#include <QVector>
#include <gearbox/analyteestimates.h>

class QPalette;

namespace gearbox {
class Gearbox;
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
                                    gearbox::Gearbox &gbox,
                                    const QPalette &palette,
                                    QObject *parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // Basic functionality:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

  void updateModel(const int chargeLow, const int chargeHigh,
                   const gearbox::AnalyteEstimates::ParameterVec &mobilities);
private:
  enum Item {
    IT_MOBILITY,
    IT_LOW_CONSTR,
    IT_UP_CONSTR
  };

  QVariant displayValidity(const int row, const int col) const;
  bool hasPrev(const int charge, const int row) const;

  QVector<MobInfo> m_data;

  QBrush m_defBrush;
  QBrush m_invalBrush;

  const gearbox::LimitMobilityConstraintsModel &h_backend;
  gearbox::Gearbox &h_gbox;
  const QPalette &h_palette;

private slots:
  void onEstimatesUpdated();


};

#endif // MOBILITYCONSTRAINTSMODEL_H

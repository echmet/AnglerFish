#ifndef CONSTITUENTCHARGESMODEL_H
#define CONSTITUENTCHARGESMODEL_H

#include "abstractconstituentchargesmodel.h"

#include <QAbstractTableModel>
#include <tuple>
#include <QVector>

class ConstituentChargesModel : public AbstractConstituentChargesModel {
  Q_OBJECT

  using ChargeBlock = std::tuple<int, qreal, qreal>;
public:
  enum HeaderCaptions : int {
    MOBILITY = 0,
    PKA = 1
  };

  explicit ConstituentChargesModel(QObject *parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

  // Basic functionality:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  // Editable:
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole) override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;

  // Add data:
  bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
  bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

  virtual bool isBaseCharge(const int charge) const override;

  // Remove data:
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
  bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

  virtual void refreshData(const std::map<int, double> &pKas, const std::map<int, double> &mobilities,
                           const int chargeLow, const int chargeHigh) override;

private:
  bool isBaseCharge(const QModelIndex &index) const;

  QVector<ChargeBlock> m_charges;
};

#endif // CONSTITUENTCHARGESMODEL_H

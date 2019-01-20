#ifndef ABSTRACTCONSTITUENTCHARGESMODEL_H
#define ABSTRACTCONSTITUENTCHARGESMODEL_H

#include <QAbstractTableModel>

class AbstractConstituentChargesModel : public QAbstractTableModel {
public:
  using QAbstractTableModel::QAbstractTableModel;

  virtual bool isBaseCharge(const int charge) const = 0;
  virtual void refreshData(const std::map<int, double> &pKas, const std::map<int, double> &mobilities,
                           const int chargeLow, const int chargeHigh) = 0;
};

#endif // ABSTRACTCONSTITUENTCHARGESMODEL_H

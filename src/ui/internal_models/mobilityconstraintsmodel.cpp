#include "mobilityconstraintsmodel.h"

#include <gearbox/limitmobilityconstraintsmodel.h>

MobilityConstraintsModel::MobilityConstraintsModel(const gearbox::LimitMobilityConstraintsModel &backend,
                                                   QObject *parent) :
  QAbstractTableModel{parent},
  h_backend{backend}
{
}

QVariant MobilityConstraintsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return {};

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case IT_MOBILITY:
      return "Mobility";
    case IT_LOW_CONSTR:
      return "Lower constr.";
    case IT_UP_CONSTR:
      return "Upper constr.";
    default:
      return {};
    }
  } else if (orientation == Qt::Vertical) {
    if (section >= rowCount())
      return {};

    const auto &item = m_data.at(section);
    return std::get<0>(item);
  }

  return {};
}

int MobilityConstraintsModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return m_data.size();
}

int MobilityConstraintsModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return 3;
}

QVariant MobilityConstraintsModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return {};
  if (role != Qt::DisplayRole)
    return {};

  const auto col = index.column();
  const auto row = index.row();

  if (col < 0 || col >= columnCount() || row < 0 || row >= rowCount())
    return {};

  const auto &item = m_data.at(row);
  const auto charge = std::get<0>(item);
  auto getLow = [&item, charge]() -> QVariant {
    if (charge == 0)
      return "-";
    return std::get<2>(item);
  };
  auto getUp = [&item, charge]() -> QVariant {
    if (charge == 0)
      return "-";
    return std::get<3>(item);
  };

  switch (col) {
  case IT_MOBILITY:
    return std::get<1>(item);
  case IT_LOW_CONSTR:
    return getLow();
  case IT_UP_CONSTR:
    return getUp();
  }

  return {};
}

void MobilityConstraintsModel::estimatesUpdated(const int chargeLow, const int chargeHigh)
{
  beginResetModel();

  endResetModel();
}

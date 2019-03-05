#include "mobilityconstraintsmodel.h"

#include <gearbox/limitmobilityconstraintsmodel.h>
#include <cassert>

MobilityConstraintsModel::MobInfo::MobInfo() noexcept :
  charge{0},
  mobility{0.0},
  lowerBound{0.0},
  upperBound{0.0}
{
}

MobilityConstraintsModel::MobInfo::MobInfo(const int chg, const double mob, const double lower, const double upper) noexcept :
  charge{chg},
  mobility{mob},
  lowerBound{lower},
  upperBound{upper}
{
}

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
    return item.charge;
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
  const auto charge = item.charge;
  auto getLow = [&item, charge]() -> QVariant {
    if (charge == 0)
      return "-";
    return item.lowerBound;
  };
  auto getUp = [&item, charge]() -> QVariant {
    if (charge == 0)
      return "-";
    return item.upperBound;
  };

  switch (col) {
  case IT_MOBILITY:
    return item.mobility;
  case IT_LOW_CONSTR:
    return getLow();
  case IT_UP_CONSTR:
    return getUp();
  }

  return {};
}

void MobilityConstraintsModel::updateConstraints(const int chargeLow, const int chargeHigh,
                                                 const QVector<EstimatedMobility> &estimates)
{
  beginResetModel();

  int size = chargeHigh - chargeLow;
  assert(size > 0);

  m_data.clear();
  m_data.reserve(size);

  for (const auto &item : estimates) {
    const auto cstrs = h_backend.constraintsForMobility(item.mobility);

    m_data.push_back({ item.charge, item.mobility, cstrs.low, cstrs.high });
  }

  std::sort(m_data.begin(), m_data.end(), [](const MobInfo &lhs, const MobInfo &rhs) { return lhs.charge < rhs.charge; });

  endResetModel();
}

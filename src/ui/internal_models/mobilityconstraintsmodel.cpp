#include "mobilityconstraintsmodel.h"

#include <QPalette>
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
                                                   gearbox::Gearbox &gbox,
                                                   const QPalette &palette,
                                                   QObject *parent) :
  QAbstractTableModel{parent},
  m_defBrush{palette.foreground()},
  m_invalBrush{palette.foreground()},
  h_backend{backend},
  h_gbox{gbox},
  h_palette{palette}
{
  m_invalBrush.setColor(Qt::red);
}

bool MobilityConstraintsModel::hasPrev(const int charge, const int row) const
{
  if (charge > 1) {
    if (row > 0)
      return m_data.at(row - 1).charge > 0;
    return false;
  } else {
    if (row < m_data.size() - 1)
      return m_data.at(row + 1).charge < 0;
    return false;
  }
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

  const auto col = index.column();
  const auto row = index.row();

  if (col < 0 || col >= columnCount() || row < 0 || row >= rowCount())
    return {};

  if (role == Qt::ForegroundRole)
    return displayValidity(row, col);
  if (role != Qt::DisplayRole)
    return {};

  const auto &item = m_data.at(row);
  const auto charge = item.charge;
  auto getLow = [&item, charge, row, this]() -> QVariant {
    if (!hasPrev(charge, row))
      return "-";
    return item.lowerBound;
  };
  auto getUp = [&item, charge, row, this]() -> QVariant {
    if (!hasPrev(charge, row))
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

QVariant MobilityConstraintsModel::displayValidity(const int row, const int col) const
{
  if (col > 0)
    return m_defBrush;

  const auto &item = m_data[row];
  const int charge = item.charge;
  if (charge >= -1 && charge <= 1)
    return m_defBrush;

  if (!hasPrev(charge, row))
    return m_defBrush;

  const bool isWithinConstrs = item.mobility > item.lowerBound && item.mobility < item.upperBound;

  return isWithinConstrs ? m_defBrush : m_invalBrush;
}

void MobilityConstraintsModel::updateConstraints(const int chargeLow, const int chargeHigh,
                                                 QVector<EstimatedMobility> estimates)
{
  std::sort(estimates.begin(), estimates.end(), [](const EstimatedMobility &lhs, const EstimatedMobility &rhs) { return lhs.charge < rhs.charge; });

  beginResetModel();

  int size = chargeHigh - chargeLow;
  assert(size >= 0);

  m_data.clear();
  m_data.reserve(size);

  for (auto &item : estimates) {
    if (item.charge == 0)
      continue;

    const auto ret = h_backend.constraintsForCharge(item.charge, h_gbox);
    m_data.push_back({ item.charge, item.mobility, ret.low, ret.high });
  }

  endResetModel();
}

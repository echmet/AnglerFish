#include "mobilityconstraintsmodel.h"

#include <QPalette>
#include <gearbox/limitmobilityconstraintsmodel.h>
#include <gearbox/gearbox.h>
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

  updateModel(h_gbox.analyteEstimates().chargeLow, h_gbox.analyteEstimates().chargeHigh,
              h_gbox.analyteEstimates().mobilities);

  connect(&h_gbox, &gearbox::Gearbox::analyteEstimatesChanged, this, &MobilityConstraintsModel::onEstimatesUpdated);
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

Qt::ItemFlags MobilityConstraintsModel::flags(const QModelIndex &index) const
{
  static const auto def = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  if (!index.isValid())
    return Qt::NoItemFlags;

  if (index.row() >= m_data.size())
    return Qt::NoItemFlags;

  if (index.column() > 0)
    return def;

  const auto &item = m_data.at(index.row());
  if (item.charge != 0)
    return def | Qt::ItemIsEditable;

  return def;
}

bool MobilityConstraintsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid())
    return false;

  const auto col = index.column();
  const auto row = index.row();

  if (col < 0 || col >= columnCount() || row < 0 || row >= rowCount())
    return false;
  if (role != Qt::EditRole)
    return false;
  if (col > 0)
    return false;

  if (!value.canConvert<double>())
    return false;

  const double uEst = value.toDouble(nullptr);

  const auto &estimates = h_gbox.analyteEstimates();
  auto mobilities = estimates.mobilities;
  if (mobilities.at(row).value == uEst)
    return false;

  mobilities[row] = gearbox::AnalyteEstimates::Parameter{uEst, mobilities.at(row).fixed};

  h_gbox.setAnalyteEstimates(estimates.chargeLow, estimates.chargeHigh,
                             mobilities, estimates.pKas);

  updateModel(estimates.chargeLow, estimates.chargeHigh, mobilities);

  emit dataChanged(index, index);

  return true;
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

void MobilityConstraintsModel::onEstimatesUpdated()
{
  updateModel(h_gbox.analyteEstimates().chargeLow, h_gbox.analyteEstimates().chargeHigh,
              h_gbox.analyteEstimates().mobilities);
}

void MobilityConstraintsModel::updateModel(const int chargeLow, const int chargeHigh,
                                           const gearbox::AnalyteEstimates::ParameterVec &mobilities)
{
  beginResetModel();

  int size = chargeHigh - chargeLow;
  assert(size >= 0);

  m_data.clear();
  m_data.reserve(size);

  int charge = chargeLow;
  for (const auto &item : mobilities) {
    if (charge == 0)
      m_data.push_back({ 0, 0, 0, 0});
    else {
      const auto ret = h_backend.constraintsForCharge(charge, h_gbox);
      m_data.push_back({ charge, item.value, ret.low, ret.high });
    }

    charge++;
  }

  endResetModel();
}

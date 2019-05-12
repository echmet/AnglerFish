#include "constituentchargesmodelfixable.h"

#include <cassert>


ConstituentChargesModelFixable::ConstituentChargesModelFixable(QObject *parent)
  : AbstractConstituentChargesModel{parent}
{
  m_charges.push_back({0, 0.0, 0.0, Qt::Unchecked, Qt::Unchecked});
}

QVariant ConstituentChargesModelFixable::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical) {
    if (role != Qt::DisplayRole)
      return {};

    if (m_charges.size() <= section)
      return {};

    return std::get<0>(m_charges.at(section));
  }

  if (role == Qt::DisplayRole) {
    switch (section) {
    case MOBILITY:
      return {tr("Mobility")};
    case PKA:
      return {"pKa"};
    case FIX_MOBILITY:
      return {tr("Fix mobility")};
    case FIX_PKA:
      return {tr("Fix pKa")};
    }
  }

  return {};
}

bool ConstituentChargesModelFixable::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  Q_UNUSED(section) Q_UNUSED(orientation) Q_UNUSED(value) Q_UNUSED(role)

  return false;
}

int ConstituentChargesModelFixable::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)

  assert(!m_charges.empty());
  return static_cast<int>(m_charges.size());
}

int ConstituentChargesModelFixable::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)

  return 4;
}

QVariant ConstituentChargesModelFixable::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return {};

  const int row = index.row();
  if (m_charges.size() <= row)
    return {};

  if (role == Qt::DisplayRole) {
    const int col = index.column();
    if (col > 2)
      return {};

    switch (col) {
    case 0:
      return std::get<1>(m_charges.at(row));
    case 1:
    {
      if (isBaseCharge(index))
        return "-";

      return std::get<2>(m_charges.at(row));
    }
    }
  } else if (role == Qt::CheckStateRole) {
    const int col = index.column();

    if ((isBaseCharge(index) && col == 3) || std::get<0>(m_charges.at(row)) == 0)
      return {};

    switch (col) {
    case 2:
      return std::get<3>(m_charges.at(row));
    case 3:
      return std::get<4>(m_charges.at(row));
    default:
      return {};
    }
  }

  return {};
}

bool ConstituentChargesModelFixable::setData(const QModelIndex &index, const QVariant &value, int role)
{
  const int row = index.row();

  if (!(role == Qt::EditRole || role == Qt::CheckStateRole))
    return false;

  if (data(index, role) != value) {
    const int col = index.column();
    if (m_charges.size() <= row || col > 3)
      return false;

    /* Properties of zero charge are immutable */
    if (std::get<0>(m_charges.at(row)) == 0)
      return false;

    auto &data = m_charges[row];

    if (col < 2) {
      bool isReal;
      const qreal realVal = value.toReal(&isReal);
      if (!isReal)
        return false;

      switch (col) {
      case 0:
        return updateIfNeeded<1, qreal>(data, realVal, index, role);
      case 1:
        return updateIfNeeded<2, qreal>(data, realVal, index, role);
      }
    } else {
      bool ok;
      const int checked = value.toInt(&ok);

      if (!ok)
        return {};

      switch (col) {
      case 2:
        return updateIfNeeded<3, int>(data, checked, index, role);
      case 3:
        return updateIfNeeded<4, int>(data, checked, index, role);
      }
    }
  }
  return false;
}

Qt::ItemFlags ConstituentChargesModelFixable::flags(const QModelIndex &index) const
{
  Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (!index.isValid())
    return Qt::NoItemFlags;

  const int row = index.row();
  const int col = index.column();

  if (m_charges.size() <= row || col > 3)
    return Qt::NoItemFlags;

  if (std::get<0>(m_charges.at(row)) == 0)
    return defaultFlags;
  /* Changing pKa value of the base charge makes no sense */
 if (isBaseCharge(index) && index.column() == 1)
   return defaultFlags;

  if (col > 1)
    return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
  else
    return defaultFlags | Qt::ItemIsEditable;
}

bool ConstituentChargesModelFixable::insertRows(int row, int count, const QModelIndex &parent)
{
  if (m_charges.size() < row)
    return false;

  beginInsertRows(parent, row, row + count - 1);
  const int fromCharge = [row, this]() {
    if (m_charges.size() == row)
      return std::get<0>(m_charges.at(row - 1));
    return std::get<0>(m_charges.at(row));
  }();
  const int toCharge = [row, count, fromCharge]() {
    if (row == 0)
      return fromCharge - count;
    return fromCharge + count;
  }();

  auto backup = m_charges; // TODO: Figure out something safer!
  try {
    if (fromCharge < toCharge) {
      for (int charge = fromCharge + 1; charge <= toCharge; charge++)
        m_charges.push_back({charge, 0.0, 0.0, Qt::Unchecked, Qt::Unchecked});
    } else {
      for (int charge = fromCharge - 1; charge >= toCharge; charge--)
        m_charges.push_front({charge, 0.0, 0.0, Qt::Unchecked, Qt::Unchecked});
    }
  } catch (std::bad_alloc &) {
    m_charges = backup;
    endInsertRows();
    return false;
  }

  endInsertRows();
  return true;
}

bool ConstituentChargesModelFixable::insertColumns(int column, int count, const QModelIndex &parent)
{
  Q_UNUSED(column) Q_UNUSED(count) Q_UNUSED(parent)

  return false;
}

bool ConstituentChargesModelFixable::isBaseCharge(const int charge) const
{
  const int chargeLow = std::get<0>(m_charges.front());
  const int chargeHigh = std::get<0>(m_charges.back());

  assert(charge >= chargeLow && charge <= chargeHigh);

  return isBaseCharge(createIndex(charge - chargeLow, 0));
}

bool ConstituentChargesModelFixable::isBaseCharge(const QModelIndex &index) const
{
  const int row = index.row();
  const int chargeLow = std::get<0>(m_charges.front());
  const int chargeHigh = std::get<0>(m_charges.back());

  if (chargeHigh < 0 && row == m_charges.size() - 1)
    return true;
  if (chargeLow > 0 && row == 0)
    return true;

  return std::get<0>(m_charges.at(row)) == 0;
}

bool ConstituentChargesModelFixable::removeRows(int row, int count, const QModelIndex &parent)
{
  assert(row >= 0);
  assert(count > 0);

  if (m_charges.size() <= row)
    return false;
  if (m_charges.size() == 1)
    return false;

  beginRemoveRows(parent, row, row + count - 1);
  QVector<ChargeBlock>::iterator from = m_charges.begin() + row;
  QVector<ChargeBlock>::iterator to = from + count;

  m_charges.erase(from, to);
  endRemoveRows();

  return true;
}

void ConstituentChargesModelFixable::refreshData(const std::map<int, double> &pKas, const std::map<int, double> &mobilities,
                           const int chargeLow, const int chargeHigh)
{
  assert(pKas.size() == mobilities.size());
  assert(pKas.size() == static_cast<size_t>(chargeHigh - chargeLow + 1));

  beginResetModel();

  m_charges.clear();

  for (int charge = chargeLow; charge <= chargeHigh; charge++) {
    const auto &pKa = pKas.at(charge);
    const auto &mobility = mobilities.at(charge);

    m_charges.push_back({charge, mobility, pKa, Qt::Unchecked, Qt::Unchecked});
  }

  endResetModel();

}

void ConstituentChargesModelFixable::refreshData(const std::map<int, std::pair<double, bool>> &pKas,
                                                 const std::map<int, std::pair<double, bool>> &mobilities,
                                                 const int chargeLow, const int chargeHigh)
{
  assert(pKas.size() == mobilities.size());
  assert(pKas.size() == static_cast<size_t>(chargeHigh - chargeLow + 1));

  beginResetModel();

  m_charges.clear();

  for (int charge = chargeLow; charge <= chargeHigh; charge++) {
    const auto &pKa = pKas.at(charge);
    const auto &mobility = mobilities.at(charge);

    m_charges.push_back({charge, mobility.first, pKa.first,
                         mobility.second ? Qt::Checked : Qt::Unchecked ,
                         pKa.second ? Qt::Checked : Qt::Unchecked});
  }

  endResetModel();
}

bool ConstituentChargesModelFixable::removeColumns(int column, int count, const QModelIndex &parent)
{
  Q_UNUSED(column) Q_UNUSED(count) Q_UNUSED(parent)

  return false;
}

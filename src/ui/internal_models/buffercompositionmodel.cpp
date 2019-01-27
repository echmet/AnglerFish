#include "buffercompositionmodel.h"

#include <util_lowlevel.h>
#include <gearbox/gdmproxy.h>

BufferCompositionModel::BufferCompositionModel(gearbox::GDMProxy &proxy, QObject *parent) :
  QAbstractTableModel{parent},
  h_proxy{proxy}
{
  for (auto it = proxy.cbegin(); it != proxy.cend(); it++)
    m_names.append(QString::fromStdString(it->name()));
}

void BufferCompositionModel::add(QString name)
{
  const int idx = m_names.size();

  beginInsertRows({}, idx, idx);
  m_names.append(std::move(name));
  endInsertRows();
}

int BufferCompositionModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return 2;
}

QVariant BufferCompositionModel::data(const QModelIndex &index, int role) const
{
  const int row = index.row();
  const int col = index.column();

  if (col < 0 || col >= columnCount())
    return {};
  if (row < 0 || row >= rowCount())
    return {};

  const auto &name = m_names.at(row);

  if (role == Qt::UserRole)
    return name;

  if (role != Qt::DisplayRole)
    return {};

  switch (col) {
  case 0:
    return name;
  case 1:
    return h_proxy.concentrations(name.toStdString()).at(0);
  }

  return {};
}


Qt::ItemFlags BufferCompositionModel::flags(const QModelIndex &idx) const
{
  Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (!idx.isValid())
    return Qt::NoItemFlags;

  if (idx.column() == 1)
    return defaultFlags | Qt::ItemIsEditable;

  return defaultFlags;
}

QVariant BufferCompositionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (section < 0 || section >= columnCount())
    return {};

  if (orientation != Qt::Horizontal)
    return {};
  if (role != Qt::DisplayRole)
    return {};

  switch (section) {
  case 0:
    return "Name";
  case 1:
    return "Concentration (mM)";
  }

  return {};
}

QString BufferCompositionModel::remove(const int row)
{
  if (row < 0 || row >= m_names.size())
    return {};

  QString name;
  beginRemoveRows({}, row, row);
  name = m_names.takeAt(row);
  endRemoveRows();

  return name;
}

int BufferCompositionModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return m_names.size();
}

bool BufferCompositionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid())
    return false;

  const int row = index.row();
  if (row < 0 || row >= m_names.size())
    return false;

  bool ok;
  const double v = value.toReal(&ok);
  if (!ok)
    return false;

  const auto &name = m_names.at(row);

  h_proxy.setConcentrations(name.toStdString(), {v});
  emit dataChanged(index, index, { role });
  return true;
}

void BufferCompositionModel::updateName(const QString &oldName, const QString &newName)
{
  for (auto &n : m_names) {
    if (n == oldName) {
      n = newName;
      return;
    }
  }

  IMPOSSIBLE_PATH;
}



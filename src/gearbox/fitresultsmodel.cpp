#include "fitresultsmodel.h"

namespace gearbox {

FitResultsModel::FitResultsModel(QString name, QObject *parent) :
  QAbstractTableModel{parent},
  m_name{std::move(name)}
{
}

int FitResultsModel::columnCount(const QModelIndex &index) const
{
  if (index.isValid())
    return 0;
  return 3;
}

QVariant FitResultsModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return {};

  const int col = index.column();
  const int row = index.row();

  if (row < 0 || row >= rowCount())
    return 0;

  const auto &item = m_data.at(row);

  if (role == Qt::DisplayRole) {
    switch (col) {
    case ID_VALUE:
      return item.value;
    case ID_ABS_ERR:
      return item.absoluteError;
    case ID_REL_ERR:
      return item.relativeError;
    }
  } else if (role == Qt::UserRole + 1)
    return item.charge;

  return {};
}

QVariant FitResultsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return {};

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case ID_VALUE:
      return m_name;
    case ID_ABS_ERR:
      return tr("Abs. error");
    case ID_REL_ERR:
      return tr("Rel. error (%)");
    }
  } else {
    if (section >= 0 && section < rowCount())
      return m_data.at(section).charge;
  }

  return {};
}

int FitResultsModel::rowCount(const QModelIndex &index) const
{
  if (index.isValid())
    return 0;
  return m_data.size();
}

void FitResultsModel::setNewData(QVector<Result> data)
{
  beginResetModel();
  m_data = std::move(data);
  endResetModel();
}

} // namespace gearbox

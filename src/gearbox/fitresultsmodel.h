#ifndef FITRESULTSMODEL_H
#define FITRESULTSMODEL_H

#include <QAbstractTableModel>
#include <QVector>

namespace gearbox {

class FitResultsModel : public QAbstractTableModel {
public:
  enum DataID : int {
    ID_VALUE = 0,
    ID_ABS_ERR = 1,
    ID_REL_ERR = 2
  };

  class Result {
  public:
    int charge;
    double value;
    double absoluteError;
    double relativeError;
  };

  explicit FitResultsModel(const QString &name, QObject *parent = nullptr);
  virtual int columnCount(const QModelIndex &index = {}) const override;
  virtual QVariant data(const QModelIndex &index, int role) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  virtual int rowCount(const QModelIndex &index = {}) const override;
  void setNewData(QVector<Result> data);

private:
  QVector<Result> m_data;
  QString m_name;
};

} // namespace gearbox

#endif // FITRESULTSMODEL_H

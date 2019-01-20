#ifndef BUFFERCOMPOSITIONMODEL_H
#define BUFFERCOMPOSITIONMODEL_H

#include <QAbstractTableModel>

class GDMProxy;

class BufferCompositionModel : public QAbstractTableModel {
  Q_OBJECT
public:
  BufferCompositionModel(GDMProxy &proxy, QObject *parent = nullptr);

  void add(QString name);
  virtual int columnCount(const QModelIndex &parent = {}) const override;
  virtual QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  virtual Qt::ItemFlags flags(const QModelIndex &idx) const override;
  QString remove(const int row);
  virtual int rowCount(const QModelIndex &parent = {}) const override;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  void updateName(const QString &oldName, const QString &newName);

private:
  GDMProxy &h_proxy;
  QVector<QString> m_names;
};

#endif // BUFFERCOMPOSITIONMODEL_H

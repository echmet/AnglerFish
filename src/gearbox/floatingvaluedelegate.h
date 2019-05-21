#ifndef FLOATINGVALUEDELEGATE_H
#define FLOATINGVALUEDELEGATE_H

#include <QLocale>
#include <QStyledItemDelegate>

namespace gearbox {

class FloatingValueDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  FloatingValueDelegate(QObject *parent = nullptr);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  QString displayText(const QVariant &value, const QLocale &locale) const override;
  void forceCommit();
  void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private slots:
  void onTextChanged(const QString &);

signals:
  void editorCommit();

};

} // namespace gearbox

#endif // FLOATINGVALUEDELEGATE_H

#include "floatingvaluedelegate.h"
#include "doubletostringconvertor.h"
#include "additionalfloatingvalidator.h"
#include <QLineEdit>
#include <QEvent>

namespace gearbox {

inline
int precision(const QModelIndex &index)
{
  const auto pv = index.model()->data(index, Qt::UserRole + 1);
  if (!pv.canConvert<int>())
    return 9;
  return pv.toInt();
}

FloatingValueDelegate::FloatingValueDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QWidget *FloatingValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  Q_UNUSED(option);

  auto lineEdit = new QLineEdit(parent);

  const auto &current = index.model()->data(index);

  auto s = DoubleToStringConvertor::convert(current.toDouble(), 'g', precision(index));
  lineEdit->setText(DoubleToStringConvertor::convert(current.toDouble()));
  lineEdit->selectAll();

  connect(lineEdit, &QLineEdit::textChanged, this, &FloatingValueDelegate::onTextChanged);
  connect(this, &FloatingValueDelegate::editorCommit, lineEdit, &QLineEdit::clearFocus);

  return lineEdit;
}

QString FloatingValueDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
  /* We have to set the actual displayText from initStyleOption() */
  Q_UNUSED(value) Q_UNUSED(locale)

  return {};
}
void FloatingValueDelegate::forceCommit()
{
  emit editorCommit();
}

void FloatingValueDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
  QStyledItemDelegate::initStyleOption(option, index);

  auto value = index.data(Qt::DisplayRole);
  if (value.isValid()) {
    option->features |= QStyleOptionViewItem::HasDisplay;
    option->text = DoubleToStringConvertor::convert(value.toDouble(), 'g', precision(index));
  }
}

void FloatingValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  if (!index.isValid())
    return;

  const QVariant v = index.model()->data(index, Qt::EditRole);
  if (!v.isValid())
    return;

  bool ok;
  double value = v.toDouble(&ok);
  if (!ok)
    return;

  auto lineEdit = qobject_cast<QLineEdit *>(editor);
  if (lineEdit == nullptr)
    return;

  lineEdit->setText(DoubleToStringConvertor::convert(value, 'g', precision(index)));
}

void FloatingValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  double value;
  bool ok;
  auto lineEdit = qobject_cast<QLineEdit *>(editor);
  if (lineEdit == nullptr)
    return;

  QString s(lineEdit->text());
  s = s.replace(QChar::Nbsp, QString(""));
  value = DoubleToStringConvertor::back(s, &ok);
  if (!ok)
    return;
  if (!AdditionalFloatingValidator::additionalValidatorsOk(this, value))
    return;

  const int prec  = DoubleToStringConvertor::decimalDigits(s);
  /* Calling order matters. See FloatingMapperModel::setData() for details */
  model->setData(index, prec, Qt::UserRole + 1);
  model->setData(index, value, Qt::EditRole);
}

void FloatingValueDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  Q_UNUSED(index);

  editor->setGeometry(option.rect);
}

void FloatingValueDelegate::onTextChanged(const QString &)
{
  bool ok;
  auto lineEdit = qobject_cast<QLineEdit *>(QObject::sender());
  if (lineEdit == nullptr)
    return;

  QString s(lineEdit->text());
  s = s.replace(QChar::Nbsp, QString(""));
  const double dv = DoubleToStringConvertor::back(s, &ok);
  ok = ok || lineEdit->text().length() == 0;

  if (ok)
    ok = AdditionalFloatingValidator::additionalValidatorsOk(lineEdit, dv);
  if (ok)
    ok = AdditionalFloatingValidator::additionalValidatorsOk(this, dv);

  if (ok)
    lineEdit->setPalette(QPalette());
  else {
    QPalette palette = lineEdit->palette();

    palette.setColor(QPalette::Base, QColor(Qt::red));
    lineEdit->setPalette(palette);
  }
}

} // namespace gearbox

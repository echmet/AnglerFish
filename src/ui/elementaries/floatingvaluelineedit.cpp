#include "../../gearbox/doubletostringconvertor.h"
#include "../../gearbox/additionalfloatingvalidator.h"
#include "floatingvaluelineedit.h"
#include <QLocale>

FloatingValueLineEdit::FloatingValueLineEdit(QWidget *parent) :
  QLineEdit(parent)
{
  this->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

  connect(this, &FloatingValueLineEdit::textChanged, this, &FloatingValueLineEdit::ensureSanity);
  connect(this, &FloatingValueLineEdit::editingFinished, this, &FloatingValueLineEdit::onEditingFinished);
  gearbox::DoubleToStringConvertor::notifyOnFormatChanged(this);
}

void FloatingValueLineEdit::ensureSanity(QString text)
{
  const auto ok = isInputValid(text);

  if (ok)
      this->setPalette(QPalette());
  else {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Qt::red);
    this->setPalette(palette);
  }
}

bool FloatingValueLineEdit::isInputValid() const
{
  return isInputValid(this->text());
}

bool FloatingValueLineEdit::isInputValid(QString text) const
{
  bool ok;

  QString _text = text.replace(QChar::Nbsp, QString(""), Qt::CaseInsensitive);

  const double dv = gearbox::DoubleToStringConvertor::back(_text, &ok);
  if (ok)
    ok = gearbox::AdditionalFloatingValidator::additionalValidatorsOk(this, dv);

  return ok;
}

void FloatingValueLineEdit::onEditingFinished()
{
  bool ok;
  double dv;

  dv = gearbox::DoubleToStringConvertor::back(text(), &ok);
  if (ok) {
    setNumberText(dv);
    emit valueChanged(dv);
  }
}

void FloatingValueLineEdit::onNumberFormatChanged(const QLocale *oldLocale)
{
  bool ok;
  double dv;

  dv = oldLocale->toDouble(this->text(), &ok);
  if (ok)
    setNumberText(dv);
}

void FloatingValueLineEdit::revalidate()
{
  ensureSanity(text());
}

void FloatingValueLineEdit::setNumberText(const double dv)
{
  blockSignals(true);

  const int prec = gearbox::DoubleToStringConvertor::decimalDigits(text());
  const QString t = gearbox::DoubleToStringConvertor::convert(dv, 'f', prec);
  this->setText(t);

  blockSignals(false);
}

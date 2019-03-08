#ifndef FLOATINGVALUELINEEDIT_H
#define FLOATINGVALUELINEEDIT_H

#include <QLineEdit>
#include <QLocale>
#include "../../gearbox/inumberformatchangeable.h"

class FloatingValueLineEdit : public QLineEdit, public gearbox::INumberFormatChangeable
{
  Q_OBJECT
  Q_INTERFACES(gearbox::INumberFormatChangeable)
public:
  FloatingValueLineEdit(QWidget *parent = nullptr);
  bool isInputValid() const;
  double numericValue() const;
  void onNumberFormatChanged(const QLocale *oldLocale) override;

public slots:
  void revalidate();

private:
  bool isInputValid(QString text) const;
  void setNumberText(const double dv);

private slots:
  void ensureSanity(QString text);
  void onEditingFinished();

signals:
  void valueChanged(double);
};

#endif // FLOATINGVALUELINEEDIT_H

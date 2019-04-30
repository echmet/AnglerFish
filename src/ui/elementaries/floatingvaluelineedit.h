#ifndef FLOATINGVALUELINEEDIT_H
#define FLOATINGVALUELINEEDIT_H

#include <QLineEdit>
#include <QLocale>
#include <gearbox/inumberformatchangeable.h>

class FloatingValueLineEdit : public QLineEdit, public gearbox::INumberFormatChangeable
{
  Q_OBJECT
  Q_INTERFACES(gearbox::INumberFormatChangeable)
public:
  FloatingValueLineEdit(QWidget *parent = nullptr);
  bool isEmptyAllowed() const;
  bool isInputValid() const;
  double numericValue() const;
  void onNumberFormatChanged(const QLocale *oldLocale) override;
  void setAllowEmpty(const bool allow);

public slots:
  void revalidate();

private:
  bool isInputValid(QString text) const;
  void setNumberText(const double dv);

  bool m_allowEmpty;

private slots:
  void ensureSanity(const QString &text);
  void onEditingFinished();

signals:
  void valueChanged(double);
};

#endif // FLOATINGVALUELINEEDIT_H

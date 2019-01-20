#ifndef EDITCHARGESWIDGETSIMPLE_H
#define EDITCHARGESWIDGETSIMPLE_H

#include "editchargeswidget.h"

class FloatingValueDelegate;

class EditChargesWidgetSimple : public EditChargesWidget {
  Q_OBJECT
public:
  using EditChargesWidget::EditChargesWidget;

protected:
  virtual void setupChargesModel(QTableView *tbv) override;

private:
  FloatingValueDelegate *m_fltDelegate;
};

#endif // EDITCHARGESWIDGETSIMPLE_H

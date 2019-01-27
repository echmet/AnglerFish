#ifndef EDITCHARGESWIDGETSIMPLE_H
#define EDITCHARGESWIDGETSIMPLE_H

#include "editchargeswidget.h"

namespace gearbox {
  class FloatingValueDelegate;
} // namespace gearbox

class EditChargesWidgetSimple : public EditChargesWidget {
  Q_OBJECT
public:
  using EditChargesWidget::EditChargesWidget;

protected:
  virtual void setupChargesModel(QTableView *tbv) override;

private:
  gearbox::FloatingValueDelegate *m_fltDelegate;
};

#endif // EDITCHARGESWIDGETSIMPLE_H

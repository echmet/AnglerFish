#ifndef EDITCHARGESWIDGETESTIMATES_H
#define EDITCHARGESWIDGETESTIMATES_H

#include "editchargeswidget.h"

class FloatingValueDelegate;

class EditChargesWidgetEstimates : public EditChargesWidget {
  Q_OBJECT
public:
  using EditChargesWidget::EditChargesWidget;

  std::vector<bool> fixedMobilities() const;
  std::vector<bool> fixedpKas() const;

protected:
  virtual void setupChargesModel(QTableView *tbv) override;

private:
  FloatingValueDelegate *m_fltDelegate;
};

#endif // EDITCHARGESWIDGETESTIMATES_H

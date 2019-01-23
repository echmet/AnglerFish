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

  void setCharges(std::map<int, std::pair<double, bool>> pKas, std::map<int, std::pair<double, bool>> mobilities,
                  const int chargeLow, const int chargeHigh);

protected:
  virtual void setupChargesModel(QTableView *tbv) override;
};

#endif // EDITCHARGESWIDGETESTIMATES_H

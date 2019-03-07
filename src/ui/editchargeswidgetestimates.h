#ifndef EDITCHARGESWIDGETESTIMATES_H
#define EDITCHARGESWIDGETESTIMATES_H

#include "editchargeswidget.h"

namespace gearbox {
  class FloatingValueDelegate;
} // namespace gearbox

class EditChargesWidgetEstimates : public EditChargesWidget {
  Q_OBJECT
public:
  using EditChargesWidget::EditChargesWidget;

  std::vector<bool> fixedMobilities() const;
  std::vector<bool> fixedpKas() const;

  void setCharges(const std::map<int, std::pair<double, bool>> &pKas,
                  const std::map<int, std::pair<double, bool>> &mobilities,
                  const int chargeLow, const int chargeHigh);

protected:
  virtual void setupChargesModel(QTableView *tbv) override;

signals:
  void estimatesChanged();
};

#endif // EDITCHARGESWIDGETESTIMATES_H

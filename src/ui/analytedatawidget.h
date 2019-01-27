#ifndef ANALYTEDATAWIDGET_H
#define ANALYTEDATAWIDGET_H

#include <gearbox/floatingvaluedelegate.h>
#include <QWidget>

namespace gearbox {
  class Gearbox;
} // namespace gearbox

namespace Ui {
  class AnalyteDataWidget;
} // namespace Ui

class QDataWidgetMapper;
class EditChargesWidgetEstimates;

class AnalyteDataWidget : public QWidget {
  Q_OBJECT
public:
  explicit AnalyteDataWidget(gearbox::Gearbox &h_gbox, QWidget *parent = nullptr);
  ~AnalyteDataWidget();

  int chargeLow() const noexcept;
  int chargeHigh() const noexcept;
  std::vector<std::pair<double, bool>> estimatedMobilities() const;
  std::vector<std::pair<double, bool>> estimatedpKas() const;

  void setEstimatesFromCurrent();

private:
  void setWidgetSizes();

  Ui::AnalyteDataWidget *ui;

  EditChargesWidgetEstimates *m_estimatedParamsWidget;

  QDataWidgetMapper *m_scalarFitResultsMapper;

  gearbox::Gearbox &h_gbox;

private slots:
  void onResultsToClipboard();
  void onScreenChanged();

signals:
  void estimatesChanged();
};

#endif // ANALYTEDATAWIDGET_H

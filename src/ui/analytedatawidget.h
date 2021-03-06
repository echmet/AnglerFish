#ifndef ANALYTEDATAWIDGET_H
#define ANALYTEDATAWIDGET_H

#include <QWidget>

namespace gearbox {
  class Gearbox;
  class FloatingValueDelegate;
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
  Ui::AnalyteDataWidget *ui;

  EditChargesWidgetEstimates *m_estimatedParamsWidget;

  QDataWidgetMapper *m_scalarFitResultsMapper;
  gearbox::FloatingValueDelegate *m_fltDelegate;

  gearbox::Gearbox &h_gbox;

private slots:
  void onEstimatesChanged();
  void onResultsToClipboard();
  void onUpdateEstimates();

signals:
  void estimatesChanged();
};

#endif // ANALYTEDATAWIDGET_H

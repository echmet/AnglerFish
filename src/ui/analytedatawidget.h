#ifndef ANALYTEDATAWIDGET_H
#define ANALYTEDATAWIDGET_H

#include <gearbox/floatingvaluedelegate.h>
#include <QWidget>

namespace Ui {
class AnalyteDataWidget;
}

class QDataWidgetMapper;
class EditChargesWidgetEstimates;

class AnalyteDataWidget : public QWidget {
  Q_OBJECT
public:
  explicit AnalyteDataWidget(QWidget *parent = nullptr);
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

private slots:
  void onResultsToClipboard();
  void onScreenChanged();
};

#endif // ANALYTEDATAWIDGET_H

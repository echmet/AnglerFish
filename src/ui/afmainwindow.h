#ifndef AFMAINWINDOW_H
#define AFMAINWINDOW_H

#include "toggletracepointsdialog.h"

#include <QFileDialog>
#include <QMainWindow>

class AnalyteDataWidget;
class BuffersInputWidget;
class CheckForUpdateDialog;
class MobilityConstraintsDialog;
class SoftwareUpdater;
class SummarizeDialog;
class QPushButton;
class FitPlotWidget;
class Gearbox;
class QSplitter;

namespace gearbox {
  class Gearbox;
} // namespace gearbox

namespace Ui {
  class AFMainWindow;
} // namespace Ui

class AFMainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit AFMainWindow(gearbox::Gearbox &gbox,
                        QWidget *parent = nullptr);
  ~AFMainWindow() override;
  void connectUpdater(SoftwareUpdater *updater);

protected:
  virtual void closeEvent(QCloseEvent *evt) override;

private:
  void invalidateResults();
  void updatePlotExperimental();
  void updatePlotFitted();
  void setEstimates();
  void setupIcons();

  Ui::AFMainWindow *ui;
  gearbox::Gearbox &h_gbox;

  QSplitter *qsp_controlsChart;
  QWidget *m_buffersAnalyte;
  FitPlotWidget *m_fitPlotWidget;
  BuffersInputWidget *m_bufInpWidget;
  AnalyteDataWidget *m_analDataWidget;
  SummarizeDialog *m_summarizeDlg;
  MobilityConstraintsDialog *m_mobConstrsDlg;

  QPushButton *m_qpb_new;
  QPushButton *m_qpb_newBuffers;
  QPushButton *m_qpb_load;
  QPushButton *m_qpb_save;
  QPushButton *m_qpb_calculate;
  QPushButton *m_qpb_summarize;
  QPushButton *m_qpb_toggleAnalytePanel;

  CheckForUpdateDialog *m_checkForUpdateDlg;

  QFileDialog m_saveDlg;

  ToggleTracepointsDialog *m_tptsDlg;
  ToggleTracepointsDialog::TracingSetup m_tracingSetup;

private slots:
  void onBuffersChanged();
  void onAboutTriggered();
  void onCalculate();
  void onCheckForUpdate();
  void onCurveExperimentalChanged();
  void onCurveFittedChanged();
  void onLoad();
  void onNew();
  void onNewBuffers();
  void onOpenDatabase();
  void onSave();
  void onSetDebuggingOutput();
  void onSummarize();
  void onToggleAnalytePanel();
};

#endif // AFMAINWINDOW_H

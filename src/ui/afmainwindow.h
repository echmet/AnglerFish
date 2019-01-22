#ifndef AFMAINWINDOW_H
#define AFMAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>

class AnalyteDataWidget;
class BuffersInputWidget;
class QPushButton;
class FitPlotWidget;
class Gearbox;
class QSplitter;

namespace Ui {
class AFMainWindow;
}

class AFMainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit AFMainWindow(QWidget *parent = nullptr);
  ~AFMainWindow() override;

protected:
  virtual void closeEvent(QCloseEvent *evt) override;

private:
  void invalidateResults();
  void updatePlotExperimental();
  void updatePlotFitted();
  void setEstimates();
  void setupIcons();

  Ui::AFMainWindow *ui;

  QSplitter *qsp_controlsChart;
  QWidget *m_buffersAnalyte;
  FitPlotWidget *m_fitPlotWidget;
  BuffersInputWidget *m_bufInpWidget;
  AnalyteDataWidget *m_analDataWidget;

  QPushButton *m_qpb_new;
  QPushButton *m_qpb_load;
  QPushButton *m_qpb_save;
  QPushButton *m_qpb_calculate;

  QFileDialog m_loadDlg;
  QFileDialog m_saveDlg;

private slots:
  void onBuffersChanged();
  void onAboutTriggered();
  void onCalculate();
  void onLoad();
  void onNew();
  void onSave();
};

#endif // AFMAINWINDOW_H

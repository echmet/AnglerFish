#ifndef FITPLOTWIDGET_H
#define FITPLOTWIDGET_H

#include <QWidget>

class DoubleClickableQwtPlotZoomer;
class QwtPlot;
class QwtPlotBarChart;
class QwtPlotCurve;
class QwtPlotPicker;

namespace Ui {
class FitPlotWidget;
}

class FitPlotWidget : public QWidget
{
  Q_OBJECT

public:
  explicit FitPlotWidget(QWidget *parent = nullptr);
  ~FitPlotWidget();
  void setExperimentalData(const QVector<QPointF> &data);
  void setFittedData(const QVector<QPointF> &data);
  void setResidualsData(const QVector<QPointF> &data);

public slots:
  void onExperimentalChanged();
  void onFittedChanged();
  void onResidualsChanged();

private:
  void refreshPlot();
  void setupPlot();

  Ui::FitPlotWidget *ui;

  QwtPlot *m_plot;
  QwtPlotCurve *m_curveExperimental;
  QwtPlotCurve *m_curveFitted;
  QwtPlotBarChart *m_barResiduals;
  QwtPlotPicker *m_plotPicker;
  DoubleClickableQwtPlotZoomer *m_plotZoomer;

  static const QString s_uEffUnit;
  static const QString s_uEffCoeff;
};

#endif // FITPLOTWIDGET_H

#ifndef FITPLOTWIDGET_H
#define FITPLOTWIDGET_H

#include <QWidget>

class DoubleClickableQwtPlotZoomer;
class QwtPlot;
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
  void setExperimentalData(const QVector<QPointF> &experimental, const QVector<QPointF> &excluded);
  void setFittedData(const QVector<QPointF> &data);
  void setResidualsData(const QVector<QPointF> &data);

private:
  void refreshPlot();
  void setupPlot();

  Ui::FitPlotWidget *ui;

  QwtPlot *m_plot;
  QwtPlotCurve *m_curveExperimental;
  QwtPlotCurve *m_curveExcluded;
  QwtPlotCurve *m_curveFitted;
  QwtPlotCurve *m_curveResiduals;
  QwtPlotPicker *m_plotPicker;
  DoubleClickableQwtPlotZoomer *m_plotZoomer;

  static const QString s_uEffUnit;
  static const QString s_uEffCoeff;
};

#endif // FITPLOTWIDGET_H

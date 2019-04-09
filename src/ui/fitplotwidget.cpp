#include "fitplotwidget.h"
#include "ui_fitplotwidget.h"

#include "doubleclickableqwtplotzoomer.h"

#include <globals.h>
#include <QFontMetrics>
#include <QLayout>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_symbol.h>

const QString FitPlotWidget::s_uEffUnit{"(m<sup>2</sup>/V/s)"};
const QString FitPlotWidget::s_uEffCoeff{"10<sup>-9</sup>"};

template <typename T>
constexpr inline
int cxsgn(const T &v)
{
  return (v > T{0}) - (v < T{0});
}

inline
QwtSymbol * makeSymbol(const QwtSymbol::Style s, const QColor &clr, const QFontMetrics &fm)
{
  static const int SIZE{fm.height()};

  auto symbol = new QwtSymbol{s};
  symbol->setSize(SIZE);
  symbol->setColor(clr);

  return symbol;
}

FitPlotWidget::FitPlotWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::FitPlotWidget)
{
  ui->setupUi(this);

  Q_ASSERT_X(layout() != nullptr, "FitPlotWidget", "FitPlotWidget must have a layout");

  m_plot = new QwtPlot{this};
  layout()->addWidget(m_plot);

  m_curveExperimental = new QwtPlotCurve{};
  m_curveExcluded = new QwtPlotCurve{};
  m_curveFitted = new QwtPlotCurve{};
  m_curveProvisional = new QwtPlotCurve{};
  m_curveResiduals = new QwtPlotCurve{};
  m_plotZoomer = new DoubleClickableQwtPlotZoomer(m_plot->canvas());
  m_plotPicker = new QwtPlotPicker(QwtPlot::Axis::xBottom, QwtPlot::Axis::yLeft,
                                   QwtPicker::NoRubberBand, QwtPicker::AlwaysOff,
                                   m_plot->canvas());

  setupPlot();
}

FitPlotWidget::~FitPlotWidget()
{
  delete ui;
}

void FitPlotWidget::refreshPlot()
{
  auto brect = m_curveExperimental->boundingRect();

  if (m_curveExcluded->data()->size() > 0)
    brect = brect.united(m_curveExcluded->boundingRect());

  if (m_curveFitted->data()->size() > 0)
    brect = brect.united(m_curveFitted->boundingRect());

  if (m_curveProvisional->data()->size() > 0)
    brect = brect.united(m_curveProvisional->boundingRect());

  m_plot->replot();
  m_plotZoomer->zoom(brect);
  m_plotZoomer->setZoomBase(brect);
}

void FitPlotWidget::setExperimentalData(const QVector<QPointF> &experimental, const QVector<QPointF> &excluded)
{
  m_curveExperimental->setSamples(experimental);
  m_curveExcluded->setSamples(excluded);

  refreshPlot();
}

void FitPlotWidget::setProvisionalData(const QVector<QPointF> &data)
{
  m_curveProvisional->setSamples(data);

  refreshPlot();
}

void FitPlotWidget::setFittedData(const QVector<QPointF> &data)
{
  m_curveFitted->setSamples(data);

  refreshPlot();
}

void FitPlotWidget::setResidualsData(const QVector<QPointF> &data)
{
  m_curveResiduals->setSamples(data);

  auto brect = m_curveResiduals->boundingRect();
  qreal rMax = brect.top();
  qreal rMin = brect.bottom();

  if (cxsgn(rMin) != cxsgn(rMax)) {
    qreal ref = std::abs(rMax) > std::abs(rMin) ? std::abs(rMax) : std::abs(rMin);

    rMax = ref;
    rMin = -ref;
  }

  m_plot->setAxisScale(QwtPlot::yRight, rMin, rMax);

  refreshPlot();
}

void FitPlotWidget::setupPlot()
{
  m_plot->enableAxis(QwtPlot::yRight);

  m_plot->setAxisTitle(QwtPlot::xBottom, QStringLiteral("pH"));
  if (Globals::isWindowsXP())
    m_plot->setAxisTitle(QwtPlot::yLeft, QString{"<html>u<sub>eff</sub> %1 . %2</html>"}.arg(s_uEffUnit, s_uEffCoeff));
  else
    m_plot->setAxisTitle(QwtPlot::yLeft, QString{"<html>\xCE\xBC<sub>eff</sub> %1 \xC2\xB7 %2</html>"}.arg(s_uEffUnit, s_uEffCoeff));

  m_plot->setAxisTitle(QwtPlot::yRight, QString{"Residuals"});

  m_plot->setCanvasBackground(Qt::white);

  m_curveResiduals->attach(m_plot);
  m_curveResiduals->setYAxis(QwtPlot::yRight);

  m_curveExperimental->attach(m_plot);
  m_curveExcluded->attach(m_plot);
  m_curveFitted->attach(m_plot);
  m_curveProvisional->attach(m_plot);


  m_plotZoomer->setTrackerMode(QwtPicker::AlwaysOn);
  m_plotZoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::LeftButton, Qt::ShiftModifier);
  m_plotPicker->setStateMachine(new QwtPickerClickPointMachine{}); /* Come blow my ClickPoint machine! */
  m_plotPicker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);

  m_curveExperimental->setStyle(QwtPlotCurve::NoCurve);
  m_curveExperimental->setSymbol(makeSymbol(QwtSymbol::Cross, Qt::black, fontMetrics()));

  m_curveExcluded->setStyle(QwtPlotCurve::NoCurve);
  m_curveExcluded->setSymbol(makeSymbol(QwtSymbol::Cross, Qt::red, fontMetrics()));

  m_curveFitted->setStyle(QwtPlotCurve::NoCurve);
  m_curveFitted->setSymbol(makeSymbol(QwtSymbol::XCross, Qt::blue, fontMetrics()));

  m_curveProvisional->setStyle(QwtPlotCurve::NoCurve);
  m_curveProvisional->setSymbol(makeSymbol(QwtSymbol::XCross, QColor{255, 5, 147}, fontMetrics()));

  m_curveResiduals->setStyle(QwtPlotCurve::NoCurve);
  m_curveResiduals->setSymbol(makeSymbol(QwtSymbol::Triangle, Qt::yellow, fontMetrics()));
}

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
#include <qwt_legend.h>

const QString FitPlotWidget::s_uEffUnit{"(m<sup>2</sup>/V/s)"};
const QString FitPlotWidget::s_uEffCoeff{"10<sup>-9</sup>"};

template <typename T>
constexpr inline
int cxsgn(const T &v)
{
  return (v > T{0}) - (v < T{0});
}

inline
QwtSymbol * makeSymbol(const QwtSymbol::Style s, const QColor &clr, const QFont &font)
{
  const int SIZE{QFontMetrics{font}.height()};

  auto symbol = new QwtSymbol{s};
  symbol->setSize(SIZE);
  symbol->setColor(clr);

  return symbol;
}

inline
void setLegendAttributes(QwtPlotCurve *curve)
{
  curve->setLegendAttribute(QwtPlotCurve::LegendShowLine, false);
  curve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);
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
  m_curveResidualsZeroLine = new QwtPlotCurve{};
  m_plotZoomer = new DoubleClickableQwtPlotZoomer(m_plot->canvas());
  m_plotPicker = new QwtPlotPicker(QwtPlot::Axis::xBottom, QwtPlot::Axis::yLeft,
                                   QwtPicker::NoRubberBand, QwtPicker::AlwaysOff,
                                   m_plot->canvas());

  setupPlot();

  makeLegend();
}

FitPlotWidget::~FitPlotWidget()
{
  delete ui;
}

bool FitPlotWidget::isLegendShown() const
{
  return m_plot->legend() != nullptr;
}

void FitPlotWidget::makeLegend()
{
  auto legend = new QwtLegend{};

  legend->setMaxColumns(1);
  legend->setDefaultItemMode(QwtLegendData::ReadOnly);

  m_plot->insertLegend(legend);
}

void FitPlotWidget::refreshPlot()
{
  static const double minSize{1e-13};
  const auto setMinSize = [](QRectF r) {
    if (r.width() <= 0.0)
      r.adjust(-minSize, 0.0, minSize, 0.0);

    if (r.height() <= 0.0)
      r.adjust(0.0, -minSize, 0.0, minSize);

    return r;
  };

  auto brect = m_curveExperimental->boundingRect();

  if (m_curveExcluded->data()->size() > 0) {
    auto nbr = setMinSize(m_curveExcluded->boundingRect());
    brect = brect.united(nbr);
  }

  if (m_curveFitted->data()->size() > 0) {
    auto nbr = setMinSize(m_curveFitted->boundingRect());
    brect = brect.united(nbr);
  }

  if (m_curveProvisional->data()->size() > 0) {
    auto nbr = setMinSize(m_curveProvisional->boundingRect());
    brect = brect.united(nbr);
  }

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

  if (data.empty())
    m_curveResidualsZeroLine->setSamples(QVector<QPointF>{});
  else {
    qreal l = brect.left();
    qreal r = brect.right();
    l -= l * 0.05;
    r += r * 0.05;

    QVector<QPointF> zeroCurve = { { l, 0.0 }, { r, 0.0 } };
    m_curveResidualsZeroLine->setSamples(zeroCurve);
  }

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

  auto symfont = font();
  auto symFontSmall = symfont;
  symFontSmall.setPointSize(qRound(symfont.pointSize() * 2.0 / 3.0));

  m_curveResiduals->attach(m_plot);
  m_curveResiduals->setYAxis(QwtPlot::yRight);
  m_curveResidualsZeroLine->attach(m_plot);
  m_curveResidualsZeroLine->setYAxis(QwtPlot::yRight);

  m_curveFitted->attach(m_plot);
  m_curveExperimental->attach(m_plot);
  m_curveExcluded->attach(m_plot);
  m_curveProvisional->attach(m_plot);

  m_plotZoomer->setTrackerMode(QwtPicker::AlwaysOn);
  m_plotZoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::LeftButton, Qt::ShiftModifier);
  m_plotPicker->setStateMachine(new QwtPickerClickPointMachine{}); /* Come blow my ClickPoint machine! */
  m_plotPicker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);

  m_curveExperimental->setStyle(QwtPlotCurve::NoCurve);
  m_curveExperimental->setSymbol(makeSymbol(QwtSymbol::Cross, Qt::black, symfont));

  m_curveExcluded->setStyle(QwtPlotCurve::NoCurve);
  m_curveExcluded->setSymbol(makeSymbol(QwtSymbol::Cross, Qt::red, symfont));

  m_curveFitted->setStyle(QwtPlotCurve::NoCurve);
  m_curveFitted->setSymbol(makeSymbol(QwtSymbol::Rect, Qt::blue, symFontSmall));

  m_curveProvisional->setStyle(QwtPlotCurve::NoCurve);
  m_curveProvisional->setSymbol(makeSymbol(QwtSymbol::Diamond, QColor{255, 5, 147}, symFontSmall));

  m_curveResiduals->setStyle(QwtPlotCurve::NoCurve);
  m_curveResiduals->setSymbol(makeSymbol(QwtSymbol::Hexagon, QColor{255, 255, 127}, symfont));

  m_curveResidualsZeroLine->setPen(QColor{16, 16, 16}, 1.0, Qt::DotLine);

  m_curveFitted->setTitle(QStringLiteral("Fitted"));
  m_curveExperimental->setTitle(QStringLiteral("Experimental"));
  m_curveExcluded->setTitle(QStringLiteral("Excluded"));
  m_curveResiduals->setTitle(QStringLiteral("Residuals"));
  m_curveProvisional->setTitle(QStringLiteral("Estimated"));

  setLegendAttributes(m_curveFitted);
  setLegendAttributes(m_curveExperimental);
  setLegendAttributes(m_curveResiduals);
  setLegendAttributes(m_curveProvisional);
  setLegendAttributes(m_curveExcluded);
  m_curveResidualsZeroLine->setItemAttribute(QwtPlotItem::Legend, false);
}

void FitPlotWidget::showLegend(const bool show)
{
  if (show)
    makeLegend();
  else
    m_plot->insertLegend(nullptr);
}

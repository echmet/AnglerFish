#include "analytedatawidget.h"
#include "ui_analytedatawidget.h"

#include "editchargeswidgetestimates.h"

#include <gearbox/doubletostringconvertor.h>
#include <gearbox/gearbox.h>
#include <gearbox/curvetoclipboardexporter.h>
#include <QClipboard>
#include <QMessageBox>
#include <QScreen>
#include <QTextStream>
#include <QTimer>
#include <QWindow>
#include <cassert>

inline
int estimatedParamsWidth(const QFontMetrics &fm)
{
  static const QString cue{"Mobility pKa Fix mobility Fix pKa"};

#ifdef Q_OS_WIN
  return qRound(fm.width(cue) * 2.9);
#else
  return qRound(fm.width(cue) * 2.2);
#endif // Q_OS_WIN
}

inline
int fitResultsWidth(const QFontMetrics &fm)
{
  static const QString cue{"Mobility Abs. error Rel. error"};

#ifdef Q_OS_WIN
  return qRound(fm.width(cue) * 2.25);
#else
  return qRound(fm.width(cue) * 2.3);
#endif // Q_OS_WIN
}

inline
int fitResultsHeight(const QFontMetrics &fm)
{
  return qRound(fm.height() * 5.0);
}


AnalyteDataWidget::AnalyteDataWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AnalyteDataWidget)
{
  ui->setupUi(this);

  m_estimatedParamsWidget = new EditChargesWidgetEstimates{this};
  m_estimatedParamsWidget->initialize();
  m_estimatedParamsWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

  ui->qvlay_estimates->addWidget(m_estimatedParamsWidget);

  ui->qtbv_fittedMobilities->setModel(&Gearbox::instance()->mobilitiesResultsModel());
  ui->qtbv_fittedpKas->setModel(&Gearbox::instance()->pKaResultsModel());

  connect(ui->qpb_resultsToClipboard, &QPushButton::clicked, this, &AnalyteDataWidget::onResultsToClipboard);
  connect(ui->qpb_curveToClipboard, &QPushButton::clicked,
          []() { try {
                   CurveToClipboardExporter::write();
                 } catch (const CurveToClipboardExporter::Exception &ex) {
                   QMessageBox mbox{QMessageBox::Critical, tr("Internal error"),
                                    QString{tr("Failed to copy curve to clipboard: %1")}.arg(ex.what())};
                   mbox.exec();
                 }
          });

  setWidgetSizes();

  QTimer::singleShot(0, this, [this]() { connect(this->window()->windowHandle(), &QWindow::screenChanged, this, &AnalyteDataWidget::onScreenChanged); }); /* This must be done from the event queue after the window is created */
}

AnalyteDataWidget::~AnalyteDataWidget()
{
  delete ui;
}

int AnalyteDataWidget::chargeLow() const noexcept
{
  return m_estimatedParamsWidget->chargeLow();
}

int AnalyteDataWidget::chargeHigh() const noexcept
{
  return m_estimatedParamsWidget->chargeHigh();
}

std::vector<std::pair<double, bool>> AnalyteDataWidget::estimatedMobilities() const
{
  std::vector<std::pair<double, bool>> v{};

  auto mobs = m_estimatedParamsWidget->mobilities();
  auto fixedMobs = m_estimatedParamsWidget->fixedMobilities();

  assert(mobs.size() == fixedMobs.size());

  for (size_t idx = 0; idx < mobs.size(); idx++)
    v.emplace_back(mobs[idx], fixedMobs[idx]);

  return v;
}

std::vector<std::pair<double, bool>> AnalyteDataWidget::estimatedpKas() const
{
  std::vector<std::pair<double, bool>> v{};

  auto pKas = m_estimatedParamsWidget->pKas();
  auto fixedPkas = m_estimatedParamsWidget->fixedpKas();

  assert(pKas.size() == fixedPkas.size());

  for (size_t idx = 0; idx < pKas.size(); idx++)
    v.emplace_back(pKas[idx], fixedPkas[idx]);

  return v;
}

void AnalyteDataWidget::onResultsToClipboard()
{
  static const QChar SEP{';'};
  const QString ABS_ERR{tr("Abs. error")};
  const QString REL_ERR{tr("Rel. error")};

  const auto gbox = Gearbox::instance();

  QString buf{};
  QTextStream str{&buf};
  str.setCodec("UTF-8");

  auto hdr = [&str](const QStringList &list) {
    for (const auto &i : list)
      str << i << SEP;
    str << "\n";
  };

  auto write = [&str](const FitResultsModel &model) {
    const auto &loc = DoubleToStringConvertor::locale();

    for (int row = 0; row < model.rowCount(); row++) {
      str << model.data(model.index(row, 0), Qt::UserRole + 1).toInt(nullptr) << SEP;
      str << loc.toString(model.data(model.index(row, FitResultsModel::ID_VALUE), Qt::DisplayRole).toDouble(nullptr)) << SEP;
      str << loc.toString(model.data(model.index(row, FitResultsModel::ID_ABS_ERR), Qt::DisplayRole).toDouble(nullptr)) << SEP;
      str << loc.toString(model.data(model.index(row, FitResultsModel::ID_REL_ERR), Qt::DisplayRole).toDouble(nullptr)) << SEP;
      str << "\n";
    }
  };

  str << tr("--- Mobilities ---") << "\n";
  hdr({tr("Mobility"), ABS_ERR, REL_ERR});
  write(gbox->mobilitiesResultsModel());

  str << "\n" << tr("--- pKas ---") << "\n";
  hdr({tr("pKa"), ABS_ERR, REL_ERR});
  write(gbox->pKaResultsModel());

  auto clip = QApplication::clipboard();
  if (clip != nullptr)
    clip->setText(buf);
}

void AnalyteDataWidget::onScreenChanged()
{
  setWidgetSizes();
}

void AnalyteDataWidget::setEstimatesFromCurrent()
{
  const auto &analyte = Gearbox::instance()->analyteInputParameters();

  std::map<int, std::pair<double, bool>> pKas{};

  /* These conversions are so damn annoying...! */
  const int bChg = m_estimatedParamsWidget->baseCharge(analyte.chargeLow, analyte.chargeHigh);
  auto it = analyte.pKas.cbegin();
  for (int charge = analyte.chargeLow; charge <= analyte.chargeHigh; charge++) {
    if (charge != bChg) {
      pKas[charge] = {it->value, it->fixed};
      it++;
    } else
      pKas[charge] = {0.0, false};
  }

  std::map<int, std::pair<double, bool>> mobilities{};
  int charge = analyte.chargeLow;
  for (const auto &item : analyte.mobilities) {
    mobilities[charge] = {item.value, item.fixed};
    charge++;
  }

  m_estimatedParamsWidget->setCharges(std::move(pKas), std::move(mobilities), analyte.chargeLow, analyte.chargeHigh);
}

void AnalyteDataWidget::setWidgetSizes()
{
  {
    const int w = estimatedParamsWidth(fontMetrics());
    m_estimatedParamsWidget->setMinimumWidth(w);
    m_estimatedParamsWidget->setMaximumWidth(w);
  }

  {
    const int w = fitResultsWidth(fontMetrics());
    ui->qtbv_fittedMobilities->setMinimumWidth(w);
    ui->qtbv_fittedMobilities->setMaximumWidth(w);
    ui->qtbv_fittedpKas->setMinimumWidth(w);
    ui->qtbv_fittedpKas->setMaximumWidth(w);
  }

  ui->ql_resultsCaption->setMinimumWidth(ui->qtbv_fittedpKas->minimumWidth());
  ui->ql_resultsCaption->setMaximumWidth(ui->qtbv_fittedpKas->maximumWidth());

  ui->qtbv_fittedMobilities->setMinimumHeight(fitResultsHeight(fontMetrics()));
  ui->qtbv_fittedpKas->setMinimumHeight(fitResultsHeight(fontMetrics()));

  {
    const int tw = m_estimatedParamsWidget->minimumWidth() + ui->qtbv_fittedpKas->minimumWidth() +2 * fontMetrics().width('x');

    setMinimumWidth(tw);
    setMaximumWidth(tw);
  }
}

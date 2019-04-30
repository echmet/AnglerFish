#include "analytedatawidget.h"
#include "ui_analytedatawidget.h"

#include "editchargeswidgetestimates.h"

#include <gearbox/gearbox.h>
#include <gearbox/fitresultsmodel.h>
#include <gearbox/floatingvaluedelegate.h>
#include <gearbox/doubletostringconvertor.h>
#include <gearbox/scalarfitresultsmapping.h>
#include <gearbox/curveutility.h>
#include <QClipboard>
#include <QDataWidgetMapper>
#include <QMessageBox>
#include <QScreen>
#include <QTextStream>
#include <QTimer>
#include <QWindow>
#include <cassert>

AnalyteDataWidget::AnalyteDataWidget(gearbox::Gearbox &gbox,
                                     QWidget *parent) :
  QWidget{parent},
  ui{new Ui::AnalyteDataWidget},
  h_gbox{gbox}
{
  ui->setupUi(this);

  m_estimatedParamsWidget = new EditChargesWidgetEstimates{this};
  m_estimatedParamsWidget->initialize();
  m_estimatedParamsWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  m_estimatedParamsWidget->layout()->setMargin(0);

  ui->qvlay_estimates->addWidget(m_estimatedParamsWidget);

  ui->qtbv_fittedMobilities->setModel(&h_gbox.fittedMobilitiesModel());
  ui->qtbv_fittedpKas->setModel(&h_gbox.fittedpKasModel());

  {
    auto &model = h_gbox.scalarResultsModel();

    m_fltDelegate = new gearbox::FloatingValueDelegate{this};
    m_scalarFitResultsMapper = new QDataWidgetMapper{this};
    m_scalarFitResultsMapper->setModel(&model);
    m_scalarFitResultsMapper->setItemDelegate(m_fltDelegate);
    m_scalarFitResultsMapper->addMapping(ui->qle_rSquared, model.indexFromItem(gearbox::ScalarFitResultsMapping::Items::R_SQUARED));
    m_scalarFitResultsMapper->toFirst();
  }

  connect(ui->qpb_resultsToClipboard, &QPushButton::clicked, this, &AnalyteDataWidget::onResultsToClipboard);
  connect(ui->qpb_curveToClipboard, &QPushButton::clicked, this,
          [this]() {
            auto blocks = gearbox::CurveUtility::blockify(h_gbox.mobilityCurveModel());
            auto str = gearbox::CurveUtility::blocksToCSV(blocks, ';');
            QApplication::clipboard()->setText(str.c_str());
          });

  connect(m_estimatedParamsWidget, &EditChargesWidgetEstimates::estimatesChanged, this, &AnalyteDataWidget::onUpdateEstimates);

  onUpdateEstimates(); /* Set the initial empty estimates */
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

  QString buf{};
  QTextStream str{&buf};
  str.setCodec("UTF-8");

  auto hdr = [&str](const QStringList &list) {
    str << tr("Charge") << SEP;
    for (const auto &i : list)
      str << i << SEP;
    str << "\n";
  };

  auto write = [&str](const gearbox::FitResultsModel &model) {
    const auto &loc = gearbox::DoubleToStringConvertor::locale();

    for (int row = 0; row < model.rowCount(); row++) {
      str << model.data(model.index(row, 0), Qt::UserRole + 1).toInt(nullptr) << SEP;
      str << loc.toString(model.data(model.index(row, gearbox::FitResultsModel::ID_VALUE), Qt::DisplayRole).toDouble(nullptr)) << SEP;
      str << loc.toString(model.data(model.index(row, gearbox::FitResultsModel::ID_ABS_ERR), Qt::DisplayRole).toDouble(nullptr)) << SEP;
      str << loc.toString(model.data(model.index(row, gearbox::FitResultsModel::ID_REL_ERR), Qt::DisplayRole).toDouble(nullptr)) << SEP;
      str << "\n";
    }
  };

  str << tr("--- Mobilities ---") << "\n";
  hdr({tr("Mobility"), ABS_ERR, REL_ERR});
  write(h_gbox.fittedMobilitiesModel());

  str << "\n" << tr("--- pKas ---") << "\n";
  hdr({tr("pKa"), ABS_ERR, REL_ERR});
  write(h_gbox.fittedpKasModel());

  auto clip = QApplication::clipboard();
  if (clip != nullptr)
    clip->setText(buf);
}

void AnalyteDataWidget::onUpdateEstimates()
{
  const auto &mobs = estimatedMobilities();
  const auto &pKas = estimatedpKas();

  gearbox::AnalyteEstimates::ParameterVec aMobs{};
  gearbox::AnalyteEstimates::ParameterVec apKas{};

  for (const auto &p : mobs)
    aMobs.emplace_back(p.first, p.second);
  for (const auto &p :pKas)
    apKas.emplace_back(p.first, p.second);

  h_gbox.setAnalyteEstimates(chargeLow(), chargeHigh(),
                             std::move(aMobs), std::move(apKas));

  emit estimatesChanged();
}

void AnalyteDataWidget::setEstimatesFromCurrent()
{
  const auto &analyte = h_gbox.analyteEstimates();

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
  if (mobilities.empty())
    mobilities[0] = {0.0, false};

  m_estimatedParamsWidget->setCharges(std::move(pKas), std::move(mobilities), analyte.chargeLow, analyte.chargeHigh);

  emit estimatesChanged();
}

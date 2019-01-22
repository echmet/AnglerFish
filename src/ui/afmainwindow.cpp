#include "afmainwindow.h"
#include "ui_afmainwindow.h"
#include "fitplotwidget.h"
#include "buffersinputwidget.h"
#include "analytedatawidget.h"
#include "aboutdialog.h"

#include <globals.h>
#include <gearbox/gearbox.h>
#include <calculators/empfitterinterface.h>
#include <persistence/persistence.h>
#include <QCloseEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

AFMainWindow::AFMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::AFMainWindow),
  m_loadDlg{this, tr("Save setup"), {}, QString{tr("%1 JSON file (*.json)")}.arg(Globals::SOFTWARE_NAME)},
  m_saveDlg{this, tr("Save setup"), {}, QString{tr("%1 JSON file (*.json)")}.arg(Globals::SOFTWARE_NAME)}
{
  ui->setupUi(this);

  ui->centralwidget->setLayout(new QVBoxLayout{});

  qsp_controlsChart = new QSplitter{Qt::Vertical};
  m_buffersAnalyte = new QWidget{};
  m_buffersAnalyte->setLayout(new QHBoxLayout{});
  m_fitPlotWidget = new FitPlotWidget{};
  m_fitPlotWidget->setMinimumHeight(10);

  m_bufInpWidget = new BuffersInputWidget{};
  m_analDataWidget = new AnalyteDataWidget{};

  ui->centralwidget->layout()->addWidget(qsp_controlsChart);
  qsp_controlsChart->addWidget(m_buffersAnalyte);
  qsp_controlsChart->addWidget(m_fitPlotWidget);

  m_buffersAnalyte->layout()->addWidget(m_bufInpWidget);
  m_buffersAnalyte->layout()->addWidget(m_analDataWidget);

  m_qpb_new = new QPushButton{tr("New"), this};
  m_qpb_load = new QPushButton{tr("Load"), this};
  m_qpb_save = new QPushButton{tr("Save"), this};
  m_qpb_calculate = new QPushButton{tr("Calculate!"), this};

  ui->qtb_mainToolBar->addWidget(m_qpb_new);
  ui->qtb_mainToolBar->addWidget(m_qpb_load);
  ui->qtb_mainToolBar->addWidget(m_qpb_save);
  ui->qtb_mainToolBar->addWidget(m_qpb_calculate);

  m_saveDlg.setAcceptMode(QFileDialog::AcceptSave);

  setupIcons();

  setWindowTitle(Globals::VERSION_STRING());

  connect(m_qpb_load, &QPushButton::clicked, this, &AFMainWindow::onLoad);
  connect(m_qpb_new, &QPushButton::clicked, this, &AFMainWindow::onNew);
  connect(m_qpb_save, &QPushButton::clicked, this, &AFMainWindow::onSave);
  connect(m_qpb_calculate, &QPushButton::clicked, this, &AFMainWindow::onCalculate);

  connect(ui->actionNew, &QAction::triggered, this, &AFMainWindow::onNew);
  connect(ui->actionLoad, &QAction::triggered, this, &AFMainWindow::onLoad);
  connect(ui->actionSave, &QAction::triggered, this, &AFMainWindow::onSave);
  connect(ui->actionExit, &QAction::triggered, this, &AFMainWindow::close);
  connect(m_bufInpWidget, &BuffersInputWidget::buffersChanged, this, &AFMainWindow::onBuffersChanged);
  connect(ui->actionAbout, &QAction::triggered, this, &AFMainWindow::onAboutTriggered);

  /* Connect to gearbox */
  auto gbox = Gearbox::instance();
  connect(m_bufInpWidget, static_cast<void (BuffersInputWidget:: *)()>(&BuffersInputWidget::addBuffer),
          [&]() { Gearbox::instance()->chemicalBuffersModel().add({}); });
  connect(m_bufInpWidget, static_cast<void (BuffersInputWidget:: *)(const ChemicalBuffer &)>(&BuffersInputWidget::addBuffer),
          [&](const ChemicalBuffer &buf) { Gearbox::instance()->chemicalBuffersModel().add(buf); });
  connect(m_bufInpWidget, &BuffersInputWidget::removeBuffer,
          [](const ChemicalBuffer &buf) { Gearbox::instance()->chemicalBuffersModel().remove(buf); });
  connect(&gbox->chemicalBuffersModel(), &ChemicalBuffersModel::bufferAdded, m_bufInpWidget, &BuffersInputWidget::onBufferAdded);
  connect(&gbox->chemicalBuffersModel(), &ChemicalBuffersModel::beginModelReset, m_bufInpWidget, &BuffersInputWidget::onBeginBuffersReset);
  connect(&gbox->chemicalBuffersModel(), &ChemicalBuffersModel::endModelReset, m_bufInpWidget, &BuffersInputWidget::onEndBuffersReset);
  connect(&gbox->mobilityCurveModel(), &MobilityCurveModel::experimentalChanged, this, &AFMainWindow::onCurveExperimentalChanged);
  connect(&gbox->mobilityCurveModel(), &MobilityCurveModel::fittedChanged, this, &AFMainWindow::onCurveFittedChanged);
  connect(&gbox->mobilityCurveModel(), &MobilityCurveModel::residualsChanged, this, &AFMainWindow::onCurveResidualsChanged);
}

AFMainWindow::~AFMainWindow()
{
  delete ui;
}

void AFMainWindow::closeEvent(QCloseEvent *evt)
{
  QMessageBox mbox{QMessageBox::Question, tr("Confirm action"),
                   QString{tr("Do you really want to exit %1?\nAll unsaved data will be lost")}.arg(Globals::SOFTWARE_NAME),
                   QMessageBox::Yes | QMessageBox::No};
  if (mbox.exec() != QMessageBox::Yes)
    evt->ignore();
}

void AFMainWindow::invalidateResults()
{
  auto gbox = Gearbox::instance();

  gbox->mobilitiesResultsModel().setNewData({});
  gbox->pKaResultsModel().setNewData({});

  m_fitPlotWidget->setFittedData({});
}

void AFMainWindow::onAboutTriggered()
{
  AboutDialog dlg{};

  dlg.exec();
}

void AFMainWindow::onBuffersChanged()
{
  invalidateResults();
  updatePlotExperimental();
}

void AFMainWindow::onCalculate()
{
  setEstimates();

  EMPFitterInterface iface{};

  try {
    iface.fit();
  } catch (const EMPFitterInterface::Exception &ex) {
    invalidateResults();
    QMessageBox mbox{QMessageBox::Warning, tr("Calculation failed"), ex.what()};
    mbox.exec();
    return;
  }
}

void AFMainWindow::onLoad()
{
  if (m_loadDlg.exec() == QDialog::Accepted) {
    if (!m_loadDlg.selectedFiles().empty()) {
      try {
        persistence::loadEntireSetup(m_loadDlg.selectedFiles().first());
        m_analDataWidget->setEstimatesFromCurrent();
      } catch (const persistence::Exception &ex) {
        QMessageBox mbox{QMessageBox::Warning, tr("Failed to load setup"), ex.what()};
        mbox.exec();
      }
    }
  }
}

void AFMainWindow::onCurveExperimentalChanged()
{
  m_fitPlotWidget->setExperimentalData(Gearbox::instance()->mobilityCurveModel().experimental());
}

void AFMainWindow::onCurveFittedChanged()
{
  m_fitPlotWidget->setFittedData(Gearbox::instance()->mobilityCurveModel().fitted());
}

void AFMainWindow::onCurveResidualsChanged()
{
  m_fitPlotWidget->setResidualsData(Gearbox::instance()->mobilityCurveModel().residuals());
}

void AFMainWindow::onNew()
{
  QMessageBox mbox{QMessageBox::Question,
                   tr("Confirm action"),
                   tr("Create new setup\n\n"
                      "Are you sure you want to discard the current "
                      "setup and create a new one?"),
                   QMessageBox::Yes | QMessageBox::No};
  if (mbox.exec() != QMessageBox::Yes)
    return;

  auto gbox = Gearbox::instance();
  gbox->chemicalBuffersModel().clear();
  gbox->clearAnalyteInputParameters();
  m_analDataWidget->setEstimatesFromCurrent();
}

void AFMainWindow::onSave()
{
  if (m_saveDlg.exec() == QDialog::Accepted) {
    if (!m_saveDlg.selectedFiles().empty()) {
      try {
        const auto gbox = Gearbox::instance();

        setEstimates();

        persistence::saveEntireSetup(m_saveDlg.selectedFiles().first(),
                                     gbox->chemicalBuffersModel(),
                                     gbox->analyteInputParameters());
      } catch (const persistence::Exception &ex) {
        QMessageBox mbox{QMessageBox::Warning, tr("Failed to save setup"), ex.what()};
        mbox.exec();
      }
    }
  }
}

void AFMainWindow::setEstimates()
{
  auto mobs = m_analDataWidget->estimatedMobilities();
  auto pKas = m_analDataWidget->estimatedpKas();

  AnalyteInputParameters::ParameterVec aMobs{};
  AnalyteInputParameters::ParameterVec apKas{};

  for (const auto &p : mobs)
    aMobs.emplace_back(p.first, p.second);
  for (const auto &p :pKas)
    apKas.emplace_back(p.first, p.second);

  Gearbox::instance()->setAnalyteInputParameters(m_analDataWidget->chargeLow(), m_analDataWidget->chargeHigh(),
                                                 std::move(aMobs), std::move(apKas));
}

void AFMainWindow::setupIcons()
{
#ifdef Q_OS_LINUX
  /* Menu bar */
  ui->actionNew->setIcon(QIcon::fromTheme("document-new"));
  ui->actionLoad->setIcon(QIcon::fromTheme("document-open"));
  ui->actionSave->setIcon(QIcon::fromTheme("document-save"));
  ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
  ui->actionAbout->setIcon(QIcon::fromTheme("help-about"));

  /* Button bar */
  m_qpb_new->setIcon(QIcon::fromTheme("document-new"));
  m_qpb_load->setIcon(QIcon::fromTheme("document-open"));
  m_qpb_save->setIcon(QIcon::fromTheme("document-save"));
  m_qpb_calculate->setIcon(QIcon::fromTheme("media-playback-start"));
#else
  /* Menu bar */
  ui->actionExit->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
  ui->actionAbout->setIcon(style()->standardIcon(QStyle::SP_DialogHelpButton));

  /* Button bar */
  m_qpb_calculate->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
#endif // Q_OS_
}

void AFMainWindow::updatePlotExperimental()
{
  auto gbox = Gearbox::instance();
  auto &bufsModel = gbox->chemicalBuffersModel();

  QVector<QPointF> data{};
  for (const auto &buf : bufsModel) {
    if (buf.empty())
      continue;

    for (const auto &uExp : buf.experimentalMobilities())
      data.push_back({buf.pH(), uExp});
  }

  gbox->mobilityCurveModel().setExperimental(std::move(data));
}

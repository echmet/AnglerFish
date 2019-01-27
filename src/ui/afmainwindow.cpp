#include "afmainwindow.h"
#include "ui_afmainwindow.h"
#include "fitplotwidget.h"
#include "buffersinputwidget.h"
#include "analytedatawidget.h"
#include "aboutdialog.h"
#include "ioniceffectscorrections.h"
#include "operationinprogressdialog.h"
#include "checkforupdatedialog.h"
#include "toggletracepointsdialog.h"

#include <globals.h>
#include <softwareupdater.h>
#include <gearbox/gearbox.h>
#include <gearbox/chemicalbuffersmodel.h>
#include <gearbox/mobilitycurvemodel.h>
#include <gearbox/calcworker.h>
#include <persistence/persistence.h>
#include <QCloseEvent>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QThread>
#include <QVBoxLayout>

AFMainWindow::AFMainWindow(gearbox::Gearbox &gbox,
                           QWidget *parent) :
  QMainWindow{parent},
  ui{new Ui::AFMainWindow},
  h_gbox{gbox},
  m_saveDlg{this, tr("Save setup"), {}, QString{tr("%1 JSON file (*.json)")}.arg(Globals::SOFTWARE_NAME)}
{
  ui->setupUi(this);

  ui->centralwidget->setLayout(new QVBoxLayout{});

  qsp_controlsChart = new QSplitter{Qt::Vertical};
  m_buffersAnalyte = new QWidget{};
  m_buffersAnalyte->setLayout(new QHBoxLayout{});
  m_fitPlotWidget = new FitPlotWidget{};
  m_fitPlotWidget->setMinimumHeight(150);

  m_bufInpWidget = new BuffersInputWidget{h_gbox};
  m_analDataWidget = new AnalyteDataWidget{h_gbox};

  m_checkForUpdateDlg = new CheckForUpdateDialog{this};

  ui->centralwidget->layout()->addWidget(qsp_controlsChart);
  qsp_controlsChart->addWidget(m_buffersAnalyte);
  qsp_controlsChart->addWidget(m_fitPlotWidget);

  m_buffersAnalyte->layout()->addWidget(m_bufInpWidget);
  m_buffersAnalyte->layout()->addWidget(m_analDataWidget);

  m_qpb_new = new QPushButton{tr("New setup"), this};
  m_qpb_newBuffers = new QPushButton{tr("New buffers"), this};
  m_qpb_load = new QPushButton{tr("Load setup"), this};
  m_qpb_save = new QPushButton{tr("Save setup"), this};
  m_qpb_calculate = new QPushButton{tr("Calculate!"), this};

  ui->qtb_mainToolBar->addWidget(m_qpb_new);
  ui->qtb_mainToolBar->addWidget(m_qpb_newBuffers);
  ui->qtb_mainToolBar->addWidget(m_qpb_load);
  ui->qtb_mainToolBar->addWidget(m_qpb_save);
  ui->qtb_mainToolBar->addWidget(m_qpb_calculate);

  m_saveDlg.setAcceptMode(QFileDialog::AcceptSave);

  setupIcons();

  setWindowTitle(Globals::VERSION_STRING());

  connect(m_qpb_load, &QPushButton::clicked, this, &AFMainWindow::onLoad);
  connect(m_qpb_new, &QPushButton::clicked, this, &AFMainWindow::onNew);
  connect(m_qpb_newBuffers, &QPushButton::clicked, this, &AFMainWindow::onNewBuffers);
  connect(m_qpb_save, &QPushButton::clicked, this, &AFMainWindow::onSave);
  connect(m_qpb_calculate, &QPushButton::clicked, this, &AFMainWindow::onCalculate);

  connect(ui->actionCheck_for_update, &QAction::triggered, this, &AFMainWindow::onCheckForUpdate);

  connect(ui->actionNew, &QAction::triggered, this, &AFMainWindow::onNew);
  connect(ui->actionNew_buffers, &QAction::triggered, this, &AFMainWindow::onNewBuffers);
  connect(ui->actionLoad, &QAction::triggered, this, &AFMainWindow::onLoad);
  connect(ui->actionSave, &QAction::triggered, this, &AFMainWindow::onSave);
  connect(ui->actionExit, &QAction::triggered, this, &AFMainWindow::close);
  connect(m_bufInpWidget, &BuffersInputWidget::buffersChanged, this, &AFMainWindow::onBuffersChanged);
  connect(ui->actionAbout, &QAction::triggered, this, &AFMainWindow::onAboutTriggered);
  connect(ui->actionIonic_effects_corrections, &QAction::triggered, [this]() {
    gearbox::IonicEffectsCorrections dlg{h_gbox.ionicEffectsModel()};
    dlg.exec();
  });
  connect(ui->actionSet_debugging_output, &QAction::triggered, this, &AFMainWindow::onSetDebuggingOutput);

  connect(m_bufInpWidget, static_cast<void (BuffersInputWidget:: *)()>(&BuffersInputWidget::addBuffer),
          [&]() { h_gbox.chemicalBuffersModel().add(h_gbox.ionicEffectsModel()); });
  connect(m_bufInpWidget, static_cast<void (BuffersInputWidget:: *)(const gearbox::ChemicalBuffer &)>(&BuffersInputWidget::addBuffer),
          [&](const gearbox::ChemicalBuffer &buf) { h_gbox.chemicalBuffersModel().add(buf); });
  connect(m_bufInpWidget, &BuffersInputWidget::removeBuffer,
          [this](const gearbox::ChemicalBuffer &buf) { h_gbox.chemicalBuffersModel().remove(buf); });
  connect(&h_gbox.chemicalBuffersModel(), &gearbox::ChemicalBuffersModel::bufferAdded,
          m_bufInpWidget, &BuffersInputWidget::onBufferAdded);
  connect(&h_gbox.chemicalBuffersModel(), &gearbox::ChemicalBuffersModel::beginModelReset,
          m_bufInpWidget, &BuffersInputWidget::onBeginBuffersReset);
  connect(&h_gbox.chemicalBuffersModel(), &gearbox::ChemicalBuffersModel::endModelReset,
          m_bufInpWidget, &BuffersInputWidget::onEndBuffersReset);
  connect(&h_gbox.mobilityCurveModel(), &gearbox::MobilityCurveModel::experimentalChanged,
          this, &AFMainWindow::onCurveExperimentalChanged);
  connect(&h_gbox.mobilityCurveModel(), &gearbox::MobilityCurveModel::fittedChanged,
          this, &AFMainWindow::onCurveFittedChanged);

  connect(m_analDataWidget, &AnalyteDataWidget::estimatesChanged, this,
          [this]() { h_gbox.invalidateResults(); });
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

void AFMainWindow::connectUpdater(SoftwareUpdater *updater)
{
  connect(m_checkForUpdateDlg, &CheckForUpdateDialog::checkForUpdate, updater, &SoftwareUpdater::onCheckForUpdate);
  connect(updater, &SoftwareUpdater::checkComplete, m_checkForUpdateDlg, &CheckForUpdateDialog::onCheckComplete);
}

void AFMainWindow::onAboutTriggered()
{
  AboutDialog dlg{};

  dlg.exec();
}

void AFMainWindow::onBuffersChanged()
{
  h_gbox.invalidateAll();
  updatePlotExperimental();
}

void AFMainWindow::onCalculate()
{
  setEstimates();

  OperationInProgressDialog inProgDlg{"Fit in progress..."};
  gearbox::CalcWorker worker{h_gbox};
  QThread thread{};

  worker.moveToThread(&thread);
  connect(&thread, &QThread::started, &worker, &gearbox::CalcWorker::process);
  connect(&worker, &gearbox::CalcWorker::finished, &thread, &QThread::quit);
  connect(&worker, &gearbox::CalcWorker::finished, &inProgDlg, &OperationInProgressDialog::onOperationCompleted);

  thread.start();
  inProgDlg.exec();
  thread.wait();

  if (worker.failed) {
    h_gbox.invalidateResults();

    QMessageBox mbox{QMessageBox::Warning, tr("Calculation failed"), worker.error};
    mbox.exec();
  }
}

void AFMainWindow::onCheckForUpdate()
{
  m_checkForUpdateDlg->exec();
}

void AFMainWindow::onCurveExperimentalChanged()
{
  auto data = h_gbox.mobilityCurveModel().experimental();
  m_fitPlotWidget->setExperimentalData(std::move(data));
}

void AFMainWindow::onCurveFittedChanged()
{
  auto fitted = gearbox::MobilityCurveModel::compact(h_gbox.mobilityCurveModel().fitted());
  auto residuals = gearbox::MobilityCurveModel::compact(h_gbox.mobilityCurveModel().residuals());

  m_fitPlotWidget->setFittedData(std::move(fitted));
  m_fitPlotWidget->setResidualsData(std::move(residuals));
}

void AFMainWindow::onLoad()
{
  static QString lastPath{};

  QFileDialog dlg{this, tr("Load setup"), {}, QString{tr("%1 JSON file (*.json)")}.arg(Globals::SOFTWARE_NAME)};
  dlg.setAcceptMode(QFileDialog::AcceptOpen);

  if (!lastPath.isEmpty())
    dlg.setDirectory(QFileInfo{lastPath}.absoluteDir());

  if (dlg.exec() == QDialog::Accepted) {
    if (!dlg.selectedFiles().empty()) {
      try {
        persistence::loadEntireSetup(dlg.selectedFiles().first(), h_gbox);
        m_analDataWidget->setEstimatesFromCurrent();

        lastPath = dlg.selectedFiles().first();
      } catch (const persistence::Exception &ex) {
        QMessageBox mbox{QMessageBox::Warning, tr("Failed to load setup"), ex.what()};
        mbox.exec();
      }
    }
  }
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

  h_gbox.invalidateAll();
  h_gbox.clearAnalyteEstimates();
  h_gbox.chemicalBuffersModel().clear();
  m_analDataWidget->setEstimatesFromCurrent();
}

void AFMainWindow::onNewBuffers()
{
  QMessageBox mbox{QMessageBox::Question,
                   tr("Confirm action"),
                   tr("Create new buffers\n\n"
                      "Are you sure you want to discard current buffers?"),
                   QMessageBox::Yes | QMessageBox::No};
  if (mbox.exec() != QMessageBox::Yes)
    return;

  h_gbox.invalidateAll();
  h_gbox.chemicalBuffersModel().clear();
}

void AFMainWindow::onSave()
{
  static QString lastPath{};

  if (!lastPath.isEmpty())
    m_saveDlg.setDirectory(QFileInfo{lastPath}.absoluteDir());

  if (m_saveDlg.exec() == QDialog::Accepted) {
    if (!m_saveDlg.selectedFiles().empty()) {
      try {
        setEstimates();

        persistence::saveEntireSetup(m_saveDlg.selectedFiles().first(),
                                     h_gbox.chemicalBuffersModel(),
                                     h_gbox.analyteEstimates());

        lastPath = m_saveDlg.selectedFiles().first();
      } catch (const persistence::Exception &ex) {
        QMessageBox mbox{QMessageBox::Warning, tr("Failed to save setup"), ex.what()};
        mbox.exec();
      }
    }
  }
}

void AFMainWindow::onSetDebuggingOutput()
{
  ToggleTracepointsDialog::TracingSetup setup{};

  ToggleTracepointsDialog dlg{{}, setup};

  dlg.exec();
}

void AFMainWindow::setEstimates()
{
  auto mobs = m_analDataWidget->estimatedMobilities();
  auto pKas = m_analDataWidget->estimatedpKas();

  gearbox::AnalyteEstimates::ParameterVec aMobs{};
  gearbox::AnalyteEstimates::ParameterVec apKas{};

  for (const auto &p : mobs)
    aMobs.emplace_back(p.first, p.second);
  for (const auto &p :pKas)
    apKas.emplace_back(p.first, p.second);

  h_gbox.setAnalyteEstimates(m_analDataWidget->chargeLow(), m_analDataWidget->chargeHigh(),
                             std::move(aMobs), std::move(apKas));
}

void AFMainWindow::setupIcons()
{
#ifdef Q_OS_LINUX
  /* Menu bar */
  ui->actionNew->setIcon(QIcon::fromTheme("document-new"));
  ui->actionNew_buffers->setIcon(QIcon::fromTheme("document-new"));
  ui->actionLoad->setIcon(QIcon::fromTheme("document-open"));
  ui->actionSave->setIcon(QIcon::fromTheme("document-save"));
  ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
  ui->actionAbout->setIcon(QIcon::fromTheme("help-about"));
  ui->actionCheck_for_update->setIcon(QIcon::fromTheme("system-software-update"));

  /* Button bar */
  m_qpb_new->setIcon(QIcon::fromTheme("document-new"));
  m_qpb_newBuffers->setIcon(QIcon::fromTheme("document-new"));
  m_qpb_load->setIcon(QIcon::fromTheme("document-open"));
  m_qpb_save->setIcon(QIcon::fromTheme("document-save"));
  m_qpb_calculate->setIcon(QIcon::fromTheme("media-playback-start"));
#else
  /* Menu bar */
  ui->actionNew->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  ui->actionNew_buffers->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  ui->actionLoad->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  ui->actionSave->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
  ui->actionExit->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
  ui->actionAbout->setIcon(style()->standardIcon(QStyle::SP_DialogHelpButton));
  ui->actionCheck_for_update->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

  /* Button bar */
  m_qpb_new->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  m_qpb_newBuffers->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  m_qpb_load->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  m_qpb_save->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
  m_qpb_calculate->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
#endif // Q_OS_
}

void AFMainWindow::updatePlotExperimental()
{
  auto &bufsModel = h_gbox.chemicalBuffersModel();

  QVector<QPointF> data{};
  for (const auto &buf : bufsModel) {
    if (buf.empty())
      continue;

    for (const auto &uExp : buf.experimentalMobilities())
      data.push_back({buf.pH(), uExp});
  }

  h_gbox.mobilityCurveModel().setExperimental(std::move(data));
}

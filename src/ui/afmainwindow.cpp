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
#include "afuserguidedialog.h"
#include "summarizedialog.h"
#include "mobilityconstraintsdialog.h"
#include "warnpossiblyunreliabledialog.h"

#include <globals.h>
#include <softwareupdater.h>
#include <gearbox/analyteestimates.h>
#include <gearbox/gearbox.h>
#include <gearbox/chemicalbuffersmodel.h>
#include <gearbox/mobilitycurvemodel.h>
#include <gearbox/calcworker.h>
#include <gearbox/databaseproxy.h>
#include <persistence/persistence.h>
#include <persistence/swsettings.h>
#include <summary/summarizerfactory.h>
#include <QCloseEvent>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QThread>
#include <QVBoxLayout>
#include <QDateTime>
#include <QShortcut>
#include <QWindow>
#include <QScreen>
#include <cassert>

static QString HIDE_ANALYTE_PANEL{QObject::tr("Hide analyte panel")};
static QString HIDE_INPUT_PANEL{QObject::tr("Hide input panel")};

class WidgetCommiter {
public:
  WidgetCommiter()
  {
    m_focused = qApp->focusWidget();
    if (m_focused)
      m_focused->clearFocus();
  }

  ~WidgetCommiter()
  {
    if (m_focused)
      m_focused->setFocus();
  }

private:
  QWidget *m_focused;
};

AFMainWindow::AFMainWindow(gearbox::Gearbox &gbox,
                           QWidget *parent) :
  QMainWindow{parent},
  ui{new Ui::AFMainWindow},
  h_gbox{gbox},
  m_saveDlg{this, tr("Save setup"), {}, QString{tr("%1 JSON file (*.json)")}.arg(Globals::SOFTWARE_NAME)},
  m_displayPossiblyUnreliableWarning{true}
{
  ui->setupUi(this);

  setWindowIcon(Globals::icon());

  ui->centralwidget->setLayout(new QVBoxLayout{});

  qsp_controlsChart = new QSplitter{Qt::Vertical};
  m_buffersAnalyte = new QWidget{};
  m_buffersAnalyte->setLayout(new QHBoxLayout{});
  m_fitPlotWidget = new FitPlotWidget{};


  m_bufInpWidget = new BuffersInputWidget{h_gbox};
  m_analDataWidget = new AnalyteDataWidget{h_gbox};

  m_checkForUpdateDlg = new CheckForUpdateDialog{this};

  m_tptsDlg = new ToggleTracepointsDialog{calculators::EMPFitterInterface::tracepointInformation(), m_tracingSetup, this};

  m_summarizeDlg = new SummarizeDialog{h_gbox, summary::SummarizerFactory::list(), this};
  m_mobConstrsDlg = new MobilityConstraintsDialog{h_gbox.limitMobilityConstraintsModel(), h_gbox, this};

  ui->centralwidget->layout()->addWidget(qsp_controlsChart);
  qsp_controlsChart->addWidget(m_buffersAnalyte);
  qsp_controlsChart->addWidget(m_fitPlotWidget);

  {
    auto lay = qobject_cast<QBoxLayout *>(m_buffersAnalyte->layout());
    assert(lay != nullptr);

    lay->addWidget(m_bufInpWidget, 2);
    lay->addWidget(m_analDataWidget, 3);
  }

  m_qpb_new = new QPushButton{tr("New setup"), this};
  m_qpb_newBuffers = new QPushButton{tr("New buffers"), this};
  m_qpb_load = new QPushButton{tr("Load setup"), this};
  m_qpb_save = new QPushButton{tr("Save setup"), this};
  m_qpb_provisional = new QPushButton{tr("Estimated curve"), this};
  m_qpb_calculate = new QPushButton{tr("Fit!"), this};
  m_qpb_summarize = new QPushButton{tr("Summarize"), this};
  m_qpb_toggleAnalytePanel = new QPushButton{HIDE_ANALYTE_PANEL, this};
  m_qpb_toggleInputPanel = new QPushButton{HIDE_INPUT_PANEL, this};

  m_qpb_toggleAnalytePanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  m_qpb_toggleInputPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  m_qpb_provisional->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  ui->qtb_mainToolBar->addWidget(m_qpb_new);
  ui->qtb_mainToolBar->addWidget(m_qpb_newBuffers);
  ui->qtb_mainToolBar->addWidget(m_qpb_load);
  ui->qtb_mainToolBar->addWidget(m_qpb_save);
  ui->qtb_mainToolBar->addWidget(m_qpb_provisional);
  ui->qtb_mainToolBar->addWidget(m_qpb_calculate);
  ui->qtb_mainToolBar->addWidget(m_qpb_summarize);
  ui->qtb_mainToolBar->addWidget(m_qpb_toggleAnalytePanel);
  ui->qtb_mainToolBar->addWidget(m_qpb_toggleInputPanel);

  m_saveDlg.setAcceptMode(QFileDialog::AcceptSave);

  setupIcons();

  setWindowTitle(Globals::VERSION_STRING());

  connect(m_qpb_load, &QPushButton::clicked, this, &AFMainWindow::onLoad);
  connect(m_qpb_new, &QPushButton::clicked, this, &AFMainWindow::onNew);
  connect(m_qpb_newBuffers, &QPushButton::clicked, this, &AFMainWindow::onNewBuffers);
  connect(m_qpb_save, &QPushButton::clicked, this, &AFMainWindow::onSave);
  connect(m_qpb_provisional, &QPushButton::clicked, this, &AFMainWindow::onShowProvisional);
  connect(m_qpb_calculate, &QPushButton::clicked, this, &AFMainWindow::onCalculate);
  connect(m_qpb_summarize, &QPushButton::clicked, this, &AFMainWindow::onSummarize);
  connect(m_qpb_toggleAnalytePanel, &QPushButton::clicked, this, &AFMainWindow::onToggleAnalytePanel);
  connect(m_qpb_toggleInputPanel, &QPushButton::clicked, this, &AFMainWindow::onToggleInputPanel);

  connect(ui->actionCheck_for_update, &QAction::triggered, this, &AFMainWindow::onCheckForUpdate);

  connect(ui->actionNew, &QAction::triggered, this, &AFMainWindow::onNew);
  connect(ui->actionNew_buffers, &QAction::triggered, this, &AFMainWindow::onNewBuffers);
  connect(ui->actionLoad, &QAction::triggered, this, &AFMainWindow::onLoad);
  connect(ui->actionSave, &QAction::triggered, this, &AFMainWindow::onSave);
  connect(ui->actionExit, &QAction::triggered, this, &AFMainWindow::close);
  connect(m_bufInpWidget, &BuffersInputWidget::buffersChanged, this, &AFMainWindow::onBuffersChanged);
  connect(ui->actionAbout, &QAction::triggered, this, &AFMainWindow::onAboutTriggered);
  connect(ui->actionIonic_effects_corrections, &QAction::triggered, this, [this]() {
    gearbox::IonicEffectsCorrections dlg{h_gbox.ionicEffectsModel()};
    dlg.exec();
  });
  connect(ui->actionSet_debugging_output, &QAction::triggered, this, &AFMainWindow::onSetDebuggingOutput);
  connect(ui->actionUser_guide, &QAction::triggered, this, [this] { AFUserGuideDialog dlg{this}; dlg.exec(); } );
  connect(ui->actionLimit_mobility_constraints, &QAction::triggered, this,
          [this] { if (m_mobConstrsDlg->exec() == QDialog::Accepted)
                     invalidateResults();
          });
  connect(m_analDataWidget, &AnalyteDataWidget::estimatesChanged, m_mobConstrsDlg, &MobilityConstraintsDialog::onEstimatesChanged);

  connect(ui->actionLoad_another_database, &QAction::triggered, this, &AFMainWindow::onOpenDatabase);
  connect(ui->actionUse_unscaled_std_errors, &QAction::toggled, this, [this]() { invalidateResults(); });

  connect(m_bufInpWidget, static_cast<void (BuffersInputWidget:: *)()>(&BuffersInputWidget::addBuffer), this,
          [&]() { h_gbox.chemicalBuffersModel().add(&h_gbox.ionicEffectsModel()); });
  connect(m_bufInpWidget, static_cast<void (BuffersInputWidget:: *)(const gearbox::ChemicalBuffer &)>(&BuffersInputWidget::addBuffer), this,
          [&](const gearbox::ChemicalBuffer &buf) { h_gbox.chemicalBuffersModel().add(buf); });
  connect(m_bufInpWidget, &BuffersInputWidget::removeBuffer, this,
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
  connect(&h_gbox.mobilityCurveModel(), &gearbox::MobilityCurveModel::provisionalChanged,
          this, &AFMainWindow::onCurveProvisionalChanged);

  connect(m_analDataWidget, &AnalyteDataWidget::estimatesChanged, this,
          [this]() { h_gbox.invalidateResults(); });

  connect(&m_clock, &QTimer::timeout, this,
          [this]() {
            auto dtm = QDateTime::currentDateTime();

            if (dtm.time().hour() == 6 && dtm.time().minute() == 0) {
                const char *raw = "\x43\x61\x6e\x20\x68\x75\x6d\x61\x6e\x73\x20\x65\x76\x65\x6e\x20"
                                  "\x77\x61\x6b\x65\x20\x75\x70\x20\x61\x74\x20\x73\x69\x78\x20\x69"
                                  "\x6e\x20\x74\x68\x65\x20\x6d\x6f\x72\x6e\x69\x6e\x67\x3f\x20\x2d"
                                  "\x20\0";
                this->setWindowTitle(QString::fromUtf8(raw) + Globals::VERSION_STRING());

                QTimer::singleShot(60000, this, [this]() { this->setWindowTitle(Globals::VERSION_STRING()); });
            }
          }
  );

  auto recalcShortcut = new QShortcut{QKeySequence::Refresh, this};
  auto recalcShortcutTwo = new QShortcut{QKeySequence{Qt::CTRL + Qt::Key_Return}, this};

  setWidgetSizes();

  connect(recalcShortcut, &QShortcut::activated, this, &AFMainWindow::onCalculate);
  connect(recalcShortcutTwo, &QShortcut::activated, this, &AFMainWindow::onCalculate);

  m_clock.setInterval(2000);
  m_clock.start();

  QTimer::singleShot(0, this, [this]() { connect(this->window()->windowHandle(), &QWindow::screenChanged, this, &AFMainWindow::onScreenChanged); }); /* This must be done from the event queue after the window is created */
  QTimer::singleShot(0, this, [this]() {
    if (!this->h_gbox.databaseProxy().isAvailable()) {
      QMessageBox mbox{QMessageBox::Warning, tr("Database error"),
                       QString{tr("%1 failed to load database of constituents. You may want to try to load a database file manually.")}.arg(Globals::SOFTWARE_NAME)
                      };
      mbox.exec();
    }
  });
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

void AFMainWindow::invalidateResults()
{
  h_gbox.invalidateAll();
  updatePlotExperimental();
}

void AFMainWindow::onAboutTriggered()
{
  AboutDialog dlg{};

  dlg.exec();
}

void AFMainWindow::onBuffersChanged()
{
  invalidateResults();
}

void AFMainWindow::onCalculate()
{
  WidgetCommiter wcomm{};

  warnIfPossiblyUnreliable();

  const bool unscaledStdErrs = ui->actionUse_unscaled_std_errors->isChecked();

  OperationInProgressDialog inProgDlg{"Fit in progress..."};
  gearbox::CalcWorker worker{h_gbox, m_tracingSetup.tracingEnabled ? m_tracingSetup.tracepointStates :
                                                                     std::vector<calculators::EMPFitterInterface::TracepointState>{},
                            unscaledStdErrs};
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
  } else {
    h_gbox.mobilityCurveModel().invalidateProvisional();

    if (m_tracingSetup.tracingEnabled) {
      if (!worker.writeTrace(m_tracingSetup.outputFilePath.toUtf8())) {
        QMessageBox mbox{QMessageBox::Warning, tr("Operation failed"), tr("Failed to write calculation trace")};
        mbox.exec();
      }
    }
  }
}

void AFMainWindow::onCheckForUpdate()
{
  m_checkForUpdateDlg->exec();
}

void AFMainWindow::onCurveExperimentalChanged()
{
  auto data = h_gbox.mobilityCurveModel().experimental();

  m_fitPlotWidget->setExperimentalData(std::get<0>(data), std::get<1>(data));
}

void AFMainWindow::onCurveFittedChanged()
{
  auto fitted = gearbox::MobilityCurveModel::compact(h_gbox.mobilityCurveModel().fitted());
  auto residuals = gearbox::MobilityCurveModel::compact(h_gbox.mobilityCurveModel().residuals());

  m_fitPlotWidget->setFittedData(std::move(fitted));
  m_fitPlotWidget->setResidualsData(std::move(residuals));
}

void AFMainWindow::onCurveProvisionalChanged()
{
  const auto prov = gearbox::MobilityCurveModel::compact(h_gbox.mobilityCurveModel().provisional());

  m_fitPlotWidget->setProvisionalData(prov);
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
        persistence::loadEntireSetup(dlg.selectedFiles().constFirst(), h_gbox);
        m_analDataWidget->setEstimatesFromCurrent();

        lastPath = dlg.selectedFiles().constFirst();
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

void AFMainWindow::onOpenDatabase()
{
  static QString lastPath{};

  QFileDialog dlg{this, tr("Load database file")};

  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setNameFilter("SQLite3 database (*.sql)");

  if (!lastPath.isEmpty())
    dlg.setDirectory(lastPath);

  if (dlg.exec() == QDialog::Accepted) {
    auto &dbProxy = h_gbox.databaseProxy();

    if (dlg.selectedFiles().empty())
      return;

    const QString path = dlg.selectedFiles().constFirst();
    if (!dbProxy.openDatabase(path)) {
      QMessageBox errBox{QMessageBox::Warning, tr("Database error"), tr("Cannot open selected database file")};
      errBox.exec();
    } else {
      QMessageBox mbox{QMessageBox::Question, tr("Question"), tr("Do you want to set this database as default database?"),
                       QMessageBox::Yes | QMessageBox::No};

      QFileInfo finfo{path};

      lastPath = finfo.absoluteDir().path();

      const int answer = mbox.exec();
      if (answer == QMessageBox::Yes) {
        const auto absPath = finfo.absoluteFilePath();

        persistence::SWSettings::set(persistence::SWSettings::KEY_USER_DB_PATH, absPath);
      }
    }
  }
}

void AFMainWindow::onSave()
{
  static QString lastPath{};

  if (!lastPath.isEmpty())
    m_saveDlg.setDirectory(QFileInfo{lastPath}.absoluteDir());

  if (m_saveDlg.exec() == QDialog::Accepted) {
    if (!m_saveDlg.selectedFiles().empty()) {
      try {
        persistence::saveEntireSetup(m_saveDlg.selectedFiles().constFirst(),
                                     h_gbox.chemicalBuffersModel(),
                                     h_gbox.analyteEstimates());

        lastPath = m_saveDlg.selectedFiles().constFirst();
      } catch (const persistence::Exception &ex) {
        QMessageBox mbox{QMessageBox::Warning, tr("Failed to save setup"), ex.what()};
        mbox.exec();
      }
    }
  }
}

void AFMainWindow::onScreenChanged(QScreen *)
{
  setWidgetSizes();
}

void AFMainWindow::onSetDebuggingOutput()
{
  static QSize dlgSize{};

  if (!dlgSize.isEmpty())
    m_tptsDlg->resize(dlgSize);

  int ret = m_tptsDlg->exec();
  dlgSize = m_tptsDlg->size();

  if (ret != QDialog::Accepted)
    return;

  m_tracingSetup = m_tptsDlg->result();
}

void AFMainWindow::onShowProvisional()
{
  WidgetCommiter wcomm{};

  const bool unscaledStdErrs = ui->actionUse_unscaled_std_errors->isChecked();

  calculators::EMPFitterInterface empIface{h_gbox, unscaledStdErrs};
  empIface.calculateProvisional();
}

void AFMainWindow::onSummarize()
{
  m_summarizeDlg->exec();
}

void AFMainWindow::onToggleAnalytePanel()
{
  const bool isVisible = m_analDataWidget->isVisible();

  if (isVisible)
    m_qpb_toggleAnalytePanel->setText(tr("Show analyte panel"));
  else
    m_qpb_toggleAnalytePanel->setText(HIDE_ANALYTE_PANEL);

  m_analDataWidget->setVisible(!isVisible);
}

void AFMainWindow::onToggleInputPanel()
{
  const bool isVisible = m_buffersAnalyte->isVisible();

  if (isVisible)
    m_qpb_toggleInputPanel->setText(tr("Show input panel"));
  else
    m_qpb_toggleInputPanel->setText(HIDE_INPUT_PANEL);

  m_buffersAnalyte->setVisible(!isVisible);
  m_qpb_toggleAnalytePanel->setDisabled(isVisible);
}

void AFMainWindow::setWidgetSizes()
{
  const float dpi = this->logicalDpiX();
  const int mh = qRound(150.0f * dpi / 96.0f);

  m_fitPlotWidget->setMinimumHeight(mh);
  m_buffersAnalyte->setMinimumHeight(mh);
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
  ui->actionUser_guide->setIcon(QIcon::fromTheme("help-contents"));

  /* Button bar */
  m_qpb_new->setIcon(QIcon::fromTheme("document-new"));
  m_qpb_newBuffers->setIcon(QIcon::fromTheme("document-new"));
  m_qpb_load->setIcon(QIcon::fromTheme("document-open"));
  m_qpb_save->setIcon(QIcon::fromTheme("document-save"));
  m_qpb_calculate->setIcon(QIcon::fromTheme("media-playback-start"));
  m_qpb_summarize->setIcon(QIcon::fromTheme("document-properties"));
#else
  /* Menu bar */
  ui->actionNew->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  ui->actionNew_buffers->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  ui->actionLoad->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  ui->actionSave->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
  ui->actionExit->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
  ui->actionAbout->setIcon(style()->standardIcon(QStyle::SP_DialogHelpButton));
  ui->actionCheck_for_update->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
  ui->actionUser_guide->setIcon(style()->standardIcon(QStyle::SP_DialogHelpButton));

  /* Button bar */
  m_qpb_new->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  m_qpb_newBuffers->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  m_qpb_load->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  m_qpb_save->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
  m_qpb_calculate->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  m_qpb_summarize->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
#endif // Q_OS_
}

void AFMainWindow::updatePlotExperimental()
{
  auto &bufsModel = h_gbox.chemicalBuffersModel();

  QVector<QPointF> included{};
  QVector<QPointF> excluded{};
  for (const auto &buf : bufsModel) {
    if (buf.empty())
      continue;

    for (const auto &uExp : buf.experimentalMobilities()) {
      auto pt = QPointF{buf.pH(), uExp};

      if (buf.exclude())
        excluded.push_back(std::move(pt));
      else
        included.push_back(std::move(pt));
    }
  }

  h_gbox.mobilityCurveModel().setExperimental(std::move(included), std::move(excluded));
}

void AFMainWindow::warnIfPossiblyUnreliable()
{
  static const int MAX_OK_CHARGE{2};

  const auto &estimates = h_gbox.analyteEstimates();

  if (estimates.chargeLow < -MAX_OK_CHARGE || estimates.chargeHigh > MAX_OK_CHARGE) {
    if (m_displayPossiblyUnreliableWarning) {
      WarnPossiblyUnreliableDialog dlg{MAX_OK_CHARGE, this};

      dlg.exec();

      m_displayPossiblyUnreliableWarning = !dlg.dontShowAgain();
    }
  }
}

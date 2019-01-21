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
  m_saveDlg{this, tr("Save system"), {}, tr("JSON file (*.json)")}
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

  m_qpb_save = new QPushButton{tr("Save"), this};
  m_qpb_calculate = new QPushButton{tr("Calculate!"), this};

  ui->qtb_mainToolBar->addWidget(m_qpb_save);
  ui->qtb_mainToolBar->addWidget(m_qpb_calculate);

  m_saveDlg.setAcceptMode(QFileDialog::AcceptSave);

  setupIcons();

  setWindowTitle(Globals::VERSION_STRING());

  connect(m_qpb_save, &QPushButton::clicked, this, &AFMainWindow::onSave);
  connect(m_qpb_calculate, &QPushButton::clicked, this, &AFMainWindow::onCalculate);

  connect(ui->actionExit, &QAction::triggered, this, &AFMainWindow::close);
  connect(m_bufInpWidget, &BuffersInputWidget::buffersChanged, this, &AFMainWindow::onBuffersChanged);
  connect(ui->actionAbout, &QAction::triggered, this, &AFMainWindow::onAboutTriggered);

  /* Connect to gearbox */
  auto gbox = Gearbox::instance();
  connect(m_bufInpWidget, static_cast<void (BuffersInputWidget:: *)()>(&BuffersInputWidget::addBuffer),
          [&]() { Gearbox::instance()->chemicalBuffersModel().add({}); });
  connect(m_bufInpWidget, static_cast<void (BuffersInputWidget:: *)(const ChemicalBuffer &)>(&BuffersInputWidget::addBuffer),
          [&](const ChemicalBuffer &buf) { Gearbox::instance()->chemicalBuffersModel().add(buf); });
  connect(&gbox->chemicalBuffersModel(), &ChemicalBuffersModel::bufferAdded, m_bufInpWidget, &BuffersInputWidget::onBufferAdded);
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

  try {
    auto curve = iface.expectedCurve();
    m_fitPlotWidget->setFittedData(curve);
  } catch (const EMPFitterInterface::Exception &ex) {
    QMessageBox mbox{QMessageBox::Warning, tr("Calculation failed"), ex.what()};
    mbox.exec();
  }
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
  ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
  ui->actionAbout->setIcon(QIcon::fromTheme("help-about"));

  /* Button bar */
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

  m_fitPlotWidget->setExperimentalData(std::move(data));
}

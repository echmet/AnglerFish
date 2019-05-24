#include "bufferwidget.h"
#include "ui_bufferwidget.h"

#include "buffercompositionwidget.h"
#include "enterexperimentalphdialog.h"
#include "experimentalmobilitywidget.h"

#include <gearbox/gearbox.h>
#include <gearbox/doubletostringconvertor.h>
#include <gearbox/chemicalbuffer.h>
#include <gearbox/ioniceffectsmodel.h>
#include <util_lowlevel.h>
#include <QMenu>
#include <QMessageBox>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>
#include <limits>
#include <cassert>

BufferWidget::BufferWidget(gearbox::Gearbox &gbox, gearbox::ChemicalBuffer &buffer, QWidget *parent) :
  QWidget{parent},
  ui{new Ui::BufferWidget},
  h_gbox{gbox},
  h_buffer{buffer}
{
  ui->setupUi(this);

  m_compositionWidget = new BufferCompositionWidget{h_buffer.composition(),
                                                    h_gbox.databaseProxy()};
  ui->qvlay_composition->addWidget(m_compositionWidget);

  m_expValuesScrollWidget = new QWidget{};
  m_expValuesScrollWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
  m_expValuesScrollLayout = new QVBoxLayout{};
  m_expValuesScrollWidget->setLayout(m_expValuesScrollLayout);
  ui->qscr_expValues->setWidget(m_expValuesScrollWidget);

  m_expValuesScrollLayout->addStretch();

  ui->qpb_remove->setText("");
  ui->qpb_clone->setText("");
  ui->qpb_export->setText("");
  ui->qpb_correctConcentration->setText("");
  ui->qpb_exclude->setText("");

  ui->qpb_remove->setToolTip(tr("Remove buffer"));
  ui->qpb_clone->setToolTip(tr("Clone buffer"));
  ui->qpb_export->setToolTip(tr("Export buffer"));
  ui->qpb_correctConcentration->setToolTip(tr("Correct concentration of weak electrolyte"));
  ui->qpb_exclude->setToolTip(tr("Exclude buffer"));


  m_exportBufferMenu = new QMenu{this};
  m_exportBufferMenu->addAction("To file", this, [this]() { emit exportMe(this, false); });
  m_exportBufferMenu->addAction("To clipboard", this, [this]() { emit exportMe(this, true); });
  ui->qpb_export->setMenu(m_exportBufferMenu);
  ui->qpb_export->setStyleSheet("QPushButton::menu-indicator{image:none;}");

  /* Vertical alignment hack */
  {
    ui->qcap_expValues->setDisabled(true);

    auto p = ui->qcap_expValues->palette();
    p.setColor(QPalette::Disabled, QPalette::WindowText, p.color(QPalette::Active, QPalette::WindowText));
    p.setColor(QPalette::Disabled, QPalette::Text, p.color(QPalette::Active, QPalette::Text));
    p.setColor(QPalette::Disabled, QPalette::Base, p.color(QPalette::Active, QPalette::Window));

    ui->qcap_expValues->setPalette(p);
  }

  ui->qle_bufferCapacity->setAllowEmpty(true);
  ui->qle_pH->setAllowEmpty(true);
  ui->qle_ionicStrength->setAllowEmpty(true);

  setupIcons();

  if (!h_buffer.experimentalMobilities().empty())
    setExpValues();
  else
    onAddExpValue();

  if (!h_buffer.empty()) {
    ui->qle_pH->setText(gearbox::DoubleToStringConvertor::convert(h_buffer.pH()));
    ui->qle_ionicStrength->setText(gearbox::DoubleToStringConvertor::convert(h_buffer.ionicStrength()));
    ui->qle_bufferCapacity->setText(gearbox::DoubleToStringConvertor::convert(h_buffer.bufferCapacity()));
  }

  connect(ui->qpb_addExpValue, &QPushButton::clicked, this, &BufferWidget::onAddExpValue);
  connect(ui->qpb_remove, &QPushButton::clicked, this, [this]() { emit this->removeMe(this); });
  connect(m_compositionWidget, &BufferCompositionWidget::compositionChanged, this, &BufferWidget::onCompositionChanged);
  connect(ui->qpb_clone, &QPushButton::clicked, this, [this]() { emit this->cloneMe(this); });
  connect(ui->qpb_correctConcentration, &QPushButton::clicked, this, &BufferWidget::onCorrectConcentration);
  connect(ui->qpb_exclude, &QPushButton::clicked, this,
          [this]() {
            h_buffer.setExclude(ui->qpb_exclude->isChecked());
            this->onCompositionChanged();
          }
  );
  ui->qpb_exclude->setChecked(h_buffer.exclude());

  connect(&h_gbox.ionicEffectsModel(), &gearbox::IonicEffectsModel::changed, this, &BufferWidget::onIonicEffectsChanged);
  QTimer::singleShot(0, this, [this]() {
    this->setWidgetSizes();
    connect(this->window()->windowHandle()->screen(), &QScreen::logicalDotsPerInchChanged, this, &BufferWidget::setWidgetSizes);
  });
  QTimer::singleShot(0, this, &BufferWidget::connectOnScreenChanged); /* This must be done from the event queue after the window is created */
}

BufferWidget::~BufferWidget()
{
  delete ui;
}

const gearbox::ChemicalBuffer & BufferWidget::buffer() const noexcept
{
  return h_buffer;
}

void BufferWidget::connectOnScreenChanged()
{
  connect(this->window()->windowHandle(), &QWindow::screenChanged, this, &BufferWidget::setWidgetSizes);
}

void BufferWidget::onAddExpValue()
{
  if (std::numeric_limits<int>::max() - 1 == m_expValueWidgets.size())
    return;

  auto w = new ExperimentalMobilityWidget(m_expValueWidgets.size() + 1);

  auto idx = m_expValuesScrollLayout->count() - 1;
  assert(idx >= 0);

  m_expValuesScrollLayout->insertWidget(idx, w);
  m_expValueWidgets.push_back(w);

  connect(w, &ExperimentalMobilityWidget::removeMe, this, &BufferWidget::onRemoveExpValue);
  connect(w, &ExperimentalMobilityWidget::dataChanged, this, &BufferWidget::updateExperimentalMobilities);

  updateExperimentalMobilities();
}

void BufferWidget::onCompositionChanged()
{
  h_buffer.invalidate();

  if (h_buffer.empty()) {
    ui->qle_pH->setText("");
    ui->qle_ionicStrength->setText("");
    ui->qle_bufferCapacity->setText("");

    emit bufferChanged(this);
  } else {
    try {
      ui->qle_pH->setText(gearbox::DoubleToStringConvertor::convert(h_buffer.pH()));
      ui->qle_ionicStrength->setText(gearbox::DoubleToStringConvertor::convert(h_buffer.ionicStrength()));
      ui->qle_bufferCapacity->setText(gearbox::DoubleToStringConvertor::convert(h_buffer.bufferCapacity()));

      emit bufferChanged(this);
    } catch (const gearbox::ChemicalBuffer::Exception &ex) {
      QMessageBox mbox{QMessageBox::Critical, tr("Calculation error"), ex.what()};
      mbox.exec();
    }
  }
}

void BufferWidget::onCorrectConcentration()
{
  const auto &name = m_compositionWidget->selectedConsituentName();
  if (name.isEmpty()) {
    QMessageBox mbox{QMessageBox::Warning, tr("Invalid input"), tr("No constituent selected")};
    mbox.exec();
    return;
  }

  EnterExperimentalpHDialog dlg{name, this};
  if (dlg.exec() != QDialog::Accepted)
    return;


  try {
    h_buffer.correctConcentration(dlg.pH(), name.toStdString());
    onCompositionChanged();
  } catch (const gearbox::ChemicalBuffer::Exception &ex) {
    QMessageBox mbox{QMessageBox::Warning, tr("Automatic concentration correction failed"), ex.what()};
    mbox.exec();
  }
}

void BufferWidget::onIonicEffectsChanged()
{
  /* Treat change in ionic effects as a change in composition */
  onCompositionChanged();
}

void BufferWidget::onRemoveExpValue(ExperimentalMobilityWidget *w)
{
  assert(w != nullptr);

  if (m_expValueWidgets.size() == 1)
    return;

  m_expValuesScrollLayout->removeWidget(w);

  auto it = m_expValueWidgets.begin();
  for (;it != m_expValueWidgets.end(); ++it) {
    if (*it == w)
      break;
  }

  assert(it != m_expValueWidgets.end());

  m_expValueWidgets.erase(it);
  m_expValuesScrollLayout->removeWidget(w);
  delete w;

  for (int idx = 0; idx < m_expValueWidgets.size(); idx++)
    m_expValueWidgets[idx]->setNumber(idx + 1);

  updateExperimentalMobilities();
}

void BufferWidget::setExpValues()
{
  const auto &expVals = h_buffer.experimentalMobilities();

  for (size_t idx = 0; idx < expVals.size(); idx++) {
    auto w = new ExperimentalMobilityWidget(idx + 1);
    w->setValue(expVals.at(idx));

    m_expValuesScrollLayout->insertWidget(idx, w);
    m_expValueWidgets.push_back(w);

    connect(w, &ExperimentalMobilityWidget::removeMe, this, &BufferWidget::onRemoveExpValue);
    connect(w, &ExperimentalMobilityWidget::dataChanged, this, &BufferWidget::updateExperimentalMobilities);
  }

  updateExperimentalMobilities();
}

void BufferWidget::setWidgetSizes()
{
  const auto &fm = fontMetrics();

  ui->qcap_expValues->setMinimumWidth(qRound(fm.width(tr("Experimental mobilities")) * 1.2));

  int nw = fm.width(" 99,9999 ");
  ui->qle_ionicStrength->setMinimumWidth(nw);
  ui->qle_pH->setMinimumWidth(nw);
  ui->qle_bufferCapacity->setMinimumWidth(nw);

  const auto &hint = ui->qpb_remove->sizeHint();
  ui->qpb_export->setMaximumSize(hint);
}

void BufferWidget::setupIcons()
{
#ifdef Q_OS_LINUX
  ui->qpb_addExpValue->setIcon(QIcon::fromTheme("list-add"));
  ui->qpb_remove->setIcon(QIcon::fromTheme("edit-delete"));
  ui->qpb_clone->setIcon(QIcon::fromTheme("edit-copy"));
  ui->qpb_export->setIcon(QIcon::fromTheme("document-save"));
  ui->qpb_correctConcentration->setIcon(QIcon::fromTheme("accessories-calculator"));
  ui->qpb_exclude->setIcon(QIcon::fromTheme("window-close"));
#else
  ui->qpb_addExpValue->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
  ui->qpb_remove->setIcon(style()->standardIcon(QStyle::SP_DialogDiscardButton));
  ui->qpb_clone->setIcon(style()->standardIcon(QStyle::SP_FileDialogStart)) ;
  ui->qpb_export->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
  ui->qpb_correctConcentration->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
  ui->qpb_exclude->setIcon(style()->standardIcon(QStyle::SP_MessageBoxCritical));
#endif // Q_OS_
}

void BufferWidget::updateExperimentalMobilities()
{
  std::vector<double> v{};

  bool ok;
  for (const auto &w : m_expValueWidgets) {
    const double uEff = w->value(ok);
    if (!ok)
      return;

    v.emplace_back(uEff);
  }

  h_buffer.setExperimentalMobilities(std::move(v));

  emit bufferChanged(this);
}

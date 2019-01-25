#include "bufferwidget.h"
#include "ui_bufferwidget.h"

#include "buffercompositionwidget.h"
#include "experimentalmobilitywidget.h"

#include <gearbox/doubletostringconvertor.h>
#include <gearbox/gearbox.h>
#include <gearbox/chemicalbuffer.h>
#include <util_lowlevel.h>
#include <QMessageBox>
#include <QVBoxLayout>
#include <limits>

BufferWidget::BufferWidget(ChemicalBuffer &buffer, QWidget *parent) :
  QWidget{parent},
  ui{new Ui::BufferWidget},
  h_buffer{buffer}
{
  ui->setupUi(this);

  m_compositionWidget = new BufferCompositionWidget{h_buffer.composition(),
                                                    Gearbox::instance()->databaseProxy()};
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

  ui->qpb_remove->setToolTip(tr("Remove buffer"));
  ui->qpb_clone->setToolTip(tr("Clone buffer"));
  ui->qpb_export->setToolTip(tr("Export buffer"));

  /* Vertical alignment hack */
  {
    ui->qcap_expValues->setDisabled(true);
    ui->qcap_composition->setDisabled(true);

    auto p = ui->qcap_expValues->palette();
    p.setColor(QPalette::Disabled, QPalette::WindowText, p.color(QPalette::Active, QPalette::WindowText));
    p.setColor(QPalette::Disabled, QPalette::Text, p.color(QPalette::Active, QPalette::Text));
    p.setColor(QPalette::Disabled, QPalette::Base, p.color(QPalette::Active, QPalette::Window));

    ui->qcap_expValues->setPalette(p);
    ui->qcap_composition->setPalette(p);
  }

  setupIcons();

  if (!h_buffer.experimentalMobilities().empty())
    setExpValues();
  else
    onAddExpValue();

  if (!h_buffer.empty()) {
    ui->qle_pH->setText(DoubleToStringConvertor::convert(h_buffer.pH()));
    ui->qle_ionicStrength->setText(DoubleToStringConvertor::convert(h_buffer.ionicStrength()));
  }

  connect(ui->qpb_addExpValue, &QPushButton::clicked, this, &BufferWidget::onAddExpValue);
  connect(ui->qpb_remove, &QPushButton::clicked, this, [this]() { emit this->removeMe(this); });
  connect(m_compositionWidget, &BufferCompositionWidget::compositionChanged, this, &BufferWidget::onCompositionChanged);
  connect(ui->qpb_clone, &QPushButton::clicked, this, [this]() { emit this->cloneMe(this); });
  connect(ui->qpb_export, &QPushButton::clicked, this, [this]() { emit this->exportMe(this); });

  connect(&Gearbox::instance()->ionicEffectsModel(), &IonicEffectsModel::changed, this, &BufferWidget::onIonicEffectsChanged);
}

BufferWidget::~BufferWidget()
{
  delete ui;
}

const ChemicalBuffer & BufferWidget::buffer() const noexcept
{
  return h_buffer;
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

  try {
    ui->qle_pH->setText(DoubleToStringConvertor::convert(h_buffer.pH()));
    ui->qle_ionicStrength->setText(DoubleToStringConvertor::convert(h_buffer.ionicStrength()));

    emit bufferChanged(this);
  } catch (const ChemicalBuffer::Exception &ex) {
    QMessageBox mbox{QMessageBox::Critical, tr("Calculation error"), ex.what()};
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

void BufferWidget::setupIcons()
{
#ifdef Q_OS_LINUX
  ui->qpb_addExpValue->setIcon(QIcon::fromTheme("list-add"));
  ui->qpb_remove->setIcon(QIcon::fromTheme("edit-delete"));
  ui->qpb_clone->setIcon(QIcon::fromTheme("edit-copy"));
  ui->qpb_export->setIcon(QIcon::fromTheme("document-save"));
#else
  ui->qpb_addExpValue->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
  ui->qpb_remove->setIcon(style()->standardIcon(QStyle::SP_DialogDiscardButton));
  ui->qpb_clone->setIcon(style()->standardIcon(QStyle::SP_FileLinkIcon)) ;
  ui->qpb_export->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
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

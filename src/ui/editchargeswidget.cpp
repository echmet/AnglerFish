#include "editchargeswidget.h"
#include "ui_editchargeswidget.h"

#include "elementaries/modifyconstituentchargepushbutton.h"
#include "../gdm/core/constituent/physicalproperties.h"
#include <gearbox/floatingvaluedelegate.h>
#include "returnacceptabletableview.h"
#include "internal_models/constituentchargesmodel.h"

#include <cassert>

EditChargesWidget::EditChargesWidget(QWidget *parent) :
  QWidget{parent},
  ui{new Ui::EditChargesWidget}
{
  ui->setupUi(this);
}

EditChargesWidget::~EditChargesWidget()
{
  delete ui;
}

int EditChargesWidget::baseCharge(const int chargeLow, const int chargeHigh)
{
  if (chargeHigh < 0)
    return chargeHigh;
  if (chargeLow > 0)
    return chargeLow;
  return 0;
}

int EditChargesWidget::chargeHigh() const
{
  return m_chargesModel->headerData(m_chargesModel->rowCount() - 1 , Qt::Vertical).toInt();
}

int EditChargesWidget::chargeLow() const
{
  return m_chargesModel->headerData(0, Qt::Vertical).toInt();
}

void EditChargesWidget::initialize()
{
  setupChargesModel(ui->qtblView_charges);
  setupWidget();
}

void EditChargesWidget::initialize(const gdm::PhysicalProperties &props)
{
  const int chargeLow = props.charges().low();
  const int chargeHigh = props.charges().high();
  const int bChg = baseCharge(chargeLow, chargeHigh);

  std::map<int, double> pKas{};
  std::map<int, double> mobilities{};
  for (int charge = chargeLow; charge <= chargeHigh; charge++) {
    mobilities.emplace(charge, props.mobility(charge));
    if (charge != bChg)
      pKas.emplace(charge, props.pKa(charge));
    else
      pKas.emplace(charge, 0);
  }
  assert(pKas.size() == mobilities.size());

  setupChargesModel(ui->qtblView_charges);
  m_chargesModel->refreshData(pKas, mobilities, chargeLow, chargeHigh);

  setupWidget();
}

void EditChargesWidget::initialize(PhysPropsMap pKas, const PhysPropsMap &mobilities, const int chargeLow, const int chargeHigh)
{
  assert(pKas.size() == mobilities.size() + 1);

  const int bChg = baseCharge(chargeLow, chargeHigh);
  if (pKas.size() - 1 == mobilities.size()) {
    if (pKas.find(bChg) == pKas.cend())
      pKas.emplace(bChg, 0);
    else
      assert(false);
  }
  assert(pKas.size() == mobilities.size());

  setupChargesModel(ui->qtblView_charges);
  m_chargesModel->refreshData(pKas, mobilities, chargeLow, chargeHigh);

  setupWidget();
}

std::vector<double> EditChargesWidget::mobilities() const
{
  std::vector<double> mobilities{};
  mobilities.reserve(static_cast<size_t>(m_chargesModel->rowCount()));

  for (int row = 0; row < m_chargesModel->rowCount(); row++)
    mobilities.emplace_back(m_chargesModel->data(m_chargesModel->index(row, ConstituentChargesModel::MOBILITY)).toReal());

  return mobilities;
}

std::vector<double> EditChargesWidget::pKas() const
{
  std::vector<double> pKas{};
  pKas.reserve(static_cast<size_t>(m_chargesModel->rowCount() - 1));

  if (m_chargesModel->rowCount() == 1)
    return pKas;

  for (int row = 0; row < m_chargesModel->rowCount(); row++) {
    const int charge = m_chargesModel->headerData(row, Qt::Vertical).toInt();
    if (m_chargesModel->isBaseCharge(charge))
      continue;

    pKas.emplace_back(m_chargesModel->data(m_chargesModel->index(row, ConstituentChargesModel::PKA)).toReal());
  }

  return pKas;
}

void EditChargesWidget::setupWidget()
{
  ui->qtblView_charges->setModel(m_chargesModel);

  m_qpb_addLow = new ModifyConstituentChargePushButton{ModifyConstituentChargePushButton::ChargeOperation::ADD, this};
  m_qpb_removeLow = new ModifyConstituentChargePushButton{ModifyConstituentChargePushButton::ChargeOperation::REMOVE, this};
  m_qpb_addHigh = new ModifyConstituentChargePushButton{ModifyConstituentChargePushButton::ChargeOperation::ADD, this};
  m_qpb_removeHigh = new ModifyConstituentChargePushButton{ModifyConstituentChargePushButton::ChargeOperation::REMOVE, this};

  connect(m_qpb_addLow, &QPushButton::clicked, this, &EditChargesWidget::onAddChargeLow);
  connect(m_qpb_removeLow, &QPushButton::clicked, this, &EditChargesWidget::onRemoveChargeLow);
  connect(m_qpb_addHigh, &QPushButton::clicked, this, &EditChargesWidget::onAddChargeHigh);
  connect(m_qpb_removeHigh, &QPushButton::clicked, this, &EditChargesWidget::onRemoveChargeHigh);
  connect(ui->qtblView_charges, &ReturnAcceptableTableView::returnPressed, this, &EditChargesWidget::onReturnPressed);

  ui->qhlay_lowChargeButtons->layout()->addWidget(m_qpb_addLow);
  ui->qhlay_lowChargeButtons->layout()->addWidget(m_qpb_removeLow);
  ui->qhlay_highChargeButtons->layout()->addWidget(m_qpb_addHigh);
  ui->qhlay_highChargeButtons->layout()->addWidget(m_qpb_removeHigh);

  updateChargeModifiers();
}

void EditChargesWidget::onAddChargeLow()
{
  if (m_chargesModel->insertRows(0, 1))
    updateChargeLow();
}

void EditChargesWidget::onAddChargeHigh()
{
  if (m_chargesModel->insertRows(m_chargesModel->rowCount(), 1))
    updateChargeHigh();
}

void EditChargesWidget::onRemoveChargeHigh()
{
  if (m_chargesModel->removeRows(m_chargesModel->rowCount() - 1, 1))
    updateChargeHigh();
}

void EditChargesWidget::onRemoveChargeLow()
{
  if (m_chargesModel->removeRows(0, 1))
    updateChargeLow();
}

void EditChargesWidget::onReturnPressed()
{
  emit acceptRequested();
}

void EditChargesWidget::setCharges(const std::map<int, double> &pKas, const std::map<int, double> &mobilities, const int chargeLow, const int chargeHigh)
{
  m_chargesModel->refreshData(pKas, mobilities, chargeLow, chargeHigh);
  updateChargeModifiers();
}

void EditChargesWidget::updateChargeHigh(const bool notify)
{
  const int chargeHigh = m_chargesModel->headerData(m_chargesModel->rowCount() - 1, Qt::Orientation::Vertical).toInt();

  m_qpb_addHigh->setText(chargeHigh + 1);
  m_qpb_removeHigh->setText(chargeHigh);

  if (notify)
    emit chargesChanged();
}

void EditChargesWidget::updateChargeLow(const bool notify)
{
  const int chargeLow = m_chargesModel->headerData(0, Qt::Orientation::Vertical).toInt();

  m_qpb_addLow->setText(chargeLow - 1);
  m_qpb_removeLow->setText(chargeLow);

  if (notify)
    emit chargesChanged();
}

void EditChargesWidget::updateChargeModifiers()
{
  updateChargeHigh(false);
  updateChargeLow(false);

  emit chargesChanged();
}

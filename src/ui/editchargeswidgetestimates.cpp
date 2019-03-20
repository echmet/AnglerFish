#include "editchargeswidgetestimates.h"

#include "internal_models/constituentchargesmodelfixable.h"

#include <gearbox/floatingvaluedelegate.h>
#include <gearbox/additionalfloatingvalidator.h>
#include <QTableView>
#include <cassert>

std::vector<bool> EditChargesWidgetEstimates::fixedMobilities() const
{
  std::vector<bool> fixed{};
  fixed.reserve(static_cast<size_t>(m_chargesModel->rowCount()));

  for (int row = 0; row < m_chargesModel->rowCount(); row++) {
    const int val = m_chargesModel->data(m_chargesModel->index(row, ConstituentChargesModelFixable::FIX_MOBILITY),
                                         Qt::CheckStateRole).toInt();
    fixed.emplace_back(val == Qt::Checked);
  }

  return fixed;
}

std::vector<bool> EditChargesWidgetEstimates::fixedpKas() const
{
  std::vector<bool> fixed{};
  fixed.reserve(static_cast<size_t>(m_chargesModel->rowCount()));

  for (int row = 0; row < m_chargesModel->rowCount(); row++) {
    const int charge = m_chargesModel->headerData(row, Qt::Vertical).toInt();
    if (m_chargesModel->isBaseCharge(charge))
      continue;

    int val = m_chargesModel->data(m_chargesModel->index(row, ConstituentChargesModelFixable::FIX_PKA),
                                   Qt::CheckStateRole).toInt();
    fixed.emplace_back(val == Qt::Checked);
  }

  return fixed;
}

void EditChargesWidgetEstimates::setCharges(const std::map<int, std::pair<double, bool>> &pKas,
                                            const std::map<int, std::pair<double, bool>> &mobilities,
                                            const int chargeLow, const int chargeHigh)
{
  auto model = qobject_cast<ConstituentChargesModelFixable *>(m_chargesModel);
  assert(model != nullptr);

  model->refreshData(pKas, mobilities, chargeLow, chargeHigh);

  updateChargeModifiers();
}

void EditChargesWidgetEstimates::setupChargesModel(QTableView *tbv)
{
  auto mustBePositiveAV = std::shared_ptr<gearbox::AdditionalFloatingValidator>{new gearbox::AdditionalFloatingValidator{[](const double d) { return d >= 0.0; }}};

  auto fltDelegate = new gearbox::FloatingValueDelegate{this};
  auto fltDelegatePos = new gearbox::FloatingValueDelegate{this};

  fltDelegatePos->setProperty(gearbox::AdditionalFloatingValidator::PROPERTY_NAME,
                              QVariant::fromValue<gearbox::AdditionalFloatingValidatorVec>({ mustBePositiveAV }));

  m_chargesModel = new ConstituentChargesModelFixable{this};

  tbv->setItemDelegateForColumn(0, fltDelegatePos);
  tbv->setItemDelegateForColumn(1, fltDelegate);

  connect(m_chargesModel, &QAbstractTableModel::dataChanged, this, [this]() { emit estimatesChanged(); });
  connect(this, &EditChargesWidget::chargesChanged, this, [this]() { emit estimatesChanged(); });
}

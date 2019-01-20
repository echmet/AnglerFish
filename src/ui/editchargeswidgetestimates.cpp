#include "editchargeswidgetestimates.h"

#include "internal_models/constituentchargesmodelfixable.h"

#include <gearbox/floatingvaluedelegate.h>
#include <QTableView>

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


void EditChargesWidgetEstimates::setupChargesModel(QTableView *tbv)
{
  m_fltDelegate = new FloatingValueDelegate{this};
  m_chargesModel = new ConstituentChargesModelFixable{this};

  tbv->setItemDelegateForColumn(0, m_fltDelegate);
  tbv->setItemDelegateForColumn(1, m_fltDelegate);
}

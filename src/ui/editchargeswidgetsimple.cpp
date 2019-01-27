#include "editchargeswidgetsimple.h"

#include "internal_models/constituentchargesmodel.h"

#include <gearbox/floatingvaluedelegate.h>
#include <QTableView>

void EditChargesWidgetSimple::setupChargesModel(QTableView *tbv)
{
  m_fltDelegate = new gearbox::FloatingValueDelegate{this};
  m_chargesModel = new ConstituentChargesModel{this};

  tbv->setItemDelegate(m_fltDelegate);
}

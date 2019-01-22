#include "ioniceffectsmodel.h"

IonicEffectsModel::IonicEffectsModel() :
  QObject{nullptr},
  m_debyeHuckel{true},
  m_onsagerFuoss{true}
{
}

bool IonicEffectsModel::debyeHuckel() const
{
  return m_debyeHuckel;
}

bool IonicEffectsModel::onsagerFuoss() const
{
  return m_onsagerFuoss;
}

void IonicEffectsModel::setDebyeHuckel(const bool v)
{
  m_debyeHuckel = v;

  emit changed();
}

void IonicEffectsModel::setOnsagerFuoss(const bool v)
{
  m_onsagerFuoss = v;

  emit changed();
}

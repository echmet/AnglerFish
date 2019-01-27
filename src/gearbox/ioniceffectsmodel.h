#ifndef IONICEFFECTSMODEL_H
#define IONICEFFECTSMODEL_H

#include <QObject>

namespace gearbox {

class IonicEffectsModel : public QObject {
  Q_OBJECT
public:
  IonicEffectsModel();

  bool debyeHuckel() const;
  bool onsagerFuoss() const;

  void setDebyeHuckel(const bool v);
  void setOnsagerFuoss(const bool v);

private:
  bool m_debyeHuckel;
  bool m_onsagerFuoss;

signals:
  void changed();
};

} // namespace gearbox

#endif // IONICEFFECTSMODEL_H

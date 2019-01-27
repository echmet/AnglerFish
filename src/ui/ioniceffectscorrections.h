#ifndef IONICEFFECTSCORRECTIONS_H
#define IONICEFFECTSCORRECTIONS_H

#include <QDialog>

namespace gearbox {
  class IonicEffectsModel;
} // namespace gearbox

namespace Ui {
  class IonicEffectsCorrections;
}

namespace gearbox {

class IonicEffectsCorrections : public QDialog {
  Q_OBJECT
public:
  explicit IonicEffectsCorrections(gearbox::IonicEffectsModel &ionEffs, QWidget *parent = nullptr);
  ~IonicEffectsCorrections();

private:
  Ui::IonicEffectsCorrections *ui;
  gearbox::IonicEffectsModel &h_ionEffs;
};

} // namespace gearbox

#endif // IONICEFFECTSCORRECTIONS_H

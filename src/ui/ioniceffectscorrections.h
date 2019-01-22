#ifndef IONICEFFECTSCORRECTIONS_H
#define IONICEFFECTSCORRECTIONS_H

#include <QDialog>

namespace Ui {
class IonicEffectsCorrections;
}

class IonicEffectsCorrections : public QDialog
{
  Q_OBJECT

public:
  explicit IonicEffectsCorrections(QWidget *parent = nullptr);
  ~IonicEffectsCorrections();

private:
  Ui::IonicEffectsCorrections *ui;
};

#endif // IONICEFFECTSCORRECTIONS_H

#ifndef MOBILITYCONSTRAINTSDIALOG_H
#define MOBILITYCONSTRAINTSDIALOG_H

#include <QDialog>

namespace Ui {
class MobilityConstraintsDialog;
}

namespace gearbox {
  class Gearbox;
  class FloatingValueDelegate;
  class LimitMobilityConstraintsModel;
} // namespace gearbox

class MobilityConstraintsModel;

class MobilityConstraintsDialog : public QDialog {
  Q_OBJECT
public:
  explicit MobilityConstraintsDialog(gearbox::LimitMobilityConstraintsModel &model,
                                     gearbox::Gearbox &gbox,
                                     QWidget *parent = nullptr);
  ~MobilityConstraintsDialog();

private:
  Ui::MobilityConstraintsDialog *ui;

  MobilityConstraintsModel *m_uiModel;
  gearbox::LimitMobilityConstraintsModel &h_model;
  gearbox::Gearbox &h_gbox;
  gearbox::FloatingValueDelegate *m_fltDelegate;
};

#endif // MOBILITYCONSTRAINTSDIALOG_H

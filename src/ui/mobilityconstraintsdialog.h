#ifndef MOBILITYCONSTRAINTSDIALOG_H
#define MOBILITYCONSTRAINTSDIALOG_H

#include <QDialog>

namespace Ui {
class MobilityConstraintsDialog;
}

namespace gearbox {
class LimitMobilityConstraintsModel;
} // namespace gearbox

class MobilityConstraintsModel;

class MobilityConstraintsDialog : public QDialog {
  Q_OBJECT
public:
  explicit MobilityConstraintsDialog(gearbox::LimitMobilityConstraintsModel &model, QWidget *parent = nullptr);
  ~MobilityConstraintsDialog();

private:
  Ui::MobilityConstraintsDialog *ui;

  MobilityConstraintsModel *m_uiModel;
  gearbox::LimitMobilityConstraintsModel &h_model;
};

#endif // MOBILITYCONSTRAINTSDIALOG_H

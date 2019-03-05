#ifndef MOBILITYCONSTRAINTSDIALOG_H
#define MOBILITYCONSTRAINTSDIALOG_H

#include <QDialog>

namespace Ui {
class MobilityConstraintsDialog;
}

namespace gearbox {
class Gearbox;
class LimitMobilityConstraintsModel;
} // namespace gearbox

class MobilityConstraintsModel;

class MobilityConstraintsDialog : public QDialog {
  Q_OBJECT
public:
  explicit MobilityConstraintsDialog(gearbox::LimitMobilityConstraintsModel &model,
                                     const gearbox::Gearbox &gbox,
                                     QWidget *parent = nullptr);
  ~MobilityConstraintsDialog();

private:
  Ui::MobilityConstraintsDialog *ui;

  MobilityConstraintsModel *m_uiModel;
  gearbox::LimitMobilityConstraintsModel &h_model;
  const gearbox::Gearbox &h_gbox;

public slots:
  void onEstimatesChanged();
};

#endif // MOBILITYCONSTRAINTSDIALOG_H

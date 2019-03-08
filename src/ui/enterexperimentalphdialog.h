#ifndef ENTEREXPERIMENTALPHDIALOG_H
#define ENTEREXPERIMENTALPHDIALOG_H

#include <QDialog>

namespace Ui {
class EnterExperimentalpHDialog;
}

class EnterExperimentalpHDialog : public QDialog {
  Q_OBJECT
public:
  explicit EnterExperimentalpHDialog(QWidget *parent = nullptr);
  ~EnterExperimentalpHDialog();

  double pH() const;

private:
  Ui::EnterExperimentalpHDialog *ui;

  double m_pH;
};

#endif // ENTEREXPERIMENTALPHDIALOG_H

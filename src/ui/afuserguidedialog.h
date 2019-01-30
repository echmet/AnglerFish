#ifndef AFUSERGUIDEDIALOG_H
#define AFUSERGUIDEDIALOG_H

#include <QDialog>

namespace Ui {
class AFUserGuideDialog;
}

class AFUserGuideDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AFUserGuideDialog(QWidget *parent = nullptr);
  ~AFUserGuideDialog();

private:
  Ui::AFUserGuideDialog *ui;
};

#endif // AFUSERGUIDEDIALOG_H

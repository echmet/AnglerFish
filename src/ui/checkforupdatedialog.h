#ifndef CHECKFORUPDATEDIALOG_H
#define CHECKFORUPDATEDIALOG_H

#include <QDialog>

class SoftwareUpdateResult;

namespace Ui {
class CheckForUpdateDialog;
}

class CheckForUpdateDialog : public QDialog
{
  Q_OBJECT
public:
  explicit CheckForUpdateDialog(QWidget *parent = nullptr);
  ~CheckForUpdateDialog();

protected:
  void closeEvent(QCloseEvent *ev);

private:
  Ui::CheckForUpdateDialog *ui;

signals:
  void checkForUpdate();
  void closed();

public slots:
  void onCheckComplete(const SoftwareUpdateResult &result);

private slots:
  void onCheckNowClicked();
  void onCheckOnStartupClicked();
  void onCloseClicked();

};

#endif // CHECKFORUPDATEDIALOG_H

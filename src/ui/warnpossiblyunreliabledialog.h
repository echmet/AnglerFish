#ifndef WARNPOSSIBLYUNRELIABLEDIALOG_H
#define WARNPOSSIBLYUNRELIABLEDIALOG_H

#include <QDialog>

namespace Ui {
class WarnPossiblyUnreliableDialog;
}

class WarnPossiblyUnreliableDialog : public QDialog
{
  Q_OBJECT

public:
  explicit WarnPossiblyUnreliableDialog(const int maxOkChargeAbs, QWidget *parent = nullptr);
  ~WarnPossiblyUnreliableDialog();
  bool dontShowAgain() const;

private:
  Ui::WarnPossiblyUnreliableDialog *ui;
};

#endif // WARNPOSSIBLYUNRELIABLEDIALOG_H

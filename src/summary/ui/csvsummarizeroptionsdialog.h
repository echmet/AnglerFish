#ifndef CSVSUMMARIZEROPTIONSDIALOG_H
#define CSVSUMMARIZEROPTIONSDIALOG_H

#include <QDialog>
#include <tuple>

namespace Ui {
class CSVSummarizerOptionsDialog;
}

class CSVSummarizerOptionsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CSVSummarizerOptionsDialog(QWidget *parent = nullptr);
  ~CSVSummarizerOptionsDialog();

  std::tuple<std::string,
             bool> options() const;
  void setOptions(const char delimiter, const bool experimentalpH);

private:
  Ui::CSVSummarizerOptionsDialog *ui;
};

#endif // CSVSUMMARIZEROPTIONSDIALOG_H

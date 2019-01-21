#ifndef EXPERIMENTALMOBILITYWIDGET_H
#define EXPERIMENTALMOBILITYWIDGET_H

#include <QWidget>

namespace Ui {
class ExperimentalMobilityWidget;
}

class ExperimentalMobilityWidget : public QWidget
{
  Q_OBJECT
public:
  explicit ExperimentalMobilityWidget(const int num, QWidget *parent = nullptr);
  ~ExperimentalMobilityWidget();

  void setNumber(const int num);
  void setValue(const double v);
  double value(bool &ok) const;

private:
  void setupIcons();

  Ui::ExperimentalMobilityWidget *ui;

signals:
  void dataChanged();
  void removeMe(ExperimentalMobilityWidget *me);
};

#endif // EXPERIMENTALMOBILITYWIDGET_H

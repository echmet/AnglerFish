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

  void connectOnScreenChanged();
  void setNumber(const int num);
  void setValue(const double v);
  double value(bool &ok) const;

private:
  void setWidgetSizes();
  void setupIcons();

  Ui::ExperimentalMobilityWidget *ui;

private slots:
  void onScreenChanged(QScreen *screen);

signals:
  void dataChanged();
  void removeMe(ExperimentalMobilityWidget *me);
};

#endif // EXPERIMENTALMOBILITYWIDGET_H

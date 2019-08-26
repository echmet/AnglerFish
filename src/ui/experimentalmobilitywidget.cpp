#include "experimentalmobilitywidget.h"
#include "ui_experimentalmobilitywidget.h"

#include <gearbox/doubletostringconvertor.h>
#include <QScreen>
#include <QTimer>
#include <QWindow>

inline
int removeBtnWidth(const QFontMetrics &fm)
{
  return qRound(fm.width("XX") * 2.3);
}

ExperimentalMobilityWidget::ExperimentalMobilityWidget(const int num, QWidget *parent) :
  QWidget{parent},
  ui{new Ui::ExperimentalMobilityWidget}
{
  ui->setupUi(this);

  ui->qle_value->setText("0");

  setNumber(num);

  setupIcons();

  connect(ui->qpb_remove, &QPushButton::clicked, this, [this]() { emit this->removeMe(this); });
  connect(ui->qle_value, &FloatingValueLineEdit::editingFinished, this, [this]() { emit this->dataChanged(); });

  setWidgetSizes();
}

ExperimentalMobilityWidget::~ExperimentalMobilityWidget()
{
  delete ui;
}

void ExperimentalMobilityWidget::connectOnScreenChanged()
{
  auto w = this->window();
  if (w == nullptr)
    return;

  auto wh = w->windowHandle();
  if (wh != nullptr)
    connect(wh, &QWindow::screenChanged, this, &ExperimentalMobilityWidget::onScreenChanged);
}

void ExperimentalMobilityWidget::onScreenChanged(QScreen *screen)
{
  if (screen == nullptr)
    return;

  setWidgetSizes();
}

void ExperimentalMobilityWidget::setNumber(const int num)
{
  ui->ql_num->setText(QString::number(num));
}

void ExperimentalMobilityWidget::setValue(const double v)
{
  ui->qle_value->setText(gearbox::DoubleToStringConvertor::convert(v));
}

void ExperimentalMobilityWidget::setupIcons()
{
#ifdef Q_OS_LINUX
  ui->qpb_remove->setIcon(QIcon::fromTheme("list-remove"));
#else
  ui->qpb_remove->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
#endif // Q_OS_
}

void ExperimentalMobilityWidget::setWidgetSizes()
{
  const int w = removeBtnWidth(fontMetrics());

  ui->qpb_remove->setMinimumWidth(w);
  ui->qpb_remove->setMaximumWidth(w);
}

double ExperimentalMobilityWidget::value(bool &ok) const
{
  return gearbox::DoubleToStringConvertor::back(ui->qle_value->text(), &ok);
}

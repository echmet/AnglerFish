#include "experimentalmobilitywidget.h"
#include "ui_experimentalmobilitywidget.h"
#include <gearbox/doubletostringconvertor.h>

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

  {
    const auto &fm = this->fontMetrics();
    const int w = qRound(fm.width("X") * 2.1);
    ui->qpb_remove->resize(w, ui->qpb_remove->height());
  }
}

ExperimentalMobilityWidget::~ExperimentalMobilityWidget()
{
  delete ui;
}

void ExperimentalMobilityWidget::setNumber(const int num)
{
  ui->ql_num->setText(QString::number(num));
}

void ExperimentalMobilityWidget::setupIcons()
{
#ifdef Q_OS_LINUX
  ui->qpb_remove->setIcon(QIcon::fromTheme("list-remove"));
#else
  ui->qpb_remove->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
#endif // Q_OS_
}

double ExperimentalMobilityWidget::value(bool &ok) const
{
  return DoubleToStringConvertor::back(ui->qle_value->text(), &ok);
}

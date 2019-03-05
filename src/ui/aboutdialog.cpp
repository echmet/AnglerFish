#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <globals.h>
#include <calculators/empfitterinterface.h>

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AboutDialog)
{
  ui->setupUi(this);

  ui->ql_echmetLink->setOpenExternalLinks(true);

  ui->ql_softwareName->setText(Globals::VERSION_STRING());
  ui->ql_echmetLink->setText(QString{"<a href=\"%1\">%1</a>"}.arg(Globals::ECHMET_WEB_LINK.toHtmlEscaped()));
  ui->ql_thankYouQt->setText(QString{"Based on Qt libraries. Version in use: %1 (%2 %3)"}
                                     .arg(qVersion(),
                                          QGuiApplication::platformName(),
                                          QSysInfo::buildCpuArchitecture()));

  m_echmetPix = QPixmap{":/images/res/ECHMET_logo_velke.png"}.scaledToWidth(qRound(100 * scaleFactor()), Qt::SmoothTransformation);
  ui->ql_echmetLogo->setText("");
  ui->ql_echmetLogo->setPixmap(m_echmetPix);

  auto afPix = QPixmap{":/images/res/AFicon_hires.png"}.scaledToWidth(qRound(48 * scaleFactor()), Qt::SmoothTransformation);
  ui->ql_logo->setPixmap(afPix);

  for (const auto &dev : Globals::DEVELOPERS) {
    const QString s = dev.linkString();

    QLabel *l = new QLabel(s, this);
    l->setAlignment(Qt::AlignHCenter);
    l->setOpenExternalLinks(true);
    ui->qvlay_authors->addWidget(l);
  }

  {
    QFontMetricsF fm(ui->ql_thankYouQt->font());
    setMinimumWidth(qRound(fm.boundingRect(ui->ql_thankYouQt->text()).width() * 1.15));
  }

  ui->ql_empfVersion->setText(QString{tr("ElmigParamsFitter library version: %1")}.arg(calculators::EMPFitterInterface::versionString()));

  connect(ui->qpb_close, &QPushButton::clicked, this, &AboutDialog::accept);
}

AboutDialog::~AboutDialog()
{
  delete ui;
}

qreal AboutDialog::scaleFactor()
{
  const auto fm = this->fontMetrics();

  const qreal sf = qRound(fm.height() / 16.0); /* "...and keep the SF-score low." */

  return sf;
}

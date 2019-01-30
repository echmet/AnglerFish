#include "afuserguidedialog.h"
#include "ui_afuserguidedialog.h"

#include <globals.h>

static const QString USER_GUIDE_LINK{"https://echmet.natur.cuni.cz/echmet/download/public/AnglerFish_Manual.pdf"};

AFUserGuideDialog::AFUserGuideDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AFUserGuideDialog)
{
  ui->setupUi(this);

  ui->ql_caption->setText(QString{tr("Please follow this link to download %1 user guide:")}.arg(Globals::SOFTWARE_NAME));

  ui->ql_userGuideLink->setOpenExternalLinks(true);
  ui->ql_userGuideLink->setText(QString{"<a href=\"%1\">%1</a>"}.arg(USER_GUIDE_LINK));
}

AFUserGuideDialog::~AFUserGuideDialog()
{
  delete ui;
}

#include "globals.h"
#include "ui/afmainwindow.h"
#include "gearbox/doubletostringconvertor.h"

#include <gearbox/gearbox.h>
#include <QApplication>

int main(int argc, char **argv)
{
  QApplication a{argc, argv};

  QCoreApplication::setOrganizationDomain(Globals::ORG_DOMAIN);
  QCoreApplication::setOrganizationName(Globals::ORG_NAME);
  QCoreApplication::setApplicationName(Globals::SOFTWARE_NAME);
  QCoreApplication::setApplicationVersion(Globals::VERSION_STRING());

  DoubleToStringConvertor::initialize();
  Gearbox::initialize();

  AFMainWindow mWin{};

  mWin.show();

  return a.exec();
}

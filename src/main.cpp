#include "globals.h"
#include "afcrashhandler.h"
#include "softwareupdater.h"

#include  <ui/afmainwindow.h>
#include <gearbox/doubletostringconvertor.h>
#include <gearbox/gearbox.h>
#include <QApplication>

void registerMetatypes();

int main(int argc, char **argv)
{
  QApplication a{argc, argv};
  SoftwareUpdater updater{};

  registerMetatypes();

  QCoreApplication::setOrganizationDomain(Globals::ORG_DOMAIN);
  QCoreApplication::setOrganizationName(Globals::ORG_NAME);
  QCoreApplication::setApplicationName(Globals::SOFTWARE_NAME);
  QCoreApplication::setApplicationVersion(Globals::VERSION_STRING());

  AFCrashHandler::installCrashHandler();

  DoubleToStringConvertor::initialize();
  Gearbox::initialize();

  AFCrashHandler::checkForCrash();

  AFMainWindow mWin{};

  mWin.connectUpdater(&updater);
  updater.checkAutomatically();

  mWin.show();

  int ret = a.exec();

  AFCrashHandler::uninstallCrashHandler();

  return ret;
}

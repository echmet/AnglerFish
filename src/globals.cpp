#include "globals.h"

#include <QPixmap>
#include <QSysInfo>

const QString Globals::ECHMET_WEB_LINK{"http://echmet.natur.cuni.cz"};
const QString Globals::ORG_DOMAIN{"echmet.natur.cuni.cz"};
const QString Globals::ORG_NAME{"ECHMET"};
const QString Globals::SOFTWARE_NAME{"AnglerFish"};
const int Globals::VERSION_MAJ{0};
const int Globals::VERSION_MIN{1};
const QString Globals::VERSION_REV{"f4"};

const QVector<Globals::DeveloperID> Globals::DEVELOPERS = {
                                                            Globals::DeveloperID{"Bob Ga\xC5\xA1", "gas@natur.cuni.cz", true},
                                                            Globals::DeveloperID{"Jana \xC5\xA0teflov\xC3\xA1", "svobod.j@natur.cuni.cz", false},
                                                            Globals::DeveloperID{"Vlastimil Hru\xC5\xA1ka", "hruska2@natur.cuni.cz", false},
                                                            Globals::DeveloperID{"Milan Boubl\xC3\xADk", "milan.boublik@natur.cuni.cz", false},
                                                            Globals::DeveloperID{"Pavel Dubsk\xC3\xBD", "pavel.dubsky@natur.cuni.cz", true},
                                                          };

QIcon Globals::icon()
{
#ifdef Q_OS_WIN
  static const QIcon PROGRAM_ICON{":/images/res/AFicon.ico"};
#else
  static const QIcon PROGRAM_ICON{":/images/res/AFicon_64.png"};
#endif // Q_OS_WIN

  return PROGRAM_ICON;
}

QString Globals::DeveloperID::linkString() const
{
  return QString{"%1 (<a href=\"mailto:%2\">%2</a>)"}.arg(name).arg(mail.toHtmlEscaped());
}

QString Globals::DeveloperID::prettyString() const
{
  return QString{"%1 (%2)"}.arg(name).arg(mail);
}

bool Globals::isWindowsXP()
{
#ifdef Q_OS_WIN
  static const QString WINDOWS{"windows"};
  static const QString XP{"xp"};

  const auto type = QSysInfo::productType();
  const auto ver = QSysInfo::productVersion();

  return type.toLower() == WINDOWS && ver.toLower() == XP;
#else
  return false;
#endif // Q_OS_WIN
}

QString Globals::VERSION_STRING()
{
  auto s = QString("%1 %2.%3%4").arg(SOFTWARE_NAME).arg(VERSION_MAJ).arg(VERSION_MIN).arg(VERSION_REV);
#ifdef UNSTABLE_VERSION
  s.append(" Build date: [" + QString(__DATE__) + " - " + QString(__TIME__)  + "]");
#endif // UNSTABLE_VERSION

  return s;
}

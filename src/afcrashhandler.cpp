#include "afcrashhandler.h"
#include "crashevent.h"
#include "crasheventcatcher.h"
#include "crashhandling/crashhandlingprovider.h"

#include <globals.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <QApplication>

#ifdef Q_OS_WIN
  #include "crashhandling/crashhandlerwindows.h"
  #define CrashHandlerPlatform CrashHandlerWindows
#elif defined (Q_OS_LINUX)
  #include "crashhandling/crashhandlerlinux.h"
  #define CrashHandlerPlatform CrashHandlerLinux
#else
  #include "crashhandling/nullcrashhandler.h"
  #define CrashHandlerPlatform NullCrashHandler
#endif // Q_OS_

inline
std::string mkSWStr(const std::string &tail)
{
  static const std::string swname = Globals::SOFTWARE_NAME.toStdString();

  return swname + "_" + tail;
}

class UICrashFinalizer : public CrashHandlerFinalizer<CrashHandlerPlatform>
{
public:
  explicit UICrashFinalizer(CrashHandlerPlatform *handler) :
    CrashHandlerFinalizer(handler)
  {}

  virtual void operator()() override
  {
    CrashEvent *evt = new CrashEvent(m_handler);

    if (m_handler->mainThreadCrashed()) {
      /* Let them know that I have been stiff
         and smiling 'till the crash */
      AFCrashHandler::s_catcher->executeEmergency();

      std::ofstream textDump(AFCrashHandler::s_textCrashDumpFile.c_str(), std::ios_base::out);
      if (textDump.is_open()) {
        textDump << m_handler->crashInfo();
        textDump.close();
      }

      /* And now make the final splash... */
      return;
    }

    qApp->postEvent(qApp, evt);

    m_handler->waitForKill();
  }
};

UICrashFinalizer * AFCrashHandler::s_uiFinalizer{nullptr};
CrashEventCatcher * AFCrashHandler::s_catcher{nullptr};
const std::string AFCrashHandler::s_textCrashDumpFile{mkSWStr("crashDump.txt")};

void AFCrashHandler::checkForCrash()
{
  std::ifstream textDump(s_textCrashDumpFile.c_str(), std::ios_base::in);
  if (!textDump.good())
    return;

  std::ostringstream textDumpSt;

  textDumpSt << textDump.rdbuf();

  CrashEventCatcher::displayCrashDialog(textDumpSt.str().c_str(), true);
  textDump.close();

  ::remove(s_textCrashDumpFile.c_str());
}

CrashHandlerBase * AFCrashHandler::handler()
{
  return CrashHandlingProvider<CrashHandlerPlatform>::handler();
}

bool AFCrashHandler::installCrashHandler()
{
#ifndef USE_CRASHHANDLER
  return true;
#else

  if (!CrashHandlingProvider<CrashHandlerPlatform>::initialize(mkSWStr("minidump.mdmp")))
    return false;

  try {
    s_uiFinalizer = new UICrashFinalizer(CrashHandlingProvider<CrashHandlerPlatform>::handler());
  } catch (std::bad_alloc &) {
    CrashHandlingProvider<CrashHandlerPlatform>::deinitialize();
    return false;
  }

  try {
    s_catcher = new CrashEventCatcher();
  } catch (std::bad_alloc &) {
    CrashHandlingProvider<CrashHandlerPlatform>::deinitialize();
    delete s_uiFinalizer;
    return false;
  }

  qApp->installEventFilter(s_catcher);

  return true;
#endif // USE_CRASHHANDLER
}

void AFCrashHandler::uninstallCrashHandler()
{
#ifndef USE_CRASHHANDLER
  return;
#else

  if (s_catcher != nullptr) {
    qApp->removeEventFilter(s_catcher);
    delete s_catcher;
  }

  CrashHandlingProvider<CrashHandlerPlatform>::deinitialize();

  delete s_uiFinalizer;
  s_uiFinalizer = nullptr;
  s_catcher = nullptr;
#endif // USE_CRASHHANDLER
}

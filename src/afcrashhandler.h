#ifndef AFCRASHHANDLER_H
#define AFCRASHHANDLER_H

#include "crasheventcatcher.h"
#include <type_traits>

class CrashHandlerBase;
class UICrashFinalizer;
class CrashEventCatcher;

class AFCrashHandler {
public:
  friend class UICrashFinalizer;
  AFCrashHandler() = delete;

  template <typename Receiver, void (Receiver::*EmergencySlot)()>
  static bool connectToEmergency(const Receiver *receiver)
  {
    static_assert(std::is_base_of<QObject, Receiver>::value, "Receiver must be a QObject");

    if (s_catcher == nullptr)
      return false;

    QObject::connect(s_catcher, &CrashEventCatcher::emergency, receiver, EmergencySlot, Qt::DirectConnection);

    return true;
  }

  static void checkForCrash();
  static bool installCrashHandler();
  static void uninstallCrashHandler();
  static CrashHandlerBase * handler();

private:
  static UICrashFinalizer *s_uiFinalizer;
  static CrashEventCatcher *s_catcher;

  static const std::string s_textCrashDumpFile;

};

#endif // AFCRASHHANDLER_H

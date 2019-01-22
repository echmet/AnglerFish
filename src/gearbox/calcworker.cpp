#include "calcworker.h"

#include <calculators/empfitterinterface.h>

void CalcWorker::process()
{
  failed = false;

  try {
    EMPFitterInterface iface{};

    iface.fit();
  } catch (const EMPFitterInterface::Exception &ex) {
    failed = true;
    error = ex.what();
  }

  emit finished();
}

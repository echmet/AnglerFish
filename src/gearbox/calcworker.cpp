#include "calcworker.h"

#include <calculators/empfitterinterface.h>

namespace gearbox {

CalcWorker::CalcWorker(gearbox::Gearbox &gbox) :
  h_gbox{gbox}
{
}

void CalcWorker::process()
{
  failed = false;

  try {
    calculators::EMPFitterInterface iface{h_gbox};

    iface.fit();
  } catch (const calculators::EMPFitterInterface::Exception &ex) {
    failed = true;
    error = ex.what();
  }

  emit finished();
}

} // namespace gearbox

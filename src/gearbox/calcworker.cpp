#include "calcworker.h"

namespace gearbox {

CalcWorker::CalcWorker(gearbox::Gearbox &gbox, std::vector<calculators::EMPFitterInterface::TracepointState> tpStates) :
  m_iface{gbox},
  m_tpStates{std::move(tpStates)}
{
}

CalcWorker::~CalcWorker()
{
  if (!failed)
    m_iface.propagateRSquared();
}

void CalcWorker::process()
{
  failed = false;

  try {
    m_iface.setTracepoints(m_tpStates);

    m_iface.fit();
  } catch (const calculators::EMPFitterInterface::Exception &ex) {
    failed = true;
    error = ex.what();
  }

  emit finished();
}

bool CalcWorker::writeTrace(const std::string &path)
{
  if (failed)
    return false;

  return m_iface.writeTrace(path);
}

} // namespace gearbox

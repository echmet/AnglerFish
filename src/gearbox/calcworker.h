#ifndef CALCWORKER_H
#define CALCWORKER_H

#include <calculators/empfitterinterface.h>
#include <QObject>

namespace gearbox {
  class Gearbox;

class CalcWorker : public QObject {
  Q_OBJECT
public:
  explicit CalcWorker(gearbox::Gearbox &gbox, std::vector<calculators::EMPFitterInterface::TracepointState> tpStates);
  ~CalcWorker();
  void process();
  bool writeTrace(const std::string &path);

  bool failed;
  QString error;

private:
  calculators::EMPFitterInterface m_iface;
  std::vector<calculators::EMPFitterInterface::TracepointState> m_tpStates;

signals:
  void finished();
};

} // namespace gearbox

#endif // CALCWORKER_H

#ifndef CALCWORKER_H
#define CALCWORKER_H

#include <QObject>

namespace gearbox {
  class Gearbox;

class CalcWorker : public QObject {
  Q_OBJECT
public:
  explicit CalcWorker(gearbox::Gearbox &gbox);
  void process();

  bool failed;
  QString error;

private:
  gearbox::Gearbox &h_gbox;

signals:
  void finished();
};

} // namespace gearbox

#endif // CALCWORKER_H

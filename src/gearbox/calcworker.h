#ifndef CALCWORKER_H
#define CALCWORKER_H

#include <QObject>

class CalcWorker : public QObject {
  Q_OBJECT
public:
  CalcWorker() = default;
  void process();

  bool failed;
  QString error;

signals:
  void finished();
};

#endif // CALCWORKER_H

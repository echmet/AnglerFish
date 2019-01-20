#ifndef EMPFITTERINTERFACE_H
#define EMPFITTERINTERFACE_H

#include <QPointF>
#include <QString>
#include <QVector>
#include <memory>
#include <stdexcept>

class EMPFitterInterface {
public:
  class Context;

  class Exception : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  explicit EMPFitterInterface();
  ~EMPFitterInterface();
  QVector<QPointF> expectedCurve();
  void fit();

private:
  void prepare();

  std::unique_ptr<Context> m_ctx;
  bool m_resultsAvailable;
};

#endif // EMPFITTERINTERFACE_H

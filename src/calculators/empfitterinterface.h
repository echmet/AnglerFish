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

  explicit EMPFitterInterface() = default;
  ~EMPFitterInterface() = default;
  void fit();
};

#endif // EMPFITTERINTERFACE_H

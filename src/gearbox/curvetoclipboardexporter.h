#ifndef CURVETOCLIPBOARDEXPORTER_H
#define CURVETOCLIPBOARDEXPORTER_H

#include <stdexcept>

class CurveToClipboardExporter {
public:
  class Exception : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  CurveToClipboardExporter() = delete;

  static void write();
};

#endif // CURVETOCLIPBOARDEXPORTER_H

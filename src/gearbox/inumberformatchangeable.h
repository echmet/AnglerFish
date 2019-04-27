#ifndef INUMBERFORMATCHANGEABLE_H
#define INUMBERFORMATCHANGEABLE_H

#include <globals.h>

#include <QtPlugin>

class QLocale;

namespace gearbox {

class INumberFormatChangeable {
public:
  virtual ~INumberFormatChangeable();
  virtual void onNumberFormatChanged(const QLocale *oldLocale) = 0;
};

} // namespace gearbox

Q_DECLARE_INTERFACE(gearbox::INumberFormatChangeable, "cz.cuni.natur.echmet." SOFTWARE_NAME_INTERNAL_S ".INumberFormatChangeable/1.0")

#endif // INUMBERFORMATCHANGEABLE_H

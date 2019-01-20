#ifndef INUMBERFORMATCHANGEABLE_H
#define INUMBERFORMATCHANGEABLE_H

#include <QtPlugin>

class QLocale;

class INumberFormatChangeable {
public:
  virtual ~INumberFormatChangeable();
  virtual void onNumberFormatChanged(const QLocale *oldLocale) = 0;
};

Q_DECLARE_INTERFACE(INumberFormatChangeable, "cz.cuni.natur.echmet.AnglerFish.INumberFormatChangeable/1.0")

#endif // INUMBERFORMATCHANGEABLE_H

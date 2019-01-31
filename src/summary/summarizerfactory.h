#ifndef SUMMARIZERFACTORY_H
#define SUMMARIZERFACTORY_H

#include "abstractsummarizer.h"

namespace summary {

using AbstractSummarizerPtr = std::shared_ptr<AbstractSummarizer>;

class SummarizerFactory {
public:
  enum class Types {
    CSV
  };

  SummarizerFactory() = delete;
  AbstractSummarizerPtr make(const Types type);
};

} // namespace summary

#endif // SUMMARIZERFACTORY_H

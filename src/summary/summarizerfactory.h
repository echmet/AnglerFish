#ifndef SUMMARIZERFACTORY_H
#define SUMMARIZERFACTORY_H

#include "abstractsummarizer.h"

#include <QMetaType>
#include <vector>

namespace summary {

using AbstractSummarizerPtr = std::shared_ptr<AbstractSummarizer>;

class Info {
public:
  Info(std::string _name, const int _id) :
    name{std::move(_name)},
    id{_id}
  {
  }

  std::string name;
  int id;
};

class SummarizerFactory {
public:
  enum Types : int {
    SUMMARIZER_CSV
  };

  SummarizerFactory() = delete;

  static std::vector<Info> list();
  static AbstractSummarizerPtr make(const Types type);
};

} // namespace summary

Q_DECLARE_METATYPE(summary::SummarizerFactory::Types)

#endif // SUMMARIZERFACTORY_H

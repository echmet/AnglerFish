#ifndef ABSTRACTSUMMARIZER_H
#define ABSTRACTSUMMARIZER_H

#include <memory>
#include <string>

namespace gearbox {
  class Gearbox;
} // namespace gearbox

namespace summary {

class CommonOptions;

class SpecificOptions {
public:
  virtual ~SpecificOptions() = 0;
};
using SpecificOptionsPtr = std::shared_ptr<SpecificOptions>;

class AbstractSummarizer {
public:
  explicit AbstractSummarizer();
  virtual ~AbstractSummarizer();
  virtual SpecificOptionsPtr options();
  virtual void summarize(const gearbox::Gearbox &gbox, const CommonOptions &common,
                         const SpecificOptionsPtr &specific, const std::string &output) = 0;
};

} // namespace summary

#endif // ABSTRACTSUMMARIZER_H

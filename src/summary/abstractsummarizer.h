#ifndef ABSTRACTSUMMARIZER_H
#define ABSTRACTSUMMARIZER_H

#include <memory>
#include <string>
#include <vector>

namespace gearbox {
  class Gearbox;
} // namespace gearbox

namespace summary {

class CommonOptions;

class FileType {
public:
  FileType(std::string _description, std::string _suffix);

  const std::string description;
  const std::string suffix;
};

class SpecificOptions {
public:
  virtual ~SpecificOptions() = 0;
};
using SpecificOptionsPtr = std::shared_ptr<SpecificOptions>;

class AbstractSummarizer {
public:
  explicit AbstractSummarizer();
  virtual ~AbstractSummarizer();
  virtual std::vector<FileType> allowedFileTypes() const;
  virtual std::string name() const = 0;
  virtual SpecificOptionsPtr options();
  virtual void summarize(const gearbox::Gearbox &gbox, const CommonOptions &common,
                         const SpecificOptionsPtr &specific, const std::string &output) = 0;
};

} // namespace summary

#endif // ABSTRACTSUMMARIZER_H

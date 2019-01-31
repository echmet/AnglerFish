#ifndef CSVSUMMARIZER_H
#define CSVSUMMARIZER_H

#include "abstractsummarizer.h"

namespace summary {

class CSVSummarizer : public AbstractSummarizer {
public:
  explicit CSVSummarizer() = default;
  virtual SpecificOptionsPtr options() override;
  virtual void summarize(const gearbox::Gearbox &gbox, const CommonOptions &common,
                         const SpecificOptionsPtr &specific, const std::string &output) override;

};

} // namespace summary

#endif // CSVSUMMARIZER_H

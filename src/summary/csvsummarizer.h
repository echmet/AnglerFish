#ifndef CSVSUMMARIZER_H
#define CSVSUMMARIZER_H

#include "abstractsummarizer.h"

class CSVSummarizerOptionsDialog;

namespace summary {

class CSVSummarizerOptions : public SpecificOptions {
public:
  CSVSummarizerOptions(const char _delimiter, const bool _experimentalpH);
  virtual ~CSVSummarizerOptions() override;

  const char delimiter;
  const bool experimentalpH;
};
using CSVSummarizerOptionsPtr = std::shared_ptr<CSVSummarizerOptions>;

class CSVSummarizer : public AbstractSummarizer {
public:
  explicit CSVSummarizer();
  virtual ~CSVSummarizer() override;
  virtual std::vector<FileType> allowedFileTypes() const override;
  virtual std::string name() const override;
  virtual SpecificOptionsPtr options() override;
  virtual void summarize(const gearbox::Gearbox &gbox, const CommonOptions &common,
                         const SpecificOptionsPtr &specific, const std::string &output) override;

private:
  CSVSummarizerOptionsDialog *m_optsDlg;
};

} // namespace summary

#endif // CSVSUMMARIZER_H

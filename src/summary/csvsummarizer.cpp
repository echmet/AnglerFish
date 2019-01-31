#include "csvsummarizer.h"

#include <trstr.h>

namespace summary {

std::string CSVSummarizer::name()
{
  return trstr("CSV summary");
}

SpecificOptionsPtr CSVSummarizer::options()
{
  return nullptr;
}

void CSVSummarizer::summarize(const gearbox::Gearbox &gbox, const CommonOptions &common,
                              const SpecificOptionsPtr &specific, const std::string &output)
{
  return;
}

} // namespace summary

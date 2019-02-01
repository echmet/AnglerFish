#include "abstractsummarizer.h"

namespace summary {

SpecificOptions::~SpecificOptions()
{
}

AbstractSummarizer::AbstractSummarizer()
{
}

AbstractSummarizer::~AbstractSummarizer()
{
}

SpecificOptionsPtr AbstractSummarizer::options()
{
  return nullptr;
}

} // namespace summary

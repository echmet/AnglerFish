#include "abstractsummarizer.h"

namespace summary {

FileType::FileType(std::string _description, std::string _suffix) :
  description{std::move(_description)},
  suffix{std::move(_suffix)}
{
}

SpecificOptions::~SpecificOptions()
{
}

AbstractSummarizer::AbstractSummarizer()
{
}

AbstractSummarizer::~AbstractSummarizer()
{
}

std::vector<FileType> AbstractSummarizer::allowedFileTypes() const
{
  return {};
}

SpecificOptionsPtr AbstractSummarizer::options()
{
  return nullptr;
}

} // namespace summary

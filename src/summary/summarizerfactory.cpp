#include "summarizerfactory.h"

#include "csvsummarizer.h"

namespace summary {

template <int ID>
void makeInfo(std::vector<Info> &vec)
{
  auto smr = SummarizerFactory::make(static_cast<SummarizerFactory::Types>(ID));

  vec.emplace_back(smr->name(), ID);
  makeInfo<ID - 1>(vec);
}

template <>
void makeInfo<0>(std::vector<Info> &vec)
{
  auto smr = SummarizerFactory::make(static_cast<SummarizerFactory::Types>(0));

  vec.emplace_back(smr->name(), 0);
}


std::vector<Info> SummarizerFactory::list()
{
  std::vector<Info> infoVec{};

  makeInfo<Types::SUMMARIZER_CSV>(infoVec);

  return infoVec;
}

AbstractSummarizerPtr SummarizerFactory::make(const Types type)
{
  switch (type) {
  case SUMMARIZER_CSV:
    return std::make_shared<CSVSummarizer>();
  }

  return nullptr;
}

} // namespace summary

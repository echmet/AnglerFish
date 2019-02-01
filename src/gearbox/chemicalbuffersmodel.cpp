#include "chemicalbuffersmodel.h"

#include <util_lowlevel.h>
#include <algorithm>
#include <cassert>

namespace gearbox {

inline
bool operator<(const ChemicalBuffer &lhs, const ChemicalBuffer &rhs)
{
  return lhs.pH() < rhs.pH();
}

ChemicalBuffersModel::ChemicalBuffersModel() :
  QObject{nullptr}
{
}

ChemicalBuffersModel::ChemicalBuffersModel(const ChemicalBuffersModel &other) :
  QObject{nullptr}
{
  for (const auto &b : other.m_buffers)
    m_buffers.emplace_back(b);
}

void ChemicalBuffersModel::add(ChemicalBuffer buffer)
{
  m_buffers.emplace_back(std::move(buffer));

  emit bufferAdded(m_buffers.back());
}

ChemicalBuffer & ChemicalBuffersModel::at(const size_t idx)
{
  assert(idx >= 0 && idx < m_buffers.size());

  auto it = m_buffers.begin();
  std::advance(it, idx);

  return *it;
}

ChemicalBuffersModel::iterator ChemicalBuffersModel::begin()
{
  return m_buffers.begin();
}

ChemicalBuffersModel::const_iterator ChemicalBuffersModel::begin() const
{
  return m_buffers.cbegin();
}

ChemicalBuffersModel::const_iterator ChemicalBuffersModel::cbegin() const
{
  return m_buffers.cbegin();
}

void ChemicalBuffersModel::clear()
{
  setBuffers({});
}

ChemicalBuffersModel::iterator ChemicalBuffersModel::end()
{
  return m_buffers.end();
}

ChemicalBuffersModel::const_iterator ChemicalBuffersModel::end() const
{
  return m_buffers.cend();
}

ChemicalBuffersModel::const_iterator ChemicalBuffersModel::cend() const
{
  return m_buffers.cend();
}

void ChemicalBuffersModel::remove(const ChemicalBuffer &buffer) noexcept
{
  for (auto it = m_buffers.cbegin(); it != m_buffers.cend(); ++it) {
    if (&(*it) == &buffer) {
      m_buffers.erase(it);
      return;
    }
  }

  IMPOSSIBLE_PATH;
}

void ChemicalBuffersModel::removeAt(const size_t idx) noexcept
{
  assert(idx >= 0 && idx < m_buffers.size());

  auto it = m_buffers.cbegin();
  std::advance(it, idx);
  m_buffers.erase(it);

  emit bufferRemoved(idx);
}

void ChemicalBuffersModel:: setBuffers(std::vector<ChemicalBuffer> buffers)
{
  emit beginModelReset();

  m_buffers.clear();

  for (auto &&buf : buffers)
    m_buffers.emplace_back(std::move(buf));

  emit endModelReset();
}

void ChemicalBuffersModel::sortBypH()
{
  emit beginModelReset();

  std::vector<ChemicalBuffer> bufs{};
  bufs.reserve(m_buffers.size());

  for (auto &&b : m_buffers)
    bufs.emplace_back(std::move(b));

  std::sort(bufs.begin(), bufs.end());

  m_buffers.clear();

  for (auto &&b : bufs)
    m_buffers.emplace_back(std::move(b));

  emit endModelReset();
}

ChemicalBuffersModel & ChemicalBuffersModel::operator=(const ChemicalBuffersModel &other)
{
  for (const auto &b : other.m_buffers)
    m_buffers.emplace_back(b);

  return *this;
}

} // namespace gearbox

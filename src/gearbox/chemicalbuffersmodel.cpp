#include "chemicalbuffersmodel.h"

#include <cassert>
#include <util_lowlevel.h>

ChemicalBuffersModel::ChemicalBuffersModel()
{
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

ChemicalBuffersModel::const_iterator ChemicalBuffersModel::begin() const
{
  return m_buffers.cbegin();
}

ChemicalBuffersModel::const_iterator ChemicalBuffersModel::end() const
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

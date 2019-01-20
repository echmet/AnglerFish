#ifndef CHEMICALBUFFERSMODEL_H
#define CHEMICALBUFFERSMODEL_H

#include "chemicalbuffer.h"

#include <QObject>
#include <list>

class ChemicalBuffersModel : public QObject {
  Q_OBJECT
public:
  using const_iterator = std::list<ChemicalBuffer>::const_iterator;

  ChemicalBuffersModel();

  void add(ChemicalBuffer buffer);
  ChemicalBuffer & at(const size_t idx);
  const_iterator begin() const;
  const_iterator end() const;
  void remove(const ChemicalBuffer &buffer) noexcept;
  void removeAt(const size_t idx) noexcept;

private:
  std::list<ChemicalBuffer> m_buffers;

signals:
  void bufferAdded(ChemicalBuffer &buffer);
  void bufferRemoved(size_t idx);
};

#endif // CHEMICALBUFFERSMODEL_H

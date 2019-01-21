#ifndef CHEMICALBUFFERSMODEL_H
#define CHEMICALBUFFERSMODEL_H

#include "chemicalbuffer.h"

#include <QObject>
#include <list>

class ChemicalBuffersModel : public QObject {
  Q_OBJECT
public:
  using iterator = std::list<ChemicalBuffer>::iterator;
  using const_iterator = std::list<ChemicalBuffer>::const_iterator;

  explicit ChemicalBuffersModel();

  void add(ChemicalBuffer buffer);
  ChemicalBuffer & at(const size_t idx);
  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  iterator end();
  const_iterator end() const;
  void remove(const ChemicalBuffer &buffer) noexcept;
  void removeAt(const size_t idx) noexcept;
  void setBuffers(std::vector<ChemicalBuffer> buffers);

private:
  std::list<ChemicalBuffer> m_buffers;

signals:
  void bufferAdded(ChemicalBuffer &buffer);
  void bufferRemoved(size_t idx);
  void beginModelReset();
  void endModelReset();
};

#endif // CHEMICALBUFFERSMODEL_H

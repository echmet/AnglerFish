#ifndef BUFFERWIDGET_H
#define BUFFERWIDGET_H

#include <QVector>
#include <QWidget>

namespace Ui {
class BufferWidget;
}

class BufferCompositionWidget;
class ChemicalBuffer;
class ExperimentalMobilityWidget;
class QVBoxLayout;

class BufferWidget : public QWidget {
  Q_OBJECT
public:
  explicit BufferWidget(ChemicalBuffer &buffer, QWidget *parent = nullptr);
  ~BufferWidget();

  const ChemicalBuffer & buffer() const noexcept;

private:
  void setupIcons();

  Ui::BufferWidget *ui;

  BufferCompositionWidget *m_compositionWidget;
  QWidget *m_expValuesScrollWidget;
  QVBoxLayout *m_expValuesScrollLayout;

  QVector<ExperimentalMobilityWidget *> m_expValueWidgets;

  ChemicalBuffer &h_buffer;

private slots:
  void onAddExpValue();
  void onCompositionChanged();
  void onRemoveExpValue(ExperimentalMobilityWidget *w);
  void updateExperimentalMobilities();

signals:
  void cloneMe(const BufferWidget *me);
  void bufferChanged(const BufferWidget *me);
  void removeMe(BufferWidget *me);
};

#endif // BUFFERWIDGET_H

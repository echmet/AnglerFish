#ifndef BUFFERWIDGET_H
#define BUFFERWIDGET_H

#include <QVector>
#include <QWidget>

namespace gearbox {
  class Gearbox;
  class ChemicalBuffer;
} // namespace gearbox

namespace Ui {
  class BufferWidget;
} // namespace Ui

class BufferCompositionWidget;
class ExperimentalMobilityWidget;
class QVBoxLayout;

class BufferWidget : public QWidget {
  Q_OBJECT
public:
  explicit BufferWidget(gearbox::Gearbox &gbox, gearbox::ChemicalBuffer &buffer, QWidget *parent = nullptr);
  ~BufferWidget();

  const gearbox::ChemicalBuffer & buffer() const noexcept;

private:
  void setWidgetSizes();
  void setupIcons();

  Ui::BufferWidget *ui;
  gearbox::Gearbox &h_gbox;

  BufferCompositionWidget *m_compositionWidget;
  QWidget *m_expValuesScrollWidget;
  QVBoxLayout *m_expValuesScrollLayout;

  QVector<ExperimentalMobilityWidget *> m_expValueWidgets;

  gearbox::ChemicalBuffer &h_buffer;

private slots:
  void onAddExpValue();
  void onCompositionChanged();
  void onIonicEffectsChanged();
  void onRemoveExpValue(ExperimentalMobilityWidget *w);
  void setExpValues();
  void updateExperimentalMobilities();

signals:
  void cloneMe(const BufferWidget *me);
  void bufferChanged(const BufferWidget *me);
  void exportMe(const BufferWidget *me);
  void removeMe(BufferWidget *me);
};

#endif // BUFFERWIDGET_H

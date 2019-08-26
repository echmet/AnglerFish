#ifndef BUFFERSINPUTWIDGET_H
#define BUFFERSINPUTWIDGET_H

#include <QFileDialog>
#include <QWidget>

namespace gearbox {
  class Gearbox;
  class ChemicalBuffer;
} // namespace gearbox

namespace Ui {
  class BuffersInputWidget;
} // namespace Ui

class BufferWidget;
class ChemicalBuffer;
class QMenu;
class QVBoxLayout;

class BuffersInputWidget : public QWidget {
  Q_OBJECT
public:
  explicit BuffersInputWidget(gearbox::Gearbox &gbox, QWidget *parent = nullptr);
  ~BuffersInputWidget();

  void connectOnScreenChanged();

private:
  void handleAddedBuffer(gearbox::ChemicalBuffer &buffer);
  void setWidgetSizes();
  void setupIcons();

  Ui::BuffersInputWidget *ui;
  gearbox::Gearbox &h_gbox;

  QWidget *m_scrollWidget;
  QVBoxLayout *m_scrollLayout;

  QFileDialog m_saveBufferDlg;

  QMenu *m_loadBufferMenu;

private slots:
  void onBufferChanged(const BufferWidget *w);
  void onCloneBuffer(const BufferWidget *w);
  void onExportBuffer(const BufferWidget *w, const bool toClipboard);
  void onLoadBufferFromClipboard();
  void onLoadBufferFromFile();
  void onScreenChanged(QScreen *scr);
  void onScrollToBottom(const int min, const int max);
  void onSortBypH();

public slots:
  void onBeginBuffersReset();
  void onBufferAdded(gearbox::ChemicalBuffer &buffer);
  void onEndBuffersReset();
  void onRemoveBuffer(BufferWidget *w);

signals:
  void addBuffer();
  void addBuffer(const gearbox::ChemicalBuffer &buffer);
  void removeBuffer(const gearbox::ChemicalBuffer &buffer);
  void buffersChanged();
};

#endif // BUFFERSINPUTWIDGET_H

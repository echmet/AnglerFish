#ifndef BUFFERSINPUTWIDGET_H
#define BUFFERSINPUTWIDGET_H

#include <QFileDialog>
#include <QWidget>

namespace Ui {
class BuffersInputWidget;
}

class BufferWidget;
class ChemicalBuffer;
class QVBoxLayout;

class BuffersInputWidget : public QWidget {
  Q_OBJECT
public:
  explicit BuffersInputWidget(QWidget *parent = nullptr);
  ~BuffersInputWidget();

private:
  void setupIcons();

  Ui::BuffersInputWidget *ui;

  QWidget *m_scrollWidget;
  QVBoxLayout *m_scrollLayout;

  QFileDialog m_loadBufferDlg;
  QFileDialog m_saveBufferDlg;

private slots:
  void onBufferChanged(const BufferWidget *w);
  void onCloneBuffer(const BufferWidget *w);
  void onExportBuffer(const BufferWidget *w);
  void onLoadBuffer();
  void onScrollToBottom(const int min, const int max);

public slots:
  void onBufferAdded(ChemicalBuffer &buffer);
  void onRemoveBuffer(BufferWidget *w);

signals:
  void addBuffer();
  void addBuffer(const ChemicalBuffer &buffer);
  void removeBuffer(const ChemicalBuffer &buffer);
  void buffersChanged();
};

#endif // BUFFERSINPUTWIDGET_H

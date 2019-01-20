#ifndef BUFFERCOMPOSITIONWIDGET_H
#define BUFFERCOMPOSITIONWIDGET_H

#include <QWidget>

namespace Ui {
class BufferCompositionWidget;
}

class BufferCompositionModel;
class DatabaseProxy;
class EditConstituentDialog;
class FloatingValueDelegate;
class GDMProxy;

class BufferCompositionWidget : public QWidget {
  Q_OBJECT
public:
  explicit BufferCompositionWidget(GDMProxy &gdmProxy, DatabaseProxy &dbProxy, QWidget *parent = nullptr);
  ~BufferCompositionWidget();

private:
  void editConstituent(const QString &name);
  void setupIcons();

  Ui::BufferCompositionWidget *ui;

  FloatingValueDelegate *m_fltDelegate;
  BufferCompositionModel *m_model;
  GDMProxy &h_gdmProxy;
  DatabaseProxy &h_dbProxy;

private slots:
  void onAddConstituent();
  void onAddToDatabase(const EditConstituentDialog *dlg);
  void onRemoveConstituent();

signals:
  void compositionChanged();
};

#endif // BUFFERCOMPOSITIONWIDGET_H

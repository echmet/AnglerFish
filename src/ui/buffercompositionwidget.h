#ifndef BUFFERCOMPOSITIONWIDGET_H
#define BUFFERCOMPOSITIONWIDGET_H

#include <QWidget>

namespace gearbox {
  class DatabaseProxy;
  class FloatingValueDelegate;
  class GDMProxy;
} // namespace gearbox

namespace Ui {
  class BufferCompositionWidget;
}

class BufferCompositionModel;
class EditConstituentDialog;

class BufferCompositionWidget : public QWidget {
  Q_OBJECT
public:
  explicit BufferCompositionWidget(gearbox::GDMProxy &gdmProxy,
                                   gearbox::DatabaseProxy &dbProxy,
                                   QWidget *parent = nullptr);
  ~BufferCompositionWidget();

private:
  void editConstituent(const QString &name);
  void setupIcons();
  void setWidgetSizes();

  Ui::BufferCompositionWidget *ui;

  gearbox::FloatingValueDelegate *m_fltDelegate;
  BufferCompositionModel *m_model;
  gearbox::GDMProxy &h_gdmProxy;
  gearbox::DatabaseProxy &h_dbProxy;

private slots:
  void onAddConstituent();
  void onAddToDatabase(const EditConstituentDialog *dlg);
  void onRemoveConstituent();

signals:
  void compositionChanged();
};

#endif // BUFFERCOMPOSITIONWIDGET_H

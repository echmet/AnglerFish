#include "buffersinputwidget.h"
#include "ui_buffersinputwidget.h"

#include "bufferwidget.h"

#include <gearbox/gearbox.h>
#include <persistence/persistence.h>
#include <QMessageBox>
#include <QScrollBar>
#include <QVBoxLayout>
#include <cassert>

BuffersInputWidget::BuffersInputWidget(QWidget *parent) :
  QWidget{parent},
  ui{new Ui::BuffersInputWidget},
  m_loadBufferDlg{this, tr("Load buffer from PeakMaster 6"), {}, "PeakMaster 6 JSON file (*.json)"},
  m_saveBufferDlg{this, tr("Save buffer as PeakMaster 6 file"), {}, "PeakMaster 6 JSON file (*.json)"}
{
  ui->setupUi(this);

  m_scrollWidget = new QWidget{};
  m_scrollWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
  m_scrollLayout = new QVBoxLayout{};
  m_scrollWidget->setLayout(m_scrollLayout);

  ui->qscr_bufferList->setWidget(m_scrollWidget);

  m_scrollLayout->addStretch();

  setupIcons();

  m_loadBufferDlg.setAcceptMode(QFileDialog::AcceptOpen);
  m_saveBufferDlg.setAcceptMode(QFileDialog::AcceptSave);

  connect(ui->qpb_addBuffer, &QPushButton::clicked, this, [this]() { emit this->addBuffer(); });
  connect(ui->qpb_loadBuffer, &QPushButton::clicked, this, &BuffersInputWidget::onLoadBuffer);
}

BuffersInputWidget::~BuffersInputWidget()
{
  delete ui;
}

void BuffersInputWidget::onBufferAdded(ChemicalBuffer &buffer)
{
  auto w = new BufferWidget{buffer};
  auto idx = m_scrollLayout->count() - 1;
  assert(idx >= 0);

  connect(ui->qscr_bufferList->verticalScrollBar(), &QScrollBar::rangeChanged, this, &BuffersInputWidget::onScrollToBottom);

  m_scrollLayout->insertWidget(idx, w);

  connect(w, &BufferWidget::removeMe, this, &BuffersInputWidget::onRemoveBuffer);
  connect(w, &BufferWidget::cloneMe, this, &BuffersInputWidget::onCloneBuffer);
  connect(w, &BufferWidget::bufferChanged, this, &BuffersInputWidget::onBufferChanged);
  connect(w, &BufferWidget::exportMe, this, &BuffersInputWidget::onExportBuffer);

  emit buffersChanged();
}

void BuffersInputWidget::onBeginBuffersReset()
{
  while (auto *w = m_scrollLayout->takeAt(0))
    delete w;

   /* Yeah, this is a little hacky... */
  m_scrollLayout->addStretch();
}

void BuffersInputWidget::onBufferChanged(const BufferWidget *)
{
  emit buffersChanged();
}

void BuffersInputWidget::onCloneBuffer(const BufferWidget *w)
{
  ChemicalBuffer buf = w->buffer();
  buf.setExperimentalMobilities({});

  emit addBuffer(std::move(buf));
}

void BuffersInputWidget::onEndBuffersReset()
{
  auto gbox = Gearbox::instance();

  for (auto &buf : gbox->chemicalBuffersModel())
    onBufferAdded(buf);
}

void BuffersInputWidget::onExportBuffer(const BufferWidget *w)
{
  if (m_saveBufferDlg.exec() == QDialog::Accepted) {
    if (m_saveBufferDlg.selectedFiles().empty())
      return;

    try {
      persistence::savePeakMasterBuffer(m_saveBufferDlg.selectedFiles().first(), w->buffer());
    } catch (const persistence::Exception &ex) {
      QMessageBox mbox{QMessageBox::Warning, tr("Cannot save buffer"), ex.what()};
      mbox.exec();
    }
  }
}

void BuffersInputWidget::onLoadBuffer()
{
  if (m_loadBufferDlg.exec() == QDialog::Accepted) {
    if (m_loadBufferDlg.selectedFiles().empty())
      return;

    try {
      persistence::loadPeakMasterBuffer(m_loadBufferDlg.selectedFiles().first());
    } catch (const persistence::Exception &ex) {
      QMessageBox mbox{QMessageBox::Warning, tr("Cannot load buffer"), ex.what()};
      mbox.exec();
    }
  }
}

void BuffersInputWidget::onRemoveBuffer(BufferWidget *w)
{
  assert(w != nullptr);

  QMessageBox mbox{QMessageBox::Question, tr("Confirm action"), tr("Remove buffer?"),
                   QMessageBox::Yes | QMessageBox::No};
  if (mbox.exec() != QMessageBox::Yes)
    return;

  emit removeBuffer(w->buffer());

  m_scrollLayout->removeWidget(w);

  delete w;

  emit buffersChanged();
}

void BuffersInputWidget::onScrollToBottom(const int min, const int max)
{
  (void)min;

  ui->qscr_bufferList->verticalScrollBar()->setValue(max);

  disconnect(ui->qscr_bufferList->verticalScrollBar(), &QScrollBar::rangeChanged, this, &BuffersInputWidget::onScrollToBottom);
}

void BuffersInputWidget::setupIcons()
{
#ifdef Q_OS_LINUX
  ui->qpb_addBuffer->setIcon(QIcon::fromTheme("list-add"));
  ui->qpb_loadBuffer->setIcon(QIcon::fromTheme("document-open"));
#else
#endif // Q_OS_
}

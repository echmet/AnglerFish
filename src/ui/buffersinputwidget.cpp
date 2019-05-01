#include "buffersinputwidget.h"
#include "ui_buffersinputwidget.h"

#include "bufferwidget.h"

#include <gearbox/gearbox.h>
#include <gearbox/chemicalbuffersmodel.h>
#include <persistence/persistence.h>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QScrollBar>
#include <QVBoxLayout>
#include <cassert>


inline
bool cmpBufferWidgets(BufferWidget *lhs, BufferWidget *rhs)
{
  return lhs->buffer().pH() < rhs->buffer().pH();
}

BuffersInputWidget::BuffersInputWidget(gearbox::Gearbox &gbox, QWidget *parent) :
  QWidget{parent},
  ui{new Ui::BuffersInputWidget},
  h_gbox{gbox},
  m_saveBufferDlg{this, tr("Save buffer as PeakMaster 6 file"), {}, "PeakMaster 6 JSON file (*.json)"}
{
  ui->setupUi(this);

  m_scrollWidget = new QWidget{};
  m_scrollWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
  m_scrollLayout = new QVBoxLayout{};
  m_scrollWidget->setLayout(m_scrollLayout);

  ui->qscr_bufferList->setWidget(m_scrollWidget);

  m_scrollLayout->addStretch();

  m_loadBufferMenu = new QMenu{this};
  auto actionLoadBufferFromFile = new QAction{tr("From file"), this};
  auto actionLoadBufferFromClipboard = new QAction{tr("From clipboard"), this};
  m_loadBufferMenu->addAction(actionLoadBufferFromFile);
  m_loadBufferMenu->addAction(actionLoadBufferFromClipboard);
  ui->qpb_loadBuffer->setMenu(m_loadBufferMenu);

  setupIcons();

  m_saveBufferDlg.setAcceptMode(QFileDialog::AcceptSave);

  connect(ui->qpb_addBuffer, &QPushButton::clicked, this, [this]() { emit this->addBuffer(); });
  connect(ui->qpb_sortBypH, &QPushButton::clicked, this, &BuffersInputWidget::onSortBypH);
  connect(actionLoadBufferFromFile, &QAction::triggered, this, &BuffersInputWidget::onLoadBufferFromFile);
  connect(actionLoadBufferFromClipboard, &QAction::triggered, this, &BuffersInputWidget::onLoadBufferFromClipboard);
}

BuffersInputWidget::~BuffersInputWidget()
{
  delete ui;
}

void BuffersInputWidget::handleAddedBuffer(gearbox::ChemicalBuffer &buffer)
{
  auto w = new BufferWidget{h_gbox, buffer};
  auto idx = m_scrollLayout->count() - 1;
  assert(idx >= 0);

  connect(ui->qscr_bufferList->verticalScrollBar(), &QScrollBar::rangeChanged, this, &BuffersInputWidget::onScrollToBottom);

  m_scrollLayout->insertWidget(idx, w);

  connect(w, &BufferWidget::removeMe, this, &BuffersInputWidget::onRemoveBuffer);
  connect(w, &BufferWidget::cloneMe, this, &BuffersInputWidget::onCloneBuffer);
  connect(w, &BufferWidget::bufferChanged, this, &BuffersInputWidget::onBufferChanged);
  connect(w, &BufferWidget::exportMe, this, &BuffersInputWidget::onExportBuffer);
}

void BuffersInputWidget::onBufferAdded(gearbox::ChemicalBuffer &buffer)
{
  handleAddedBuffer(buffer);

  emit buffersChanged();
}

void BuffersInputWidget::onBeginBuffersReset()
{
  QLayoutItem *item;
  while ((item = m_scrollLayout->takeAt(0)) != nullptr) {
    QWidget *w = item->widget();
    delete w;
    delete item;
  }
  assert(m_scrollLayout->count() == 0);

   /* Yeah, this is a little hacky... */
  m_scrollLayout->addStretch();
}

void BuffersInputWidget::onBufferChanged(const BufferWidget *)
{
  emit buffersChanged();
}

void BuffersInputWidget::onCloneBuffer(const BufferWidget *w)
{
  gearbox::ChemicalBuffer buf = w->buffer();
  buf.setExperimentalMobilities({});

  emit addBuffer(std::move(buf));
}

void BuffersInputWidget::onEndBuffersReset()
{
  for (auto &buf : h_gbox.chemicalBuffersModel())
    handleAddedBuffer(buf);

  onSortBypH();

  emit buffersChanged();
}

void BuffersInputWidget::onExportBuffer(const BufferWidget *w, const bool toClipboard)
{
  QString path{};

  if (!toClipboard) {
    if (m_saveBufferDlg.exec() != QDialog::Accepted)
      return;

    if (m_saveBufferDlg.selectedFiles().empty())
      return;

    path = m_saveBufferDlg.selectedFiles().constFirst();
  }

  try {
    const auto ttype  = toClipboard ? persistence::Target::TT_CLIPBOARD : persistence::Target::TT_FILE;
    persistence::Target tgt{ttype, path};

    persistence::savePeakMasterBuffer(tgt, w->buffer());
  } catch (const persistence::Exception &ex) {
    QMessageBox mbox{QMessageBox::Warning, tr("Cannot save buffer"), ex.what()};
    mbox.exec();
  }
}

void BuffersInputWidget::onLoadBufferFromClipboard()
{
  try {
    persistence::loadPeakMasterBuffer({persistence::Target::TT_CLIPBOARD, ""}, h_gbox);
  } catch (const persistence::Exception &ex) {
    QMessageBox mbox{QMessageBox::Warning, tr("Cannot load buffer"),
                     QString{"Failed to load buffer from clipboard:\n%1"}.arg(ex.what())};
    mbox.exec();
  }
}

void BuffersInputWidget::onLoadBufferFromFile()
{
  static QString lastPath{};

  QFileDialog dlg{this, tr("Load buffer from PeakMaster 6"), {}, "PeakMaster 6 JSON file (*.json)"};
  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setFileMode(QFileDialog::ExistingFiles);

  if (!lastPath.isEmpty())
    dlg.setDirectory(QFileInfo{lastPath}.absoluteDir());

  if (dlg.exec() == QDialog::Accepted) {
    const auto &selectedFiles = dlg.selectedFiles();
    for (const auto &path : selectedFiles) {
      try {
        persistence::loadPeakMasterBuffer({persistence::Target::TT_FILE, path}, h_gbox);
        lastPath = path;
      } catch (const persistence::Exception &ex) {
        QMessageBox mbox{QMessageBox::Warning, tr("Cannot load buffer"),
                         QString{"Failed to load file %1:\n%2"}.arg(QFileInfo{path}.fileName(), ex.what())};
        mbox.exec();
      }
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

  m_scrollLayout->removeWidget(w);

  emit removeBuffer(w->buffer());

  delete w;

  emit buffersChanged();
}

void BuffersInputWidget::onScrollToBottom(const int min, const int max)
{
  (void)min;

  ui->qscr_bufferList->verticalScrollBar()->setValue(max);

  disconnect(ui->qscr_bufferList->verticalScrollBar(), &QScrollBar::rangeChanged, this, &BuffersInputWidget::onScrollToBottom);
}

void BuffersInputWidget::onSortBypH()
{
  std::vector<BufferWidget *> wBufs{};

  while (m_scrollLayout->count() > 1) {
    QLayoutItem *item = m_scrollLayout->itemAt(0);
    QWidget *w = item->widget();
    auto bw = qobject_cast<BufferWidget *>(w);
    if (bw != nullptr) {
      m_scrollLayout->removeWidget(bw);
      wBufs.emplace_back(bw);
    }
  }

  std::sort(wBufs.begin(), wBufs.end(), cmpBufferWidgets);

  for (auto &bw : wBufs) {
    int idx = m_scrollLayout->count() - 1;
    m_scrollLayout->insertWidget(idx, bw);
  }
}

void BuffersInputWidget::setupIcons()
{
#ifdef Q_OS_LINUX
  ui->qpb_addBuffer->setIcon(QIcon::fromTheme("list-add"));
  ui->qpb_loadBuffer->setIcon(QIcon::fromTheme("document-open"));
  ui->qpb_sortBypH->setIcon(QIcon::fromTheme("view-sort-descending"));
#else
  ui->qpb_addBuffer->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
  ui->qpb_loadBuffer->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  ui->qpb_sortBypH->setIcon(style()->standardIcon(QStyle::SP_ToolBarVerticalExtensionButton));
#endif // Q_OS_
}

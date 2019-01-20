#include "buffersinputwidget.h"
#include "ui_buffersinputwidget.h"

#include "bufferwidget.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <cassert>

BuffersInputWidget::BuffersInputWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::BuffersInputWidget)
{
  ui->setupUi(this);

  m_scrollWidget = new QWidget{};
  m_scrollWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
  m_scrollLayout = new QVBoxLayout{};
  m_scrollWidget->setLayout(m_scrollLayout);

  ui->qscr_bufferList->setWidget(m_scrollWidget);

  m_scrollLayout->addStretch();

  setupIcons();

  connect(ui->qpb_addBuffer, &QPushButton::clicked, this, [this]() { emit this->addBuffer(); });
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

  m_scrollLayout->insertWidget(idx, w);

  connect(w, &BufferWidget::removeMe, this, &BuffersInputWidget::onRemoveBuffer);
  connect(w, &BufferWidget::cloneMe, this, &BuffersInputWidget::onCloneBuffer);
  connect(w, &BufferWidget::bufferChanged, this, &BuffersInputWidget::onBufferChanged);

  emit buffersChanged();
}

void BuffersInputWidget::onBufferChanged(const BufferWidget *)
{
  emit buffersChanged();
}

void BuffersInputWidget::onCloneBuffer(const BufferWidget *w)
{
  emit addBuffer(w->buffer());
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

void BuffersInputWidget::setupIcons()
{
#ifdef Q_OS_LINUX
  ui->qpb_addBuffer->setIcon(QIcon::fromTheme("list-add"));
  ui->qpb_loadBuffer->setIcon(QIcon::fromTheme("document-open"));
#else
#endif // Q_OS_
}

#include "buffercompositionwidget.h"
#include "ui_buffercompositionwidget.h"

#include "editconstituentdialog.h"
#include "internal_models/buffercompositionmodel.h"

#include <gearbox/constituentmanipulator.h>
#include <gearbox/databaseproxy.h>
#include <gearbox/floatingvaluedelegate.h>
#include <gearbox/gdmproxy.h>
#include <QMessageBox>

BufferCompositionWidget::BufferCompositionWidget(gearbox::GDMProxy &gdmProxy,
                                                 gearbox::DatabaseProxy &dbProxy,
                                                 QWidget *parent) :
  QWidget{parent},
  ui{new Ui::BufferCompositionWidget},
  h_gdmProxy{gdmProxy},
  h_dbProxy{dbProxy}
{
  ui->setupUi(this);

  m_fltDelegate = new gearbox::FloatingValueDelegate{this};
  m_model = new BufferCompositionModel{h_gdmProxy, this};
  ui->qtbv_composition->setModel(m_model);
  ui->qtbv_composition->setItemDelegateForColumn(1, m_fltDelegate);

  ui->qtbv_composition->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  setupIcons();

  connect(ui->qpb_add, &QPushButton::clicked, this, &BufferCompositionWidget::onAddConstituent);
  connect(ui->qpb_remove, &QPushButton::clicked, this, &BufferCompositionWidget::onRemoveConstituent);
  connect(ui->qtbv_composition, &QTableView::doubleClicked, this,
          [this](const QModelIndex &idx) {
    if (idx.column() != 0)
      return;
    const auto v = m_model->data(m_model->index(idx.row(), 0), Qt::UserRole);
    if (v.isValid())
      editConstituent(v.toString());
  });
  connect(m_model, &BufferCompositionModel::dataChanged, this, [this]() { emit this->compositionChanged(); });

  setWidgetSizes();
}

BufferCompositionWidget::~BufferCompositionWidget()
{
  delete ui;
}

void BufferCompositionWidget::editConstituent(const QString &name)
{
  gearbox::ConstituentManipulator manipulator{h_gdmProxy, false};

  EditConstituentDialog *dlg = manipulator.makeEditDialog(name.toStdString(), h_gdmProxy, h_dbProxy);
  if (dlg == nullptr)
    return;

  connect(dlg, &EditConstituentDialog::validateInput, &manipulator, &gearbox::ConstituentManipulator::onValidateConstituentInputUpdate);
  connect(dlg, &EditConstituentDialog::addToDatabase, this, &BufferCompositionWidget::onAddToDatabase);

  if (dlg->exec() == QDialog::Accepted) {
    gdm::Constituent updatedCtuent = manipulator.makeConstituent(dlg);

    try {
      if (!h_gdmProxy.update(name.toStdString(), updatedCtuent)) {
        QMessageBox mbox{QMessageBox::Warning, tr("Operation failed"), tr("Failed to update the constituent properties")};
        mbox.exec();
      } else {
        m_model->updateName(name, dlg->name());

        emit compositionChanged();
      }
    } catch (GDMProxyException &ex) {
      QMessageBox mbox{QMessageBox::Warning, tr("Operation failed"), ex.what()};
      mbox.exec();
    }
  }

  delete dlg;
}

void BufferCompositionWidget::onAddConstituent()
{
  static QSize dlgSize{};

  EditConstituentDialog dlg{h_dbProxy, false, this};
  gearbox::ConstituentManipulator manipulator{h_gdmProxy, false};

  if (!dlgSize.isEmpty())
    dlg.resize(dlgSize);

  connect(&dlg, &EditConstituentDialog::validateInput, &manipulator, &gearbox::ConstituentManipulator::onValidateConstituentInput);
  connect(&dlg, &EditConstituentDialog::addToDatabase, this, &BufferCompositionWidget::onAddToDatabase);

  if (dlg.exec() == QDialog::Accepted) {
    auto constituent = manipulator.makeConstituent(&dlg);

    h_gdmProxy.insert(constituent);

    m_model->add(dlg.name());
    dlgSize = dlg.size();

    ui->qtbv_composition->resizeColumnToContents(0);

    emit compositionChanged();
  }
}

void BufferCompositionWidget::onAddToDatabase(const EditConstituentDialog *dlg)
{
  if (!gearbox::ConstituentManipulator::validateConstituentProperties(dlg))
    return;

  const auto ctuent = gearbox::ConstituentManipulator::makeConstituent(dlg);
  h_dbProxy.addConstituent(ctuent.name(), ctuent.physicalProperties().pKas(), ctuent.physicalProperties().mobilities(), ctuent.physicalProperties().charges().low(), ctuent.physicalProperties().charges().high());
}

void BufferCompositionWidget::onRemoveConstituent()
{
  /* Remove only one constituent at a time */
  auto indexes = ui->qtbv_composition->selectionModel()->selectedIndexes();
  if (indexes.empty())
    return;

  const auto &midx = indexes.constFirst();
  if (midx.isValid()) {
    const QString name = m_model->remove(midx.row());
    const auto _name = name.toStdString();
    h_gdmProxy.erase(_name);
  }

  emit compositionChanged();
}

QString BufferCompositionWidget::selectedConsituentName() const
{
  const auto &idx = ui->qtbv_composition->selectionModel()->currentIndex();

  if (!idx.isValid())
    return {};

  const auto nameIdx = m_model->index(idx.row(), 0);

  return m_model->data(nameIdx, Qt::DisplayRole).toString();
}

void BufferCompositionWidget::setWidgetSizes()
{
  const auto &fm = fontMetrics();

  int tbwh = fm.height() * 9;

  ui->qtbv_composition->setMinimumHeight(tbwh);
}

void BufferCompositionWidget::setupIcons()
{
#ifdef Q_OS_LINUX
  ui->qpb_add ->setIcon(QIcon::fromTheme("list-add"));
  ui->qpb_remove->setIcon(QIcon::fromTheme("list-remove"));
#else
  ui->qpb_add->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
  ui->qpb_remove->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
#endif // Q_OS_
}

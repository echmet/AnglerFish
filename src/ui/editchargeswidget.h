#ifndef EDITCHARGESWIDGET_H
#define EDITCHARGESWIDGET_H

#include <QWidget>

namespace Ui {
  class EditChargesWidget;
}

namespace gdm {
  class PhysicalProperties;
}

class FloatingValueDelegate;
class ModifyConstituentChargePushButton;
class AbstractConstituentChargesModel;
class QTableView;

class EditChargesWidget : public QWidget {
  Q_OBJECT
public:
  using PhysPropsMap = std::map<int, double>;

  explicit EditChargesWidget(QWidget *parent = nullptr);
  ~EditChargesWidget();
  int baseCharge(const int chargeLow, const int chargeHigh);
  void initialize();
  void initialize(const gdm::PhysicalProperties &props);
  void initialize(PhysPropsMap pKas, PhysPropsMap mobilities, const int chargeLow, const int chargeHigh);
  int chargeHigh() const;
  int chargeLow() const;
  std::vector<double> mobilities() const;
  std::vector<double> pKas() const;
  void setCharges(const std::map<int, double> &pKas, const std::map<int, double> &mobilities, const int chargeLow, const int chargeHigh);

protected:
  virtual void setupChargesModel(QTableView *tbv) = 0;

  AbstractConstituentChargesModel *m_chargesModel;

private:
  void updateChargeHigh();
  void updateChargeLow();
  void updateChargeModifiers();

  void setupWidget();

  Ui::EditChargesWidget *ui;

  ModifyConstituentChargePushButton *m_qpb_addLow;
  ModifyConstituentChargePushButton *m_qpb_removeLow;
  ModifyConstituentChargePushButton *m_qpb_addHigh;
  ModifyConstituentChargePushButton *m_qpb_removeHigh;

private slots:
  void onAddChargeLow();
  void onAddChargeHigh();
  void onRemoveChargeLow();
  void onRemoveChargeHigh();
  void onReturnPressed();

signals:
  void acceptRequested();
};

#endif // EDITCHARGESWIDGET_H

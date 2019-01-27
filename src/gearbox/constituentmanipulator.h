#ifndef CONSTITUENTVALIDATOR_H
#define CONSTITUENTVALIDATOR_H

#include <gdm/core/constituent/constituent.h>

#include <QObject>

namespace gearbox {
  class DatabaseProxy;
  class GDMProxy;
} // namespace gearbox

class EditConstituentDialog;
class IConstituentEditor;

namespace gearbox {

class ConstituentManipulator : public QObject {
  Q_OBJECT
public:
  explicit ConstituentManipulator(const GDMProxy &proxy, const bool viscosityCorrectionEnabled);
  EditConstituentDialog * makeEditDialog(const std::string &name, GDMProxy &proxy, DatabaseProxy &dbProxy);

  static gdm::Constituent makeConstituent(const IConstituentEditor *dlg);
  static bool validateConstituentProperties(const IConstituentEditor *dlg);

public slots:
  void onValidateConstituentInput(const IConstituentEditor *dlg, bool *ok);
  void onValidateConstituentInputUpdate(const IConstituentEditor *dlg, bool *ok);

private:
  const GDMProxy &h_proxy;
  const bool m_viscosityCorrectionEnabled;
};

} // namespace gearbox

#endif // CONSTITUENTVALIDATOR_H

#ifndef ADDITIONALVALIDATOR_H
#define ADDITIONALVALIDATOR_H

#include <functional>
#include <memory>
#include <QMetaType>
#include <QString>
#include <QVariant>
#include <QVector>

namespace gearbox {

class AdditionalFloatingValidator {
public:
  class InvalidValidatorException : public std::runtime_error {
  public:
    InvalidValidatorException();
  };

  using ValidFunc = std::function<bool (const double)>;

  AdditionalFloatingValidator();
  AdditionalFloatingValidator(const AdditionalFloatingValidator &other) = default;
  AdditionalFloatingValidator(ValidFunc &&func) noexcept;
  virtual ~AdditionalFloatingValidator() = default;
  AdditionalFloatingValidator & operator=(AdditionalFloatingValidator &&other) noexcept;

  virtual bool validate(const double d);

  static bool additionalValidatorsOk(const QObject *obj, const double dv);

  static const char * PROPERTY_NAME;

private:
  const bool m_valid;
  const ValidFunc m_func;

};
using AdditionalFloatingValidatorVec = QVector<std::shared_ptr<AdditionalFloatingValidator>>;

} // namespace gearbox

Q_DECLARE_METATYPE(gearbox::AdditionalFloatingValidator)
Q_DECLARE_METATYPE(gearbox::AdditionalFloatingValidatorVec)

#endif // ADDITIONALVALIDATOR_H

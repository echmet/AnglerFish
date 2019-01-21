#ifndef DATAKEYS_H
#define DATAKEYS_H

class QString;

class DataKeys {
public:
  DataKeys() = delete ;

  static const QString CTUENT_CTUENTS;
  static const QString CTUENT_TYPE;
  static const QString CTUENT_NAME;
  static const QString CTUENT_CHARGE_LOW;
  static const QString CTUENT_CHARGE_HIGH;
  static const QString CTUENT_PKAS;
  static const QString CTUENT_MOBILITIES;
  static const QString CTUENT_VISCOSITY_COEFFICIENT;

  static const QString CTUENT_TYPE_NUCLEUS;
  static const QString CTUENT_TYPE_LIGAND;
  static const QString CPX_NAME;
  static const QString CPX_CHARGE;
  static const QString CPX_MAX_COUNT;
  static const QString CPX_PBS;
  static const QString CPX_MOBILITIES;

  static const QString CPX_COMPLEX_FORMS;
  static const QString CPX_NUCLEUS_CHARGE;
  static const QString CPX_LIGAND_GROUPS;
  static const QString CPX_LIGANDS;
};

#endif // DATAKEYS_H

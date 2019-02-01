#include "csvsummarizer.h"

#include "ui/csvsummarizeroptionsdialog.h"
#include "commonoptions.h"
#include "exception.h"
#include "utility.h"

#include <trstr.h>
#include <gearbox/gearbox.h>
#include <gearbox/chemicalbuffersmodel.h>
#include <gearbox/fitresultsmodel.h>
#include <gearbox/ioniceffectsmodel.h>
#include <gearbox/doubletostringconvertor.h>
#include <gearbox/curveutility.h>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <numeric>

namespace summary {

static const char DEFAULT_DELIMTIER{';'};
static const bool DEFAULT_EXP_PH{false};

inline
CSVSummarizerOptionsPtr makeOptions(const CSVSummarizerOptionsDialog *dlg)
{
  char delimiter;

  auto opts = dlg->options();
  auto delim = std::get<0>(opts);
  if (delim == "\\t")
    delimiter = '\t';
  else if (delim.size() != 1) {
    QMessageBox mbox{QMessageBox::Warning, QObject::tr("Invalid options"), QObject::tr("Delimiter must be a single character or \\t (TAB)")};
    mbox.exec();

    throw std::runtime_error{"Invalid options"};
  } else
    delimiter = delim.front();

  return std::make_shared<CSVSummarizerOptions>(delimiter, std::get<1>(opts));
}

CSVSummarizerOptions::CSVSummarizerOptions(const char _delimiter, const bool _experimentalpH) :
  delimiter{_delimiter},
  experimentalpH{_experimentalpH}
{
}

CSVSummarizerOptions::~CSVSummarizerOptions()
{
}

CSVSummarizer::CSVSummarizer()
{
  m_optsDlg = new CSVSummarizerOptionsDialog{};
  m_optsDlg->setOptions(DEFAULT_DELIMTIER, DEFAULT_EXP_PH);
}

CSVSummarizer::~CSVSummarizer()
{
  delete m_optsDlg;
}

std::string CSVSummarizer::name()
{
  return trstr("CSV summary");
}

SpecificOptionsPtr CSVSummarizer::options()
{
  bool acceptable{false};
  auto opts = makeOptions(m_optsDlg);

  do {
    if (m_optsDlg->exec() == QDialog::Accepted) {
      try {
        opts = makeOptions(m_optsDlg);
        acceptable = true;
      } catch (const std::runtime_error &) {
        /* No-op */
      }
    } else {
      m_optsDlg->setOptions(opts->delimiter, opts->experimentalpH);
      acceptable = true;
    }
  } while (!acceptable);

  return std::move(opts); /* C++11 ISO defect workaround */
}

void CSVSummarizer::summarize(const gearbox::Gearbox &gbox, const CommonOptions &common,
                              const SpecificOptionsPtr &specific, const std::string &output)
{
  char DELIM{DEFAULT_DELIMTIER};
  bool exppH{DEFAULT_EXP_PH};

  auto spec = std::dynamic_pointer_cast<CSVSummarizerOptions>(specific);
  if (spec != nullptr) {
    DELIM = spec->delimiter;
    exppH = spec->experimentalpH;
  }

  QFile fh{QString::fromStdString(output)};
  if (!fh.open(QIODevice::WriteOnly | QIODevice::Text))
    throw Exception{"Cannot open output file for writing"};

  QTextStream stm{&fh};
  const QLocale &loc = gearbox::DoubleToStringConvertor::locale();

  if (common.title.length() > 0)
    stm << common.title.c_str() << "\n\n";

  if (common.includeEstimates) {
    const auto &ests = gbox.analyteEstimates();

    auto hdr = [DELIM](QTextStream &stm) {
      stm << QObject::tr("Charge") << DELIM << QObject::tr("Value") << DELIM << QObject::tr("Fixed") << "\n";
    };

    auto block = [DELIM, &hdr, &ests, &loc](QTextStream &stm, auto vec) {
      hdr(stm);

      auto it = vec.cbegin();
      for (int charge = ests.chargeLow; charge <= ests.chargeHigh; charge++) {
        if (charge == 0)
          continue;

        stm << charge << DELIM << loc.toString(it->value) << DELIM << int(it->fixed) << "\n";
        ++it;
      }
    };

    stm << QObject::tr("Estimated mobilities") << "\n";
    block(stm, ests.mobilities);
    stm << QObject::tr("Estimated pKas") << "\n";
    block(stm, ests.pKas);

    stm << "\n";
  }


  if (common.includeIonicEffects) {
    const auto &ionEffs = gbox.ionicEffectsModel();

    stm << QObject::tr("Debye-H\xC3\xBC ckel correction") << DELIM << int(ionEffs.debyeHuckel()) << "\n";
    stm << QObject::tr("Onsger-Fuoss correction") << DELIM << int(ionEffs.onsagerFuoss()) << "\n";

    stm << "\n";
  }


  if (common.includeBuffers) {
    stm << QObject::tr("Composition") << DELIM
        << QObject::tr("pH") << DELIM;
    if (exppH)
      stm << QObject::tr("pH experimental") << DELIM;
    stm << QObject::tr("Average effective mobility") << DELIM
        << QObject::tr("Effective mobility");
    stm << "\n";

   auto bufs = gbox.chemicalBuffersModel();
   bufs.sortBypH();

    for (const auto &b : bufs) {
      if (b.empty())
        continue;

      stm << Utility::bufferToString(b.model(), common.abbreviateBuffers).c_str() << DELIM;

      stm << b.pH() << DELIM;
      if (exppH)
        stm << DELIM;

      double avgUEff = std::accumulate(b.experimentalMobilities().cbegin(), b.experimentalMobilities().cend(), 0.0);
      avgUEff /= b.experimentalMobilities().size();
      stm << avgUEff << DELIM;

      for (auto &uEff : b.experimentalMobilities())
        stm << uEff << DELIM;

      stm << "\n";
    }

    stm << "\n";
  }

  {
    auto hdr = [DELIM](QTextStream &stm) {
      stm << QObject::tr("Charge") << DELIM
          << QObject::tr("Value") << DELIM
          << QObject::tr("Absolute error") << DELIM
          << QObject::tr("Relative error");
      stm << "\n";
    };
    auto block =[DELIM, &hdr](QTextStream &stm, const gearbox::FitResultsModel &model) {
      hdr(stm);

      int rc = model.rowCount();
      for (int row = 0; row < rc; row++) {
        int chg = model.headerData(row, Qt::Vertical, Qt::DisplayRole).toInt();
        double v = model.data(model.index(row, gearbox::FitResultsModel::ID_VALUE), Qt::DisplayRole).toDouble();
        double ar = model.data(model.index(row, gearbox::FitResultsModel::ID_ABS_ERR), Qt::DisplayRole).toDouble();
        double rr = model.data(model.index(row, gearbox::FitResultsModel::ID_REL_ERR), Qt::DisplayRole).toDouble();

        stm << chg << DELIM
            << v << DELIM
            << ar << DELIM
            << rr << DELIM;
        stm << "\n";
      }
    };

    stm << QObject::tr("Fitted mobilities") << "\n";
    block(stm, gbox.fittedMobilitiesModel());
    stm << QObject::tr("Fitted pKas") << "\n";
    block(stm, gbox.fittedpKasModel());

    stm << "\n";
  }

  if (common.includeCurve) {
    auto blocks = gearbox::CurveUtility::blockify(gbox.mobilityCurveModel());
    auto str = gearbox::CurveUtility::blocksToCSV(blocks, DELIM);

    stm << QObject::tr("Mobility curve") << "\n";
    stm << str.c_str();
  }
}

} // namespace summary

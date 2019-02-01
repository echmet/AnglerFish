#include "csvsummarizer.h"

#include "ui/csvsummarizeroptionsdialog.h"

#include <trstr.h>
#include <QMessageBox>

namespace summary {

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
  m_optsDlg->setOptions(';', false);
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
  return;
}

} // namespace summary

#ifndef TOGGLETRACEPOINTSDIALOG_H
#define TOGGLETRACEPOINTSDIALOG_H

#include <calculators/empfitterinterface.h>

#include <QDialog>
#include <vector>

class QCheckBox;
class QVBoxLayout;

namespace Ui {
class ToggleTracepointsDialog;
}

class ToggleTracepointsDialog : public QDialog {
  Q_OBJECT
public:
  using TracepointInfoVec = std::vector<calculators::EMPFitterInterface::TracepointInfo>;
  using TracepointStateVec = std::vector<calculators::EMPFitterInterface::TracepointState>;

  class TracingSetup {
  public:
    TracingSetup() :
      outputFilePath{""},
      tracepointStates(TracepointStateVec{}),
      tracingEnabled{false}
    {}

    TracingSetup(const QString &_outputFilePath,
                 TracepointStateVec _tracepointStates,
                 const bool _tracingEnabled) :
      outputFilePath{_outputFilePath},
      tracepointStates{_tracepointStates},
      tracingEnabled{_tracingEnabled}
    {}

    TracingSetup(const TracingSetup &other) :
      outputFilePath{other.outputFilePath},
      tracepointStates{other.tracepointStates},
      tracingEnabled{other.tracingEnabled}
    {}

    TracingSetup & operator=(const TracingSetup &other)
    {
      const_cast<QString&>(outputFilePath) = other.outputFilePath;
      const_cast<TracepointStateVec&>(tracepointStates) = other.tracepointStates;
      const_cast<bool&>(tracingEnabled) = other.tracingEnabled;

      return *this;
    }

    TracingSetup & operator=(TracingSetup &&other) noexcept
    {
      const_cast<QString&>(outputFilePath) = std::move(other.outputFilePath);
      const_cast<TracepointStateVec&>(tracepointStates) = std::move(other.tracepointStates);
      const_cast<bool&>(tracingEnabled) = other.tracingEnabled;

      return *this;
    }

    const QString outputFilePath;
    const TracepointStateVec tracepointStates;
    const bool tracingEnabled;
  };

  explicit ToggleTracepointsDialog(const TracepointInfoVec &tracepointInformation,
                                   const TracingSetup &tracingSetup,
                                   QWidget *parent = nullptr);
  ~ToggleTracepointsDialog();
  TracingSetup result() const;

private:
  void setupTracepointList(const TracepointInfoVec &tracepointInformation, const TracingSetup &tracingSetup);
  TracepointStateVec tracepointStates() const;

  Ui::ToggleTracepointsDialog *ui;

  std::vector<std::tuple<QCheckBox *, bool>> m_tracepoints;
  QString m_outputFilePath;
  QWidget *m_tracepointsWidget;

private slots:
  void onAccepted();
  void onDisableAllClicked();
  void onEnableAllClicked();
  void onEnableTracingToggled(const bool enabled);
  void onFilterTextChanged(const QString &text);
  void onSetOutputFile();
};

#endif // TOGGLETRACEPOINTSDIALOG_H

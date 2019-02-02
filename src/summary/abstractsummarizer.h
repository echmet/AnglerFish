#ifndef ABSTRACTSUMMARIZER_H
#define ABSTRACTSUMMARIZER_H

#include <memory>
#include <string>
#include <vector>

namespace gearbox {
  class Gearbox;
} // namespace gearbox

namespace summary {

class CommonOptions;

/*!
 * Allowed file types that a given <tt>Summarizer</tt> can create
 */
class FileType {
public:
  FileType(std::string _description, std::vector<std::string> _suffix);

  const std::string description;                /*!< Description of the file type */
  const std::vector<std::string> suffix;        /*!< List of possible suffixes for the file type */
};

class SpecificOptions {
public:
  virtual ~SpecificOptions() = 0;
};
using SpecificOptionsPtr = std::shared_ptr<SpecificOptions>;

class AbstractSummarizer {
public:
  explicit AbstractSummarizer();
  virtual ~AbstractSummarizer();

  /*!
   * Returns a list of file types that a given <tt>Summarizer</tt> can create.
   * If the <tt>Summarizer</tt> does not provide any information about file
   * types it creates, the list may be empty.
   *
   * @return List of allowed file types.
   */
  virtual std::vector<FileType> allowedFileTypes() const;

  /*!
   * Human-readable name of the <tt>Summarizer</tt>
   *
   * @return Name of the <tt>Summarizer</tt>
   */
  virtual std::string name() const = 0;

  /*!
   * Creates configuration options specific for the given <tt>Summarizer</tt>.
   *
   * @return Pointer to options object
   */
  virtual SpecificOptionsPtr options();

  /*!
   * Create and write the summary file.
   *
   * @param[in] gbox Reference to the data state object
   * @param[in] common Common summary options
   * @param[in] specific <tt>Summarizer</tt>-specific options
   * @param[in] output Path to the output file.
   *
   * @throws NoDefaultOptionsException <tt>Summarizer</tt>-specific options were not set up
   *         and the <tt>Summarizer</tt> does not allow for any sensible default.
   * @throws Exception Unspeficied failure during summary generation.
   */
  virtual void summarize(const gearbox::Gearbox &gbox, const CommonOptions &common,
                         const SpecificOptionsPtr &specific, const std::string &output) = 0;
};

} // namespace summary

#endif // ABSTRACTSUMMARIZER_H

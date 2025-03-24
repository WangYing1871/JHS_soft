#ifndef read_file_H
#define read_file_H 1 
#include <vector>
#include "QThread"
#include "readerwriterqueue/readerwritercircularbuffer.h"

class QTextBrowser;

class read_file : public QThread{
  Q_OBJECT

  typedef std::vector<char> data_array_t;

public:
  read_file();
  read_file(std::string const&, QTextBrowser*);
  ~read_file();

public:
  std::string m_fname="";
  QTextBrowser* text_browser=nullptr;

private:
  //moodycamel::BlockingReaderWriterCircularBuffer<data_array_t> mpool =
  //  moodycamel::BlockingReaderWriterCircularBuffer<data_array_t>{63};



protected:
  void run() override;
};






#endif

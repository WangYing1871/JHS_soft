#ifndef mainwindow_H
#define mainwindow_H 1 

#include <iostream>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <unordered_map>

#include "ui_mainwindow.h"
#include "QMainWindow"
#include "qcustomplot.h"
#include "unpack.h"
#include "readerwriterqueue/readerwritercircularbuffer.h"

#include "QFutureWatcher"

class TFile;
class TGraph;

class QObject;
class QEvent;
class mainwindow : public QMainWindow{
  Q_OBJECT
  typedef std::vector<char> data_array_t;
  typedef moodycamel::BlockingReaderWriterCircularBuffer<data_array_t> rw_queue_t;
  constexpr static std::size_t const cs_block_sz = 10*1024*1024;


public:
  mainwindow();
  ~mainwindow() noexcept;


public slots:
  void test_plot();

  void choose_dat_file();
  void choose_wave_file();
  void unpack();
  void draw_wave(int);
  void quit();
  void abort();
  void stop_bit_set();
  void do_custom();

  void draw_wave_fft(QString const&);
  void draw_wave_fitfun();

private:
  Ui::JHS_GUI ui;
  std::string m_fname;
  std::string m_last_dir="/home";

  QFuture<int> m_future;
  QFutureWatcher<int> m_future_watcher;
  QFuture<int> m_future1;
  QFutureWatcher<int> m_future_watcher1;

protected:
  bool eventFilter(QObject*,QEvent*) override;


private:
  int demo00();
  int demo01(); /* do unpack */
  void handle_start();
  void handle_finish();
  void handle_start1();
  void handle_finish1();
  std::mutex m_mutex;

  rw_queue_t mem_pool = rw_queue_t(63);
  std::atomic<bool> is_stop;
  std::atomic<bool> is_start_custom;


  std::atomic<int> m_read_count;

signals:
  void start_custom();

//temporary
private:
  std::map<std::string,TFile*> m_input_datas;
  std::map<std::pair<uint16_t,uint16_t>,std::pair<float,float>> m_prestal;
  std::unordered_map<std::string,TGraph*> m_graph_buffers;
  void fill_channel();
  void init_file();

};

#endif

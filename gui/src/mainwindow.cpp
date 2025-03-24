#ifndef info_out
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#endif
#include <iostream>
#include <filesystem>
#include <fstream>
#include <array>
#include <fstream>
#include <functional>
#include <regex>
#include <sstream>
#include <boost/algorithm/string.hpp>
#ifdef DEBUG
#include <future>
#include <thread>
#include <boost/timer/timer.hpp>
#include <mutex>
#include <chrono>
#include "read_file.h"
#endif

#include "QFileDialog"
#include "QLineEdit"
#include "QString"
#include "QVector"
#include "QMessageBox"
#include "QFutureWatcher"
#include "QtConcurrent"

#include "mainwindow.h"

/* FIXME */
#include "TTree.h"
#include "TFile.h"
#include "TKey.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TFolder.h"
#include "TF1.h"
#include "TGraph.h"
#include "data_strcut_cint.h"

#include "readerwriterqueue/readerwritercircularbuffer.h"
#include "util.hpp"

//#include "RooRealVar.h"
//#include "RooDataSet.h"
//#include "RooDataHist.h"
//#include "RooGaussian.h"
namespace service{
bool wave_filter(std::vector<uint16_t> const& adcs  
    ,std::pair<float,float> ms,float scale, float comp0, float comp1,int pos=50, int width=20){
  assert(pos>=width);
  auto min_iter = std::min_element(adcs.begin(),adcs.end());
  auto max_iter = std::max_element(adcs.begin(),adcs.end());
  int max_pos = (int)std::distance(adcs.begin(),max_iter);
  if (max_pos<pos) return false;
  auto sec_iter = std::max_element(adcs.begin(),adcs.end()
    ,[&](auto a, auto b){return a<b && b!=*max_iter;});
  if (*max_iter>=4095 && *sec_iter>=4095) return false;
  if (*min_iter<ms.first-comp0*ms.second) return false;
  if (*max_iter<ms.first+comp1*ms.second) return false;
  uint32_t sum_max=0;
  if (*max_iter>=4095) return false;
  size_t beg = max_pos-width;
  size_t end = max_pos+width;
  for (size_t i=beg; i<end; ++i) sum_max += adcs[i];
  float peak_sz = (float)sum_max/(end-beg);
  if (peak_sz<ms.first+scale*ms.second) return false;
  if (peak_sz<0.8**max_iter) return false;
  return true; }

void read_baseline(std::string const& pname
    ,std::map<std::pair<uint16_t,uint16_t>,std::pair<float,float>>& store){
  namespace fs = std::filesystem;
  if (!fs::exists(pname) || !fs::is_regular_file(pname)) return;
  std::ifstream fin(pname.c_str());
  while(!fin.eof()){
    std::string sbuf;
    std::getline(fin,sbuf);
    if (!sbuf.empty() && sbuf[0]!='#'){
      uint16_t a,b;
      float c,d;
      std::stringstream sstr(sbuf.c_str());
      sstr>>a>>b>>c>>d;
      if (sstr.fail()) continue;
      //std::cout<<a<<" "<<b<<" "<<c<<" "<<d<<"\n";
      store.emplace( std::make_pair(a,b) ,std::make_pair(c,d));
    }
  }
  fin.close();
}

void draw_tf1(QCustomPlot* plots
    ,TF1* f
    ,double sample_step = 1.){
  if (!plots || !f) return;
  QVector<double> x, y;
  for (double i=f->GetXmin(), e=f->GetXmax(); i<e; i+=sample_step)
    x.emplace_back(i), y.emplace_back(f->operator()(i));
  
  auto* x_axis = plots->xAxis;
  double xmin = x_axis->range().lower;
  double xmax = x_axis->range().upper;
  auto* y_axis = plots->yAxis;
  double ymin = y_axis->range().lower;
double ymax = y_axis->range().upper;
  x_axis->setRange(
      std::min((*std::min_element(std::begin(x),std::end(x)))-10,xmin)
      ,std::max((*std::max_element(std::begin(x),std::end(x)))+10,xmax)
      );
  y_axis->setRange(
      std::min((*std::min_element(std::begin(y),std::end(y)))-10,ymin)
      ,std::max((*std::max_element(std::begin(y),std::end(y)))+10,ymax)
      );
  plots->addGraph(x_axis,y_axis);
  auto* grp = plots->graph(plots->graphCount()-1);
  QPen pen(QColor(255,0,0));
  pen.setWidth(2);
  grp->setPen(pen);
  grp->setData(x,y);
  plots->replot();
}

void draw_pol1(QCustomPlot* plots, float k, float b
    ,float step=0.1){
  if (!plots) return;
  auto* x_axis = plots->xAxis;
  auto* y_axis = plots->yAxis;
  auto rg = x_axis->range();
  plots->addGraph(x_axis,y_axis);
  auto* grp = plots->graph(plots->graphCount()-1);
  QVector<double> data_x, data_y;
  for (double i=rg.lower, e=rg.upper; i<e; i+=step)
    data_x.emplace_back(i), data_y.emplace_back(k*i+b);
  grp->setData(data_x,data_y);
  QPen pen(QColor(255,255,0));
  pen.setWidth(2);
  grp->setPen(pen);
  plots->replot();



}
double get_x(double x0, double y0, double x1, double y1, double z){
  if ((y0-z)*(y1-z)>0) return std::nan("");
  return x0+(x1-x0)*(z-y0)/(y1-y0); }

bool fit_wave(TGraph* grp, QCustomPlot* plots=nullptr){
  std::size_t const bf_max_pnts = 120;
  std::size_t const af_max_pnts = 10;
  auto const& get_range_and_feature = [=](TGraph* grp, double* p, double* kb = nullptr)
    ->std::pair<int,int>{
    assert(grp);
    std::vector<double> gdata(grp->GetN());
    double _, bufy;
    for (std::size_t i=0, e=grp->GetN(); i<e; ++i)
      grp->GetPoint(i,_,bufy), gdata[i] = bufy;
    auto iter_max = std::max_element(std::begin(gdata),std::end(gdata));
    auto peak = *iter_max;
    auto max_pos = std::distance(std::begin(gdata),iter_max);
    if (max_pos<bf_max_pnts) return std::make_pair(-1,max_pos+af_max_pnts);
    double p0 = std::accumulate(
        std::next(std::begin(gdata),0)
        ,std::next(std::begin(gdata),150)
        ,std::accumulate(
          std::prev(std::end(gdata),150)
          ,std::next(std::end(gdata),0)
          ,0.f)
        )/300.;
    double p1 = (peak-p0)/(27.*std::exp(-3.));
    std::size_t index;
    double hp = (p0+peak)*0.5;
    for (index=max_pos; index>=max_pos-bf_max_pnts; --index)
      if ((gdata[index]-hp)*(gdata[index-1]-hp)<=0) break;
   // if (index==max_pos);
    double hp2 = get_x(index-1,gdata.at(index-1),index,gdata.at(index),hp);
    double k = gdata.at(index)-gdata.at(index-1);
    double p2 = (p0-(hp-k*hp2))/k;
    double p3 = (max_pos-p2)/3.;
    p[0] = p0; p[1] = p1; p[2] = p2; p[3] = p3;
    if (kb) kb[0] = k, kb[1] = hp-k*hp2;
    return std::make_pair(max_pos-bf_max_pnts, max_pos+af_max_pnts);
  };
  auto const& fit_fun = [](double* x, double* p)->double{
    double buf = (x[0]-p[2])/p[3];
    return x[0]<p[2] ? p[0] : p[0]+p[1]*std::pow(buf,3)*std::exp(-buf); };
  double ft[4];
  auto fit_range = get_range_and_feature(grp,ft);
  if (fit_range.first==-1) return false;
  float max_position = fit_range.first+bf_max_pnts;
  //std::cout<<fit_range.first<<" "<<fit_range.second;
  TF1 f("f",fit_fun,fit_range.first,fit_range.second,4);

  std::cout<<ft[0]<<";"<<ft[1]<<";"<<ft[2]<<";"<<ft[3]<<std::endl;
  //f.SetParameter(0,ft[0]); f.SetParLimits(0,550,780);
  //f.SetParameter(1,ft[1]); f.SetParLimits(1,20,4095);
  //f.SetParameter(2,ft[2]); f.SetParLimits(2,320,470);
  //f.SetParameter(2,ft[2]); f.SetParLimits(2,ft[2]-30,ft[2]+50);
  f.SetParameter(0,ft[0]); f.SetParLimits(0,ft[0]-70,ft[0]+70);
  f.SetParameter(1,ft[1]); f.SetParLimits(1,ft[1]*0.7,ft[1]*1.3);
  f.SetParameter(2,ft[2]); f.SetParLimits(2,ft[2]-30,ft[2]+70);
  f.SetParameter(3,ft[3]); f.SetParLimits(3,0.3*ft[3],5*ft[3]);
  grp->Fit(&f,"RQ");
  std::cout<<"===> "<<f.GetChisquare()/f.GetNDF();
  for (int i=0; i<4; ++i ) std::cout<<";"<<f.GetParameter(i);
  if (max_position+10<f.GetParameter(2)){
    std::cout<<"fit error, refit needed\n";
  }

  //std::cout<<"\n";
  if (plots){

    draw_tf1(plots,&f,1);
  }

  return true;

   
}

}


namespace temporary{

template <class _tp>
std::vector<_tp*> get_t_object(TFile* fin, std::string const& path){
  if(!fin) return {};
  std::vector<std::string> names;
  boost::split(names,path,boost::is_any_of("/"));
  if (names.size()==1){
    std::vector<_tp*> ret(1);
    ret[0] = dynamic_cast<_tp*>(fin->Get(names.back().c_str()));
    return ret;
  }
  std::vector<_tp*> ret;

  TFolder* root_fl = dynamic_cast<TFolder*>(fin->Get(names.at(0).c_str()));
  TFolder* record=root_fl;
  for (int i=1; i<names.size()-1; ++i){
    record = dynamic_cast<TFolder*>(record->FindObject(names[i].c_str()));
    if (!record) return {};
  }
  TIter t_iter(record->GetListOfFolders());
  TKey* key=nullptr;
  //info_out(names.back().c_str());
  std::regex reg(names.back().c_str());
  while((key=static_cast<TKey*>(t_iter()))){
    //info_out((std::string)(key->GetName()));
    if (std::regex_match((std::string)key->GetName(),reg)){
      _tp* target = dynamic_cast<_tp*>(record->FindObject(key->GetName()));
      if (target) ret.emplace_back(target);
    }
  }

  return ret;
} 

template <class _tp>
void clear(moodycamel::BlockingReaderWriterCircularBuffer<_tp>& queue){
  _tp buffer;
  if (queue.size_approx()>0)
    while(queue.try_dequeue(buffer));
}

//template <class _tp>
//struct Setter{
//
//  explicit Setter()
//
//  _tp& m_ref;
//};
//
//
//  set(object).seta(...).setb(...).setc(...).
//
//  set(object)(seta)(setb,...)(setc,...)(setd,...)()
//
//  ==> object.seta();


void unpack_file(std::string const& fname,int fec_count=6){
  TTree* data_tree = new TTree("CollectionTree","CollectionTree");
  entry_new entry_buffer;
  std::string dat_name = fname;
  data_tree->Branch("data",std::addressof(entry_buffer));
  std::string entry_out_file = dat_name.substr(
      0,dat_name.find_last_of("."))+"_entry.root";
  TFile* fout = new TFile(entry_out_file.c_str(),"recreate");
  
  //TODO !!! read-write-queue needed! //FIXME
  std::ifstream fin(dat_name.c_str(),std::ios::binary);
  fin.seekg(0,std::ios_base::end);
  size_t fsz = fin.tellg();
  std::size_t oneG = (std::size_t)1000*1024*1024;
  fsz = fsz>oneG? oneG : fsz;
  fin.seekg(0,std::ios_base::beg);
  char* data = new char[fsz];
  fin.read(data,fsz);
  char* iter_beg = data;
  fin.close();
  waveform_by_entry wf;
  wf.fec_count(fec_count);
  wf.set_store(entry_buffer);
  wf.set_tree(data_tree);
  wf.do_parse(iter_beg,iter_beg+fsz);
  fout->cd();
  data_tree->Write(); 
  fout->Write(); fout->Close(); 
  delete[] data;
}
template <class _tp>
void draw_histogram(QCustomPlot* plots, _tp* his){
  if (!his) return;
  int bins = his->GetNbinsX();
  float x_start = his->GetBinLowEdge(1);
  float x_stop =  his->GetBinLowEdge(bins+1);
  int y_start = his->GetMinimum();
  int y_stop = his->GetMaximum();
  y_stop *= 1.2;
  auto* x_axis = his->GetXaxis();
  auto* y_axis = his->GetYaxis();
  std::string xtitle = x_axis->GetTitle(), ytitle = y_axis->GetTitle();
  QVector<double> ticks(bins), counts(bins);
  for (int i=1; i<=bins; ++i)
    ticks[i-1] = his->GetBinLowEdge(i);
  for (int i=1; i<=bins; ++i)
    counts[i-1] = his->GetBinContent(i);
  auto* plot_ax = plots->xAxis;
  auto* plot_ay = plots->yAxis;
  plot_ax->setLabel(xtitle.c_str());
  plot_ax->setRange(x_start,x_stop);
  plot_ay->setLabel(ytitle.c_str());
  plot_ay->setRange(y_start,y_stop);
  auto* bars = new QCPBars(plot_ax,plot_ay);

  bars->setWidth(1.);
  bars->setAntialiased(false);
  bars->setPen(QPen(QColor(0,0,0).lighter(130)));
  bars->setBrush(QColor(255,255,255));
  QSharedPointer<QCPAxisTickerText> text_ticker(new QCPAxisTickerText);
  QVector<QString> labels(bins);
  for (int i=1; i<=bins; ++i){
    if (i%(bins/10)==0) labels[i-1] = std::to_string(i).c_str();
  }
  text_ticker->addTicks(ticks,labels);
  plot_ax->setTicker(text_ticker);
  bars->setData(ticks,counts);
  plots->replot();
}

template <class _tp>
void draw_histogram0(QCustomPlot* plots, _tp* his){
  int bins = his->GetNbinsX();
  auto* x_axis = plots->xAxis;
  auto* y_axis = plots->yAxis;
  float x_start = his->GetBinLowEdge(1);
  float x_stop =  his->GetBinLowEdge(bins+1);
  int y_start = his->GetMinimum();
  int y_stop = his->GetMaximum();
  y_stop *= 1.2;
  x_axis->setRange(x_start,x_stop);
  y_axis->setRange(y_start,y_stop);
  
  plots->addGraph(plots->xAxis,plots->yAxis);
  plots->clearGraphs();
  auto grp0 =plots->graph(0);
  grp0->setPen(QPen(Qt::black));
  grp0->setLineStyle(QCPGraph::lsStepCenter);
  //grp0->setScatterStyle(QCPScatterStyle(
  //      QCPScatterStyle::ssCircle
  //      ,Qt::red
  //      ,Qt::white
  //      ,7
  //      ));
  QVector<double> x0, y0;
  std::cout<<bins<<std::endl;
  for (int i=1; i<=bins; ++i) x0<<his->GetBinCenter(i);
  for (int i=1; i<=bins; ++i) y0<<his->GetBinContent(i);
  grp0->setData(x0,y0);
  plots->replot();
}


void draw_graph(QCustomPlot* plots,TGraph* grp){
  if (!grp) return;
  plots->clearGraphs();
  int npoints = grp->GetN();
  std::pair<double,double> point;
  QVector<double> x,y;
  for (int i=0; i<npoints; ++i){
    grp->GetPoint(i,point.first,point.second);
    x<<point.first;
    y<<point.second;
  }
  double x_start = *std::min_element(x.begin(),x.end());
  double x_stop = *std::max_element(x.begin(),x.end());
  double y_start = *std::min_element(y.begin(),y.end());
  double y_stop = *std::max_element(y.begin(),y.end());
  double yr = y_stop-y_start;
  auto* x_axis = plots->xAxis;
  auto* y_axis = plots->yAxis;
  x_axis->setRange(x_start,x_stop);
  y_axis->setRange(y_start-.2*yr,y_stop+.2*yr);

  plots->clearGraphs();
  plots->addGraph(x_axis,y_axis);
  auto grp0 =plots->graph(0);
  grp0->setPen(QPen(Qt::blue));
  //for (auto&& xx : y) std::cout<<xx<<std::endl;
  grp0->setData(x,y);
  plots->replot();
}

template <class _tp,class _up>
void draw_2d_graph(QCustomPlot* plots
    , _tp first, _tp last
    ,_up yfirst, _up ylast){
  if (!plots) return;
  typedef decltype(*std::declval<_tp>()) x_value_t;
  typedef decltype(*std::declval<_up>()) y_value_t;
  auto* x_axis = plots->xAxis;
  auto* y_axis = plots->yAxis;
  std::size_t dis = std::min(std::distance(first,last),std::distance(yfirst,ylast));
  x_value_t xmin = *std::min_element(first,last);
  x_value_t xmax = *std::max_element(first,last);
  y_value_t ymin = *std::min_element(yfirst,ylast);
  y_value_t ymax = *std::max_element(yfirst,ylast);
  x_axis->setRange(xmin-10,xmax+10);
  y_axis->setRange(ymin-10,ymax+10);
  plots->addGraph(x_axis,y_axis);
  auto grp0 =plots->graph(plots->graphCount()-1);
  QVector<double> x_data(dis), y_data(dis);
  for (auto&& x : x_data) x = *std::next(first++);
  for (auto&& x : y_data) x = *std::next(yfirst++);
  grp0->setPen(QPen(Qt::green));
  grp0->setData(x_data,y_data);
  plots->replot();
}



}

mainwindow::mainwindow(){
  ui.setupUi(this);
  auto* mean_view = (QCustomPlot*)ui.mean_qcpt;
  is_stop.store(false);
  is_start_custom.store(false);
  connect(ui.Test_Button,&QAbstractButton::clicked
      ,this,&mainwindow::test_plot);

  connect(ui.Open,&QAbstractButton::clicked
      ,this,&mainwindow::choose_dat_file);
  connect(ui.pushButton_3,&QAbstractButton::clicked
      ,this,&mainwindow::choose_wave_file);

  connect(ui.actionabout,&QAction::triggered,this,&mainwindow::abort);
  connect(ui.actionaboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  connect(ui.actionquit,&QAction::triggered,this,&mainwindow::quit);
  connect(ui.unpack,&QAbstractButton::clicked,this,&mainwindow::unpack);
  connect(ui.channel_id,SIGNAL(currentIndexChanged(int)),this,SLOT(draw_wave(int)));

  connect(ui.t1,&QAbstractButton::clicked ,this ,&mainwindow::stop_bit_set);
  QObject::connect(
      std::addressof(m_future_watcher),&QFutureWatcher<int>::started
      ,[this]{this->handle_start();}
      );
  QObject::connect(
      std::addressof(m_future_watcher),&QFutureWatcher<int>::finished
      ,[this]{this->handle_finish();}
      );
  QObject::connect(
      std::addressof(m_future_watcher1),&QFutureWatcher<int>::started
      ,[this]{this->handle_start1();}
      );
  QObject::connect(
      std::addressof(m_future_watcher1),&QFutureWatcher<int>::finished
      ,[this]{this->handle_finish1();}
      );
  //mem_pool = rw_queue_t(63);
  QObject::connect(
      this,&mainwindow::start_custom
      ,this,&mainwindow::do_custom
      );
  QObject::connect(
      ui.pushButton_2,&QAbstractButton::clicked
      ,this,&mainwindow::fill_channel
      );
  //QObject::connect(
  //    ui.lineEdit_2,&QLineEdit::editingFinished
  //    ,this,&mainwindow::fill_channel
  //    );
  QObject::connect(ui.channel_id,&QComboBox::currentTextChanged
        ,this,&mainwindow::draw_wave_fft);
       
  {
    init_file();
    service::read_baseline("20250226091806_baseline_prestal.txt",m_prestal);
  }
  QObject::connect(ui.lineEdit_4,&QLineEdit::editingFinished
      ,this,&mainwindow::draw_wave_fitfun);
  QObject::connect(ui.lineEdit_5,&QLineEdit::editingFinished
      ,this,&mainwindow::draw_wave_fitfun);
  dynamic_cast<QCustomPlot*>(ui.wavex_qcpt)->install_event_filter_axis();
  auto* wavex_qcpt = dynamic_cast<QCustomPlot*>(ui.wavex_qcpt);
  wavex_qcpt->installEventFilter(this);
  auto* axis_x0 = wavex_qcpt->xAxis;
  axis_x0->installEventFilter(this);

}
//---------------------------------------------------------------------
void mainwindow::draw_wave_fitfun(){
  //ui.textBrowser->append(__func__);
  auto* lineedit = qobject_cast<QLineEdit*>(sender());
  auto const& fit_fun = [](double* x, double* p)->double{
    double buf = (x[0]-p[2])/p[3];
    return x[0]<p[2] ? p[0] : p[0]+p[1]*std::pow(buf,3)*std::exp(-buf); };
  TF1 f("f",fit_fun,0,1024,4);
  double par[4] = {1.,1.,1.,0.};
  std::vector<std::string> numbers;
  boost::split(numbers,lineedit->text().toStdString().c_str(),boost::is_any_of(";"));
  if (numbers.size()<4) return;
  for (int i=0; i<4; ++i) f.SetParameter(i,par[i] = std::stod(numbers[i]));
  std::vector<double> data_x(1024);
  std::iota(std::begin(data_x),std::end(data_x),0);
  std::vector<double> data_y(1024);
  for (int i=0; i<1024; ++i) data_y[i] = f.operator()(i);

  QCustomPlot* qcpt_canvas = nullptr;
  if (lineedit==ui.lineEdit_5){
    qcpt_canvas = dynamic_cast<QCustomPlot*>(ui.wavex_qcpt);
  }else if (lineedit==ui.lineEdit_4){
    qcpt_canvas = dynamic_cast<QCustomPlot*>(ui.qcpt_user0);
  }
  temporary::draw_2d_graph(qcpt_canvas
      ,std::begin(data_x), std::end(data_x)
      ,std::begin(data_y), std::end(data_y)
      );



}

void mainwindow::init_file(){
  std::string fname = "wave.root";
  m_input_datas.emplace("wave",new TFile(fname.c_str()));
}
void mainwindow::fill_channel(){
  ui.channel_id->clear();
  m_graph_buffers.clear();
  std::string fpath = ui.lineEdit_2->text().toStdString();
  if (fpath.empty()) return;
  auto iter = m_input_datas.find("wave_view");
  
  auto grp = iter==m_input_datas.end() ? 
    temporary::get_t_object<TGraph>(m_input_datas.at("wave"),fpath)
    : temporary::get_t_object<TGraph>(m_input_datas.at("wave_view"),fpath);
  if (grp.empty()){

    return;
  }
  for (auto&& x : grp){
    ui.channel_id->addItem(x->GetName());
    m_graph_buffers.emplace(x->GetName(),x);
  }


}

void mainwindow::draw_wave_fft(QString const& names){
  std::string name_str = names.toStdString();
  for (auto&& x : name_str) if (!std::isdigit(x)) x=' ';
  uint16_t lid, cid;
  std::stringstream sstr(name_str.c_str());
  sstr>>lid>>cid;
  auto* wave_view = dynamic_cast<QCustomPlot*>(ui.wavex_qcpt);
  auto iter = m_graph_buffers.find(names.toStdString());
  if (iter == m_graph_buffers.end()){
    return;
  }
  temporary::draw_graph(wave_view,iter->second);
  std::vector<double> wave_data(iter->second->GetN());
  //qDebug()<<wave_data.size();
  namespace fftw3 = util::fftw3;
  namespace root = util::root;
  root::get_t_graph_y_data(iter->second,wave_data.data());
  fftw3::filter_t<std::size_t> filter_only_direct = [](fftw_complex* ff, std::size_t n){
    for (int i=3; i<15; ++i) ff[i][0] = 0, ff[i][1] = 0; };


  auto wave_direct = fftw3::fftw3_dft_and_idft(std::begin(wave_data),std::end(wave_data)
      ,filter_only_direct
      ,wave_data.size()
      );
  //auto wave_direct = fftw3::fftw3_dft_and_idft_FORTEST(wave_data);

  //temporary::get_t_object<TGraph>(m_input_datas.at("wave"),"Event-0/wave0-.*");
  //auto grp = temporary::get_t_object<TGraph>(m_input_datas.at("wave"),"Event-0/wave2-3");
  //if (grp[0]) temporary::draw_graph(wave_view,grp[0]);
  //for (auto&& x : wave_data)
  //  info_out(x);

  //for (auto&& x : wave_direct){
  //  info_out(x);
  //  
  //}
  /*
  {
  std::vector<int> data_x(wave_direct.size())
    ,data_y(wave_direct.size());
  std::iota(std::begin(data_x),std::end(data_x),0);
  std::transform(std::begin(wave_direct),std::end(wave_direct),
      //std::begin(data_y),[](auto a){return std::sqrt(a.real()*a.real()+a.imag()*a.imag());});
      std::begin(data_y),[](auto a){return a.real();});

  auto* wave_view_1 = dynamic_cast<QCustomPlot*>(ui.wavey_qcpt);
  temporary::draw_2d_graph(wave_view_1
      ,std::begin(data_x), std::end(data_x)
      ,std::begin(data_y), std::end(data_y)
      );

  }
  */
  /*
  {
    namespace dct = util::dct;
    std::vector<double> dft_data(wave_data.size());
    dct::dft_forward(wave_data.data(),wave_data.size(),dft_data.data());
    std::vector<double> idft_data(wave_data.size());

    //for(int i=17; i<dft_data.size(); ++i) dft_data[i]=0;
    //for(int i=12; i<15; ++i) dft_data[i]=0;
    //for(int i=20; i<dft_data.size(); ++i) dft_data[i]=0;
    for(int i=20; i<dft_data.size(); ++i) dft_data[i]=0;
    dft_data[4] = 0;
    dft_data[5] = 0;
    
    //for(int i=0; i<20; ++i) dft_data[i]=0;
    //for(int i=1; i<15; ++i ) dft_data[i] = 0; //baseline
    //for(int i=15; i<600; ++i ) dft_data[i] = 0;


    dct::dft_backward(dft_data.data(),dft_data.size(),idft_data.data());

    std::vector<int> data_x(wave_direct.size());
    std::iota(std::begin(data_x),std::end(data_x),0);
    auto* wave_view_1 = dynamic_cast<QCustomPlot*>(ui.wavey_qcpt);
    temporary::draw_2d_graph(wave_view_1
        ,std::begin(data_x), std::end(data_x)
        ,std::begin(idft_data), std::end(idft_data)
        );



  }
  */
  {
    std::vector<uint16_t> gdata(iter->second->GetN());
    double bufx, bufy;
    for (std::size_t i=0, e=iter->second->GetN(); i<e; ++i)
      iter->second->GetPoint(i,bufx,bufy), gdata[i] = bufy;
    bool state = service::fit_wave(iter->second,wave_view);
    if (!state) info_out("[ERROR] invalid wave, fit gived up");
    auto kb = m_prestal.at(std::make_pair(lid,cid));
    bool is_valid = service::wave_filter(
        gdata,kb,3,5,2.5,120,4);

    std::cout<<" "<<is_valid<<std::endl;

    service::draw_pol1(wave_view,0,kb.first);
     

  }

}

mainwindow::~mainwindow(){
  for(auto&& [x,y] : m_input_datas) if (y) y->Close();
}
//---------------------------------------------------------------------
void mainwindow::stop_bit_set(){
  auto* button = qobject_cast<QPushButton*>(sender());
  std::string text = button->text().toStdString();
  ui.textBrowser->append(text.c_str());
  if (text=="Start"){
    is_stop.store(false);
    button->setText("Stop");
  }else if(text=="Stop"){
    is_stop.store(true);
    button->setText("Start");
  }

}

//---------------------------------------------------------------------
void mainwindow::handle_start(){
  ui.textBrowser->append("handle start");
}
void mainwindow::handle_finish(){
  ui.textBrowser->append("handle finished");
}
void mainwindow::handle_start1(){
  ui.textBrowser->append("handle start1");
}
void mainwindow::handle_finish1(){
  ui.textBrowser->append("handle finished1");
}
//---------------------------------------------------------------------
void mainwindow::draw_wave(int index){
  //TFile* fin = new TFile("wave.root");


  //ui.textBrowser->append("draw_wave");

}
//---------------------------------------------------------------------

void mainwindow::test_plot(){
  ui.textBrowser->append("test_plot");
  
}

void mainwindow::choose_dat_file(){
 // QFileDialog fileDialog(this, tr("Open Bookmark File"));
  namespace fs = std::filesystem;
  QString fname = QFileDialog::getOpenFileName(this,
    tr("Open File"), m_last_dir.c_str(), tr("Dat Files (*.dat *.root)"));
  if (std::filesystem::exists(fname.toStdString().c_str())){
    fs::path p(fname.toStdString().c_str());
    m_last_dir = p.parent_path();
    ui.textBrowser->append(fname);
    ui.textBrowser->append(m_last_dir.c_str());
    m_fname = fname.toStdString();
    ui.lineEdit->setText(fname);
  }
}

void mainwindow::choose_wave_file(){
  namespace fs = std::filesystem;
  QString fname = QFileDialog::getOpenFileName(this
      ,tr("Open File"),m_last_dir.c_str(),tr("Dat Files (*.root)"));
  if (std::filesystem::exists(fname.toStdString().c_str())){
    fs::path p(fname.toStdString().c_str());
    m_last_dir = p.parent_path();
    //ui.textBrowser->append(fname);
    //ui.textBrowser->append(m_last_dir.c_str());
    //m_fname = fname.toStdString();
    ui.lineEdit_6->setText(fname);
    auto* tf = new TFile(fname.toStdString().c_str());
    if (tf && !tf->IsZombie())
      m_input_datas.emplace("wave_view",tf);
  }
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void mainwindow::abort(){
  QMessageBox::about(this, tr("Gas Flow Control"),
          tr("<h2>MTPC GUI 0.0.1</h2>"
             "<p>Copyright &copy; 2023 Software Inc. JW company."
             "<p>Multi Time Projection Chamber(MTPC) analysis codes"
             "<p>Author(s): Wang Ying@JW"
            ));
}
int mainwindow::demo01(){
  data_array_t block;
  info_out("demo01");
  //std::this_thread::sleep_for(std::chrono::microseconds(500));
  while(true){
    if (is_stop.load()){
      //info_out("STOP!");
      //info_out(mem_pool.size_approx());
      if (mem_pool.size_approx()==0) { 
        is_start_custom.store(false);
        //info_out("break");
        break;
      }
    }
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    bool ok = mem_pool.try_dequeue(block);
    //bool ok = mem_pool.wait_dequeue_timed(block,50);
    info_out(ok);
    //bool ok = mpool.wait_dequeue_timed(block,del);
    if (ok){
      std::lock_guard<std::mutex> lock(m_mutex);
      info_out("readed");
      info_out(block.size());
  
      
    }else{
      std::lock_guard<std::mutex> lock(m_mutex);
      info_out("empty pool");
    }
  }
  return 0;

}
int mainwindow::demo00(){
  info_out("demo00");
  //ui.textBrowser->append("demo00");
  //ui.textBrowser->append(m_fname.c_str());
  //m_fname = 
  // "/home/wangying/desktop/JW_jhs/tstData/20241029161110_baseline.dat";
  std::ifstream fin(m_fname.c_str(),std::ios::binary);
  std::vector<char> data(cs_block_sz);
  if (mem_pool.size_approx()>0)
    temporary::clear(mem_pool);
  info_out(mem_pool.size_approx());
  while(!fin.eof() && !is_stop.load()){
    fin.read(data.data(),cs_block_sz);
    bool ok = mem_pool.try_enqueue(data);
    //info_out(mem_pool.size_approx());
    if (!ok){
      std::lock_guard<std::mutex> lock(m_mutex);
      auto readed = fin.gcount();
      fin.seekg(-readed,std::ios::cur);
      info_out("write timeout");
      //ui.textBrowser->append("wirte timeout");
    }
    else{
      std::lock_guard<std::mutex> lock(m_mutex);
      info_out(fin.tellg());
      if (is_start_custom.load()==false && mem_pool.size_approx()>=3){
        emit start_custom();
      }
      //ui.textBrowser->append(std::to_string(fin->tellg()).c_str());
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  info_out("------");
  is_stop.store(true);
  fin.close();
  return 0;
}

void mainwindow::do_custom(){
  is_start_custom.store(true);
  auto fun_binder1 = std::bind(
      &mainwindow::demo01
      ,this
      );
  m_future_watcher1.setFuture(m_future1 = QtConcurrent::run(fun_binder1));
}



void mainwindow::quit(){
  this->close();
}

void mainwindow::unpack(){
  {
    auto fun_binder = std::bind(
        &mainwindow::demo00
        ,this
        );
    m_future_watcher.setFuture(m_future = QtConcurrent::run(fun_binder));

   // auto fun_binder1 = std::bind(
   //     &mainwindow::demo01
   //     ,this
   //     );
   // m_future_watcher1.setFuture(m_future1 = QtConcurrent::run(fun_binder1));


    return;

  }
  {
    //Failed
  std::string fname = "/home/wangying/desktop/JW_jhs/tstData/20241029161110_baseline.dat";
  //read_file reader00(fname,ui.textBrowser);
  //reader00.start();
  //std::this_thread::sleep_for(std::chrono::seconds(5));
  return;
  }
  //produce.text_browser = ui.textBrowser;
  //produce.m_fname ="/home/wangying/desktop/JW_jhs/tstData/20241029161110_baseline.dat"


//  {
//  ui.textBrowser->append("unpack slot");
//  constexpr static std::size_t const c_block_size = 10*1024*1024;
//  typedef std::vector<char> data_array_t;
//  moodycamel::BlockingReaderWriterCircularBuffer<data_array_t> mpool(63);
//  std::string fname = "/home/wangying/desktop/JW_jhs/tstData/20241029161110_baseline.dat";
//  std::ifstream fin(fname.c_str(),std::ios::binary);
//
//  auto m_enque = [&](){
//  while(!fin.eof()){
//    ui.textBrowser->append("WY");
//    data_array_t buffer;
//    buffer.resize(c_block_size);
//    fin.read(buffer.data(),c_block_size);
//  }
//  };
//  std::thread file_reader(m_enque);
//  file_reader.join();
//
//
//  return;
//  }


#ifdef DEBUG

  std::mutex g_mutex;
  constexpr static std::size_t const c_block_size = 10*1024*1024;
  typedef std::vector<char> data_array_t;
  moodycamel::BlockingReaderWriterCircularBuffer<data_array_t> mpool(63);
  auto m_enque = [&](std::ifstream* fin,uint32_t bs)->void{
    //if (!fin) return;

    //std::array<char,c_block_size> buffer;
    //fin.read(buffer.data(),10);
    
    //while(!fin.eof() && !stop_flag.load()){
    //data_array_t buffer;
    //buffer.resize(bs);
   
    ui.textBrowser->append("WY");
    return;
    while(!fin->eof()){
      ui.textBrowser->append("WY");
      break;
    

      //data_array_t buffer;
      //buffer.resize(bs);
      //fin->read(buffer.data(),bs);
      //bool ok = mpool.try_enqueue(buffer);
      //if (!ok){
      //  std::lock_guard<std::mutex> lock(g_mutex);
      //  auto readed = fin->gcount();
      //  fin->seekg(-readed,std::ios::cur);
      //  ui.textBrowser->append("wirte timeout");
      //}
      //else{
      //  std::lock_guard<std::mutex> lock(g_mutex);
      //  ui.textBrowser->append(std::to_string(fin->tellg()).c_str());
      //}

    //  block_t buf;
    //  fin->read(buf.data(),bs);
    //  bool ok = mpool.try_enqueue(buf);
    //  if (!ok){
    //    std::lock_guard<std::mutex> lock(g_mutex);
    //    auto readed = fin->gcount();
    //    fin->seekg(-readed,std::ios::cur);
    //    ui.textBrowser->append("wirte timeout");
    //  }else{
    //    std::lock_guard<std::mutex> lock(g_mutex);
    //    ui.textBrowser->append(std::to_string(fin->tellg()).c_str());
    //  }
    ////  //std::cout<<"BLOCK READ\n";
    ////  fin.read(buf.data(),bs);
    ////  bool ok = mpool.try_enqueue(buf);
    ////  //mpool.wait_enqueue(buf);
    ////  //bool ok = mpool.wait_enqueue_timed(buf,100);
    ////  if (!ok){
    ////    std::lock_guard<std::mutex> lock(g_mutex);
    ////    fin.seekg(-bs,std::ios::cur);
    ////    info_out("WRITE timeout!");
    ////  }else{
    ////    std::lock_guard<std::mutex> lock(g_mutex);
    ////    info_out(fin.tellg());
    ////    //info_out("enqueue");
    ////  }

    }
    //stop_flag.store(true);
  };
  



  ui.textBrowser->append(m_fname.c_str());
#endif
#ifdef DEBUG
    //std::string fname = "/home/wangying/desktop/JW_jhs/tstData/20241029161110_baseline.dat";
    //std::ifstream fin(fname,std::ios::binary);
    //boost::timer::auto_cpu_timer progress;
    ////std::future<void> file_reader(std::async(m_enque,&fin,c_block_size));
    ////std::thread file_reader(m_enque,&fin,c_block_size);
    //
    //std::thread file_reader(m_enque
    //    ,&fin,10*1024*1024
    //    );

    //file_reader.join();
    //info_out("AAA");
    //fin.close();

        //void* tag = std::malloc(10*1024*1024);
        //std::free(tag);
        //while(!fin.eof()){
        //  std::cout<<"WY"<<std::endl;
        //  //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //  fin.read(data,10*1024*1024);
        //}
        //fin.close();
        //delete[] d0;
    
  

#endif
  return;
  auto* mean_view = (QCustomPlot*)ui.mean_qcpt;
  auto* rms_view = (QCustomPlot*)ui.rms_qcpt;
  //mean_view->xAxis->setRange(-100,100);
  //mean_view->yAxis->setRange(-100,100);
  //mean_view->xAxis->setTicks(true);
  //mean_view->xAxis->setTickLabels(true);
  //mean_view->xAxis->grid()->setVisible(true);
  //mean_view->xAxis->setVisible(true); 
  //mean_view->xAxis->setTicks(true);
  //mean_view->xAxis->setBasePen(QPen(Qt::white));
  //mean_view->xAxis->setTickPen(QPen(Qt::black));
  //mean_view->xAxis2->setBasePen(QPen(Qt::black));
  //mean_view->legend->setVisible(true);
  //mean_view->legend->setFont(QFont("Helvetica", 5));
  //mean_view->legend->setTextColor(Qt::red);
  //mean_view->legend->setBrush(QBrush(Qt::transparent));
  ////mean_view->axisRect()->insetLayout()->setInsetAlignment()
  //mean_view->legend->setIconSize(20, 10);//设置图标大小
  //mean_view->replot();



 // this->plot->xAxis->setTicks(true);
 //      this->plot->yAxis->setTicks(true);
 //      // 刻度值显示
 //      this->plot->xAxis->setTickLabels(true);
 //      this->plot->yAxis->setTickLabels(true);
 //      // 网格显示
 //      this->plot->xAxis->grid()->setVisible(true);
 //      this->plot->yAxis->grid()->setVisible(true);
 //      // 子网格显示
 //      this->plot->xAxis->grid()->setSubGridVisible(true);
 //      this->plot->yAxis->grid()->setSubGridVisible(true);
 //      // 右和上坐标轴、刻度值显示
 //      this->plot->xAxis->setVisible(true);
 //      this->plot->yAxis->setVisible(true);
 //      this->plot->xAxis->setTicks(true);
 //      this->plot->yAxis->setTicks(true);
 //      this->plot->yAxis->setTickLabels(false);


 //      this->plot->xAxis->setBasePen(QPen(Qt::white));
 //      this->plot->xAxis->setTickPen(QPen(Qt::black));
 //      this->plot->xAxis->setTickLabelColor(Qt::black);
 //      this->plot->xAxis2->setBasePen(QPen(Qt::white));

 //       this->plot->yAxis->setBasePen(QPen(Qt::white));
 //       this->plot->yAxis->setTickPen(QPen(Qt::white));
 //       this->plot->yAxis->setTickLabelColor(Qt::white);
 //       this->plot->yAxis2->setBasePen(QPen(Qt::white));

  auto* demo_file = new TFile("baseline.root");
  TH1F* his1 = new TH1F("his1","his1",300,0,300);
  for (int i=1; i<=100; ++i)
    his1->SetBinContent(i,i);
  auto* mean_his = (TH1I*)(demo_file->Get("mean"));
  auto* rms_his = (TH1F*)(demo_file->Get("rms"));
  temporary::draw_histogram0(mean_view,mean_his);
  temporary::draw_histogram0(rms_view,rms_his);
  //temporary::draw_histogram0(mean_view,his2);
  mean_view->replot();
  //





}
//---------------------------------------------------------------------
bool mainwindow::eventFilter(QObject* object,QEvent* event){
  if (object==(QObject*)(ui.wavex_qcpt)){
    switch(event->type()){
      case QEvent::Enter:
        //info_out("enter");
        //ui.wavex_qcpt->xAxis->setLabelColor(QColor(255,255,255));
        //ui.wavex_qcpt->replot();
        break;
      case QEvent::Leave:
        //info_out("leave");
        //ui.wavex_qcpt->xAxis->setLabelColor(QColor(255,255,0));
        //ui.wavex_qcpt->replot();
        break;
    }
    return QWidget::eventFilter(object,event);
  }
  /*
  if (object==(QObject*)(ui.wavex_qcpt->xAxis)){
    switch(event->type()){
      case QEvent::Enter:
        info_out("enter");
        break;
      case QEvent::Leave:
        info_out("leave");
        break;
    }
    return QWidget::eventFilter(object,event);
  }
  */
  return false;
}

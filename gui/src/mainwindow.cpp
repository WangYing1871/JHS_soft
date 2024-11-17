#include <iostream>
#include <filesystem>
#include "QFileDialog"
#include "QString"
#include "QVector"
#include "QMessageBox"

#include "mainwindow.h"

/* FIXME */
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraph.h"
#include "data_strcut_cint.h"
#include <fstream>

namespace temporary{

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
  grp0->setData(x,y);

  plots->replot();






}



}




mainwindow::mainwindow(){
  ui.setupUi(this);
  auto* mean_view = (QCustomPlot*)ui.mean_qcpt;

/*
{
  QVector<double> x(5), y(5);
  x << 1 << 2 << 3 << 4 << 5;
  y << 10 << 20 << 30 << 40 << 50;

  mean_view->addGraph();
  mean_view->graph(0)->setData(x, y);
  mean_view->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 设置颜色和透明度
  mean_view->xAxis->setLabel("X轴标签");
  mean_view->yAxis->setLabel("Y轴标签");
  mean_view->xAxis->setRange(0, 5); // 根据实际数据范围调整
  mean_view->yAxis->setRange(0, 60); // 根据实际数据范围调整

  mean_view->replot();
}
*/

  connect(ui.Test_Button,&QAbstractButton::clicked
      ,this,&mainwindow::test_plot);

  connect(ui.Open,&QAbstractButton::clicked
      ,this,&mainwindow::choose_dat_file);

  connect(ui.actionabout,&QAction::triggered,this,&mainwindow::abort);
  connect(ui.actionaboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  connect(ui.actionquit,&QAction::triggered,this,&mainwindow::quit);
  connect(ui.unpack,&QAbstractButton::clicked,this,&mainwindow::unpack);
  connect(ui.channel_id,SIGNAL(currentIndexChanged(int)),this,SLOT(draw_wave(int)));
  
}
//---------------------------------------------------------------------
void mainwindow::draw_wave(int index){
  TFile* fin = new TFile("wave.root");
  GetLi


  ui.textBrowser->append("draw_wave");

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
//---------------------------------------------------------------------
void mainwindow::abort(){
  QMessageBox::about(this, tr("Gas Flow Control"),
          tr("<h2>TPC GUI 0.0.1</h2>"
             "<p>Copyright &copy; 2024 Software Inc. JW company."
             "<p>Nuclear Test Ban Treaty TPC detector analysis codes"
             "<p>Author(s): Wang Ying@JW"
            ));
}

void mainwindow::quit(){
  this->close();
}

void mainwindow::unpack(){

  ui.textBrowser->append("unpack slot");
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
 /*
  mean_view->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
mean_view->legend->setVisible(true);
QFont legendFont = font();  // start out with MainWindow's font..
legendFont.setPointSize(9); // and make a bit smaller for legend
mean_view->legend->setFont(legendFont);
mean_view->legend->setBrush(QBrush(QColor(255,255,255,230)));
// by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
mean_view->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

// setup for graph 0: key axis left, value axis bottom
// will contain left maxwell-like function
mean_view->addGraph(mean_view->yAxis, mean_view->xAxis);
mean_view->graph(0)->setPen(QPen(QColor(255, 100, 0)));
mean_view->graph(0)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // fill with texture of specified image
mean_view->graph(0)->setLineStyle(QCPGraph::lsLine);
mean_view->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
mean_view->graph(0)->setName("Left maxwell function");

// setup for graph 1: key axis bottom, value axis left (those are the default axes)
// will contain bottom maxwell-like function with error bars
mean_view->addGraph();
mean_view->graph(1)->setPen(QPen(Qt::red));
mean_view->graph(1)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // same fill as we used for graph 0
mean_view->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
mean_view->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));
mean_view->graph(1)->setName("Bottom maxwell function");
QCPErrorBars *errorBars = new QCPErrorBars(mean_view->xAxis, mean_view->yAxis);
errorBars->removeFromLegend();
errorBars->setDataPlottable(mean_view->graph(1));

// setup for graph 2: key axis top, value axis right
// will contain high frequency sine with low frequency beating:
mean_view->addGraph(mean_view->xAxis2, mean_view->yAxis2);
mean_view->graph(2)->setPen(QPen(Qt::blue));
mean_view->graph(2)->setName("High frequency sine");

// setup for graph 3: same axes as graph 2
// will contain low frequency beating envelope of graph 2
mean_view->addGraph(mean_view->xAxis2, mean_view->yAxis2);
QPen blueDotPen;
blueDotPen.setColor(QColor(30, 40, 255, 150));
blueDotPen.setStyle(Qt::DotLine);
blueDotPen.setWidthF(4);
mean_view->graph(3)->setPen(blueDotPen);
mean_view->graph(3)->setName("Sine envelope");

// setup for graph 4: key axis right, value axis top
// will contain parabolically distributed data points with some random perturbance
mean_view->addGraph(mean_view->yAxis2, mean_view->xAxis2);
mean_view->graph(4)->setPen(QColor(50, 50, 50, 255));
mean_view->graph(4)->setLineStyle(QCPGraph::lsNone);
mean_view->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
mean_view->graph(4)->setName("Some random data around\na quadratic function");

// generate data, just playing with numbers, not much to learn here:
QVector<double> x0(25), y0(25);
QVector<double> x1(15), y1(15), y1err(15);
QVector<double> x2(250), y2(250);
QVector<double> x3(250), y3(250);
QVector<double> x4(250), y4(250);
for (int i=0; i<25; ++i) // data for graph 0
{
  x0[i] = 3*i/25.0;
  y0[i] = qExp(-x0[i]*x0[i]*0.8)*(x0[i]*x0[i]+x0[i]);
}
for (int i=0; i<15; ++i) // data for graph 1
{
  x1[i] = 3*i/15.0;;
  y1[i] = qExp(-x1[i]*x1[i])*(x1[i]*x1[i])*2.6;
  y1err[i] = y1[i]*0.25;
}
for (int i=0; i<250; ++i) // data for graphs 2, 3 and 4
{
  x2[i] = i/250.0*3*M_PI;
  x3[i] = x2[i];
  x4[i] = i/250.0*100-50;
  y2[i] = qSin(x2[i]*12)*qCos(x2[i])*10;
  y3[i] = qCos(x3[i])*10;
  y4[i] = 0.01*x4[i]*x4[i] + 1.5*(rand()/(double)RAND_MAX-0.5) + 1.5*M_PI;
}

// pass data points to graphs:
mean_view->graph(0)->setData(x0, y0);
mean_view->graph(1)->setData(x1, y1);
errorBars->setData(y1err);
mean_view->graph(2)->setData(x2, y2);
mean_view->graph(3)->setData(x3, y3);
mean_view->graph(4)->setData(x4, y4);
// activate top and right axes, which are invisible by default:
mean_view->xAxis2->setVisible(true);
mean_view->yAxis2->setVisible(true);
// set ranges appropriate to show data:
mean_view->xAxis->setRange(0, 2.7);
mean_view->yAxis->setRange(0, 2.6);
mean_view->xAxis2->setRange(0, 3.0*M_PI);
mean_view->yAxis2->setRange(-70, 35);
// set pi ticks on top axis:
mean_view->xAxis2->setTicker(QSharedPointer<QCPAxisTickerPi>(new QCPAxisTickerPi));
// add title layout element:
mean_view->plotLayout()->insertRow(0);
mean_view->plotLayout()->addElement(0, 0, new QCPTextElement(mean_view, "Way too many graphs in one plot", QFont("sans", 12, QFont::Bold)));
// set labels:
mean_view->xAxis->setLabel("Bottom axis with outward ticks");
mean_view->yAxis->setLabel("Left axis label");
mean_view->xAxis2->setLabel("Top axis label");
mean_view->yAxis2->setLabel("Right axis label");
// make ticks on bottom axis go outward:
mean_view->xAxis->setTickLength(0, 5);
mean_view->xAxis->setSubTickLength(0, 3);
// make ticks on right axis go inward and outward:
mean_view->yAxis2->setTickLength(3, 3);
mean_view->yAxis2->setSubTickLength(1, 1);
mean_view->replot();

*/





}

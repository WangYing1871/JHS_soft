#ifndef mainwindow_H
#define mainwindow_H 1 

#include <iostream>
#include <map>
#include <string>

#include "ui_mainwindow.h"
#include "QMainWindow"
#include "qcustomplot.h"
#include "unpack.h"

class mainwindow : public QMainWindow{
  Q_OBJECT

public:
  mainwindow();


public slots:
  void test_plot();

  void choose_dat_file();
  void unpack();
  void draw_wave(int);
  void quit();
  void abort();

private:
  Ui::JHS_GUI ui;
  std::string m_fname;
  std::string m_last_dir="/home";


};

#endif

//#include <filesystem>
//{
//  mean_view->setLocale(QLocale(QLocale::English,QLocale::UnitedKingdom));
//  mean_view->legend->setVisible(true);
//  QFont legendFont = font();
//  legendFont.setPointSize(9);
//  mean_view->legend->setFont(legendFont);
//  mean_view->length->setBrush(QBrush(QColor(255,255,255,230)));
//  mean_view->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom|Qt::AlignRight);
//  mean_view->addGraph(mean_view->yAxis,mean_view->xAxis);
//  mean_view->graph(0)->setPen(QPen(QColor(255,100,0)));
//  if (std::filesystem::exists("JW.png")){
//    mean_view->graph(0)->setBrush(QBrush(QPixmap("JW.png")));
//  }
//  mean_view->graph(0)->setLineStyle(QCPGraph::lsLine);
//  mean_view->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));
//  mean_view->graph(0)->setName("Left maxwell function");
//
//  mean_view->addGraph();
//  auto grp1 = mean_view->graph(1);
//  grp1->setPen(QPen(Qt::red));
//  grp1->setBrush(QBrush(QPixmap("JW.png")));
//  grp1->setLineStyle(QCPGraph::lsStepCenter);
//  grp1->setScatterStyle(QCPScatterStyle(
//        QCPScatterStyle::ssCircle
//        ,Qt::red
//        ,Qt::white
//        ,7
//        ));
//  grp1->setName("g1");
//  QCPErrorBars* errorBars = new QCPErrorBars(mean_view->xAxis,mean_view->yAxis);
//  errorBars->removeFromLegend();
//  errorBars->setDataPlottable(grp1);
//
//
//  mean_view->addGraph(mean_view->xAxis2,mean_view->yAxis2);
//  auto grp2 = mean_view->graph(2);
//  grp2->setPen(QPen(Qt::blue));
//  grp2->setName("High frequency sine");
//
//  mean_view->addGraph(mean_view->xAxis2,mean_view->yAxis2);
//  auto grp3 = mean_view->graph(3);
//  QPen blueDotPen;
//  blueDotPen.setColor(QColor(30,40,255,150));
//  blueDotPen.setStyle(Qt:;DotLine);
//  blueDotPen.setWidthF(4);
//  grp3->setPen(blueDotPen);
//  grp3->setName("Sine envelope");
//
//  mean_view->addGraph(mean_view->yAxis2,mean_view->xAxis2);
//  auto grp4 = mean_view->graph(4);
//  grp4->setPen(QColor(50,50,50,255));
//  grp4->setLineStyle(QCPGraph::lsNone);
//  grp4->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,4));
//  grp4->setName("...\n");
//
//  QVector<double> x0(25), y0(25);
//  QVector<double> x1(15), y1(15), y1err(15);
//  QVector<double> x2(250), y2(250);
//  QVector<double> x3(250), y3(250);
//  QVector<double> x4(250), y4(250);
//
//  for (int i=0; i<25; ++i){
//    x0[i] = 3*i/25.0;
//    y0[i] = qExp(-x0[i]*x0[i]*0.8)*(x0[i]*x0[i]+x0[i]);
//  }
//
//  for (int i=0; i<15; ++i){
//    x1[i] = 3*i/15.0;
//  }
//
//  mean_view->graph(1)->setData(x1,y1);
//  errorBars->setData(y1err);
//
//
//
//
//
//
//
//}

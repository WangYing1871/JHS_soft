//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: wy187110@mail.ustc.edu.cn     
//^-^ Time: 2024-10-31 16:07:30   Posi: Hefei
//^-^ File: draw_hitmap.cpp
//--------------------------------------------------------------------
#ifndef info_out
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#endif
#include <algorithm>
#if __cplusplus >= 201703L
# include <numeric>
#endif
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TH2I.h"
#include "TGraph.h"
#include "TFolder.h"
#include "TClonesArray.h"
#include "data_strcut_cint.h"

std::map<int,std::vector<uint16_t>> pix_mapx, pix_mapy;

static double const strip_width = 0.4; /*400um*/

double distance(double k, double b
    ,double x, double y){
  return std::abs(k*x-y+b)/std::sqrt(1+k*k);
}
 
TGraph* track_2d(cluster* cl,std::string const& name="track"){
  TGraph* ret = new TGraph;
  ret->SetTitle(name.c_str());
  for (int i=0; i<cl->amp_time.size(); ++i)
    ret->SetPoint(i,cl->range.first+i,cl->amp_time[i]);
  return ret;
}
double cog(cluster* cl){
  auto& range = cl->range;
  auto& amp = cl->amp;
  size_t sum_adc = 0;
  size_t sum = 0;
  for (int i=range.first, index=0; i<range.second; ++i, ++index){
    sum_adc += amp[index];
    double strip_pos = strip_width*i;
    sum += strip_pos*amp[index];
  }
  return sum_adc==0 ? std::nan("") : sum/(double)sum_adc;
}

double cog_time(cluster* cl){
  auto& amp_time = cl->amp_time;
  auto& amp = cl->amp;
  size_t sum_adc = 0;
  int64_t sum = 0;
  for (int i=0; i<amp.size(); ++i){
    sum_adc += amp[i];
    sum += amp[i]*amp_time[i];
  }
  return sum_adc==0 ? std::nan("") : sum/(double)sum_adc;
}

int main(int argc, char* argv[]){
  std::string fname = argv[1];
  auto* fin = new TFile(fname.c_str());
  auto* tree = (TTree*)(fin->Get("CollectionTree"));
  auto* clusters_x = new TClonesArray("cluster");
  auto* clusters_y = new TClonesArray("cluster");
  tree->SetBranchAddress("clusters_x",&clusters_x);
  tree->SetBranchAddress("clusters_y",&clusters_y);

  auto* hisx = new TH1I("spectrumx","spectrumx",2500,0,10000);
  auto* hisxx = new TH1I("spectrumxx","spectrumxx",2500,0,10000);
  auto* hisy = new TH1I("spectrumy","spectrumy",2500,0,10000);
  auto* hisxy = new TH1I("spectrum","spectrum",2500,0,10000);
  auto* hitmap = new TH2D("hitmap","hitmap",3750,0,375,3750,0,375);
  //hitmap->SetStats(0);
  auto* hitsVsc = new TH2I("HitsVsCharge","HitsVsCharge",30,0,30,10000,0,10000);
  TCanvas* c1 = new TCanvas("c1","c1",1000,800);
  c1->Divide(3,3);
  //TCanvas* spectrum_c = new TCanvas("spectrum_c","spectrum_c",1500,800);
  //spectrum_c->Divide(3,1);
  auto* sumx_clu = new TH1I("sumx_clu","sumx_clu",2500,0,10000);
  auto* sumy_clu = new TH1I("sumy_clu","sumy_clu",2500,0,10000);
  auto* sum_clu = new TH1I("sum_clu","sum_clu",5000,0,20000);

  TFile* fout = new TFile("draw_hitmap.root","recreate");
  long long entries = tree->GetEntries();
  std::cout<<"entries: "<<entries<<std::endl;
  std::size_t index=0;

  std::vector<std::pair<double,double>> hit_points;
  TH1D* hit_onlyx = new TH1D("hit_onlyx","hit_onlyx",3750,0,3750);
  TH1D* hit_onlyy = new TH1D("hit_onlyy","hit_onlyy",3750,0,3750);
  auto* xz_f = new TFolder("XZ","XZ");
  auto* yz_f = new TFolder("YZ","YZ");
  for (long long i=0; i<entries; ++i){
    clusters_x->Clear();
    clusters_y->Clear();
    tree->GetEntry(i);
    size_t sx = clusters_x->GetEntries();
    size_t sy = clusters_y->GetEntries();

    std::cout<<"sx,sy "<<sx<<","<<sy<<std::endl;
    //info_out(sx);
    //info_out(sy);
    uint32_t amp_sum_max = 0;
    if (sx==1 && sy==1){
      index++;
      //for (int j=0; j<sx; ++j){
      //  cluster* cx_buf = (cluster*)clusters_x->At(j);
      //  if (cx_buf->sum_amp>)
      //}
      cluster* cx = (cluster*)clusters_x->At(0);
      cluster* cy = (cluster*)clusters_y->At(0);

      for (int j=cx->range.first, index=0; j<cx->range.second; ++j,++index)
        pix_mapx[j].emplace_back(cx->amp[index]);
      for (int j=cy->range.first, index=0; j<cy->range.second; ++j,++index)
        pix_mapy[j].emplace_back(cy->amp[index]);
      hisx->Fill(cx->sum_amp);
      hisy->Fill(cy->sum_amp);
      hisxy->Fill(cx->sum_amp+cy->sum_amp);
      double hit_x = cog(cx);
      double hit_y = cog(cy);
      hitmap->Fill(hit_x,hit_y);
      hit_points.emplace_back(std::make_pair(hit_x,hit_y));
      
      size_t strips = cx->strips+cy->strips;
      size_t tot_edep = cx->sum_amp+cy->sum_amp;
      hitsVsc->Fill(strips,tot_edep);
      //if (i<100){ 
      //  std::stringstream sstr("");
      //  sstr<<"Event:"<<i;
      //  fout->cd();
      //  track_2d(cx,sstr.str())->Write(), track_2d(cy,sstr.str())->Write();
      //}
    }
    if (sx==1){
      cluster* cx = (cluster*)clusters_x->At(0);
      hisxx->Fill(cx->sum_amp);
    }
    uint32_t sum_adcx = 0;
    uint32_t sum_adcy = 0;
    if (sx>=1){
      auto* grp = new TGraph();
      std::stringstream sstr("");
      sstr<<"Event-"<<i<<"-XZ";
      grp->SetName(sstr.str().c_str());
      for (int j=0; j<sx; ++j){
        cluster* cx = (cluster*)clusters_x->At(j);
        sum_adcx += cx->sum_amp;
        double hitx = cog(cx);
        double hitz = cog_time(cx);
        grp->SetPoint(j,hitx,hitz);
        hit_onlyx->Fill(hitx);
      }
      sumx_clu->Fill(sum_adcx);
      xz_f->Add(grp);
    }
    if (sy>=1){
      auto* grp = new TGraph();
      std::stringstream sstr("");
      sstr<<"Event-"<<i<<"-YZ";
      grp->SetName(sstr.str().c_str());
      for (int j=0; j<sy; ++j){
        cluster* cy = (cluster*)clusters_y->At(j);
        sum_adcy += cy->sum_amp;
        double hity = cog(cy);
        double hitz = cog_time(cy);
        grp->SetPoint(j,hity,hitz);
        hit_onlyy->Fill(hity);
      }
      sumy_clu->Fill(sum_adcy);
      yz_f->Add(grp);
    }
    sum_clu->Fill(sum_adcx+sum_adcy);


  }
  info_out(pix_mapx.size());
  info_out(pix_mapy.size());
  c1->cd(1);
  hisx->Draw();
  c1->cd(2);
  hisy->Draw();
  c1->cd(3);
  hisxy->Draw();
  c1->cd(4);
  hitmap->Draw();
  c1->cd(5);
  hitsVsc->Draw();
  c1->cd(6);
  hisxx->Draw();
  
  TH1D* hit_pos_clone = new TH1D("hit_pos_clone","hit_pos_clone",60,0,60);
  TGraph* grp = new TGraph();
  grp->SetName("hit_position");
  grp->SetTitle("hit_position");
  //TF1* linear = new TF1("linear","[0]*x+[1]",25,30,2);
  double k = argc>=3 ? std::stof(argv[2]) : -0.0738048;
  double b = argc>=4 ? std::stof(argv[3]) : 306.91;
  auto* res_his = new TH1D("residual","residual",200,-4,4);
  for (std::size_t index=0; auto&& [x,y] : hit_points){
    grp->SetPoint(index++,x,y);
    //res_his->Fill(y-(k*x+b));
    //hit_pos_clone->Fill(x,y,1);
    if ((y-(k*x+b)) >= 0)
      res_his->Fill(distance(k,b,x,y));
    else
      res_his->Fill(-distance(k,b,x,y));
  }

  //c1->cd(7);
  hit_pos_clone->Draw();
  grp->Draw("same");
 // grp->Fit(linear,"RQ");

  hit_pos_clone->Write();

  grp->Write();

  hitmap->Write();
  //res_his->Fit("gaus","RQ");
  //gStyle->SetOptFit(1111);
  res_his->Write();
  c1->Write();
  hit_onlyx->Write();
  hit_onlyy->Write();

  TFolder* pixx = new TFolder("pixx","pixx");
  TFolder* pixy = new TFolder("pixy","pixy");
  for(auto&& [x,y] : pix_mapx){
    std::stringstream sstr("");
    sstr<<"stripx-"<<x;
    auto* pix_his = new TH1F(sstr.str().c_str(),sstr.str().c_str(),4096,0,4096);
    for (auto&& y0 : y) pix_his->Fill(y0);
    pixx->Add(pix_his);
  }
  for(auto&& [x,y] : pix_mapy){
    std::stringstream sstr("");
    sstr<<"stripy-"<<x;
    auto* pix_his = new TH1F(sstr.str().c_str(),sstr.str().c_str(),4096,0,4096);
    for (auto&& y0 : y) pix_his->Fill(y0);
    pixy->Add(pix_his);
  }

  pixx->Write();
  pixy->Write();

  xz_f->Write();
  yz_f->Write();
  sumx_clu->Write();
  sumy_clu->Write();
  sum_clu->Write();

  

  fout->Write(); fout->Close();
  fin->Close();

  std::cout<<"Total Event: "<<entries<<" Draw Event: "<<index<<std::endl;

  return 0;

}

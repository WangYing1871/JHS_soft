//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: wy187110@mail.ustc.edu.cn     
//^-^ Time: 2024-10-31 15:55:10   Posi: Hefei
//^-^ File: draw_wave.cpp
//--------------------------------------------------------------------
#ifndef info_out
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#endif
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <optional>
#include <ctime>

//#include <boost/timer/timer.hpp>

#include "TTree.h"
#include "TFile.h"
#include "TFolder.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TF1.h"
#include "data_strcut_cint.h"

static std::size_t s_count=0;

//std::map<int,std::vector<uint16_t>> pix_map;
namespace serivce{
std::optional<double> fit_wave(TGraph* grp, double* prams=nullptr){
  if (!grp) return std::nullopt;
  auto const& get_range = [&](){
    std::vector<double> gdata(grp->GetN());
    double bufx, bufy;
    for (std::size_t i=0, e=grp->GetN(); i<e; ++i)
      grp->GetPoint(i,bufx,bufy), gdata[i] = bufy;
    auto pos = std::distance(std::begin(gdata)
        ,std::max_element(std::begin(gdata),std::end(gdata)));
    return std::make_pair(pos-150>=0 ? pos-150: 0, pos+10);
  };
  auto const& fit_fun = [](double* x, double* p)->double{
    double buf = (x[0]-p[2])/p[3];
    return x[0]<p[2] ? p[0] : p[0]+p[1]*std::pow(buf,3)*std::exp(-buf); };
  auto fit_range = get_range();
  static TF1 f("f",fit_fun,fit_range.first,fit_range.second,4);
  f.SetParameter(0,600); f.SetParLimits(0,580,720);
  f.SetParameter(1,300); f.SetParLimits(1,50,4095);
  f.SetParameter(2,380); f.SetParLimits(2,250,550);
  f.SetParameter(3,10); f.SetParLimits(3,1,40);
  grp->Fit(&f,"RQ");

  s_count++;
  if (prams) for (int i=0; i<4; ++i) prams[i] = f.GetParameter(i);
  return std::make_optional(f.GetChisquare()/f.GetNDF());
   
}
}

struct point_t{
  float x;
  float y;
  float z;
  //point_t(float a, float b, float c):x(a),y(b),z(c) {}
};


TFolder* waves_view(std::string const& name, entry_new* data
    ){

  auto* folder = new TFolder(name.c_str(),name.c_str());
  auto& ids = data->global_ids;
  auto& adcs = data->adcs;

  std::map<std::string,TGraph*> grps;
  for (int i=0; i<ids.size(); ++i){
    std::stringstream sstr("");
    auto layer_id = uint8_t(ids[i]>>8);
    auto channel_id = uint8_t(ids[i]&0xFF);

    sstr<<"wave"<<(int)layer_id<<"-"<<(int)channel_id;
    auto* graph = new TGraph();
    graph->SetName(sstr.str().c_str());
    graph->SetTitle(sstr.str().c_str());
    auto max_adc = *std::max_element(std::begin(adcs[i]),std::end(adcs[i]));
   // pix_map[ids[i]].emplace_back(max_adc);
    for (std::size_t index=0; auto&& x : adcs[i])
      graph->SetPoint(index,index,x), index++;
    grps[sstr.str()] = graph;
    //folder->Add(graph);
  }
  for (auto&& [x,y] : grps) folder->Add(y);
  return folder;
}
void waves_fit(entry_new* data, TH1F* his){
  //auto* folder = new TFolder(name.c_str(),name.c_str());
  auto& ids = data->global_ids;
  auto& adcs = data->adcs;
  std::map<std::string,TGraph*> grps;
  for (int i=0; i<ids.size(); ++i){
    std::stringstream sstr("");
    auto layer_id = uint8_t(ids[i]>>8);
    auto channel_id = uint8_t(ids[i]&0xFF);

    sstr<<"wave"<<(int)layer_id<<"-"<<(int)channel_id;
    auto* graph = new TGraph();
    graph->SetName(sstr.str().c_str());
    graph->SetTitle(sstr.str().c_str());
    auto max_adc = *std::max_element(std::begin(adcs[i]),std::end(adcs[i]));
   // pix_map[ids[i]].emplace_back(max_adc);
    for (std::size_t index=0; auto&& x : adcs[i])
      graph->SetPoint(index,index,x), index++;
    grps[sstr.str()] = graph;
    //folder->Add(graph);
  }
  //for (auto&& [x,y] : grps) folder->Add(y);
  for (auto&& [x,y] : grps){
    auto opt = serivce::fit_wave(y);
    if (opt) his->Fill(*opt);
  }
  //return folder;
}

template <class _iter>
void draw_graph(TGraph& g, std::string const& name, _iter begin, _iter end){
  g.SetName(name.c_str());
  g.SetTitle(name.c_str());
  int distance = std::distance(begin,end);
  for (int i=0; i<distance; ++i)
    g.SetPoint(i,i,*std::next(begin,i));
}

void demo00(uint64_t evt_id, entry_tm* data){
}


using std::cout; using std::endl; using std::string; using std::vector;
#include <filesystem>
int main(int argc, char* argv[]){
  {
    namespace fs = std::filesystem;
    if (!fs::exists("wave"))
      fs::create_directory("wave");
    std::string fname = argv[1];
    auto* fin = new TFile(fname.c_str());
    auto* tree = (TTree*)fin->Get("CollectionTree");
    entry_new* data = new entry_new;
    tree->SetBranchAddress("data",std::addressof(data));

    std::string fout_name = fs::path{fname}.filename();
    fout_name = fout_name.substr(0,fout_name.find_first_of("."));
    fout_name = "wave/"+fout_name+"_wave_view.root";
    auto* fout = new TFile(fout_name.c_str(),"recreate");
    fout->cd();

    auto entries = tree->GetEntries();
    info_out(entries);
   // TH1F* his = new TH1F("his","his",3000,0,3000);

      //boost::timer::auto_cpu_timer progress;
    //for (int i=0; i<tree->GetEntries(); ++i){
    //for (int i=0; i<1000; ++i){
    for (int i=0; i<(entries>1000 ? 1000 : entries); ++i){
    //for (int i=200; i<300; ++i){
      tree->GetEntry(i);
      std::stringstream sstr("");
      sstr<<"Event-"<<i;

      //waves_fit(data,his);
      
      waves_view(sstr.str(),data)->Write();

      //if (i%(int)(entries*0.05)==0) info_out("PS");
    }
    //his->SaveAs("temp.root");
    fout->Write(); fout->Close();
    fin->Close();
    return 0;
  }
  //{
  //  TFile* file = new TFile("temp.root","recreate");
  //  file->cd();
  //  for (int i=0; i<100000; ++i){
  //    std::stringstream sstr;
  //    sstr<<"his"<<i;
  //    TH1F f1(sstr.str().c_str(),sstr.str().c_str(),1000,-5,5);
  //    f1.FillRandom("gaus",10000);
  //    f1.Write();
  //    if (i%1000==0){
  //      info_out(std::filesystem::file_size("temp.root"));
  //    }
  //  }
  //  file->Write();
  //  file->Close();
  //}
  {
    std::string fname = argv[1];
    auto* fin = new TFile(fname.c_str());
    auto* tree = (TTree*)fin->Get("CollectionTree");
    auto* data = new entry_tm;
    tree->SetBranchAddress("data",std::addressof(data));
    info_out(tree->GetEntries());
    auto* fout = new TFile("wave.root","recreate");
    fout->cd();
    std::map<std::string,TGraph> grp_map;
    for (long long i=0; i<tree->GetEntries(); ++i){
      grp_map.clear();
      tree->GetEntry(i);
      std::stringstream sstr("");
      sstr<<"Event"<<i;
      TFolder folder(sstr.str().c_str(),sstr.str().c_str());
      for (std::size_t index=0; auto&& x : data->det_ids){
        sstr = std::stringstream("");
        sstr<<"wave-"<<(x>>16)<<"-"<<(x&0xFFFF);
        auto& adc = data->adcs[index++];
        TGraph g;
        draw_graph(g,sstr.str(),std::begin(adc),std::end(adc));
        //g.Write();
        grp_map.emplace(sstr.str(),g);
      }
      for (auto&& [x,y] : grp_map) folder.Add(&y);
      folder.Write();
      if (i%1000==0)
        info_out(std::filesystem::file_size("wave.root"));
    }
    fout->Write();
    fout->Close();



  }
  return 0;
}

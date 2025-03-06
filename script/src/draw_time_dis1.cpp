//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: wy187110@mail.ustc.edu.cn     
//^-^ Time: 2024-11-20 16:15:18   Posi: Hefei
//^-^ File: draw_time_dis.cpp
//--------------------------------------------------------------------
//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng           Email: 2835516101@qq.com     
//^-^ Time: 2025-02-15 15:44:47   Posi: JW company@AnHui Soft.D
//^-^ File: ../src/draw_time_dis1.cpp
//--------------------------------------------------------------------
//
//Same as 'draw_time_dis' //XXX
#ifndef info_out
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#endif
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <optional>
#include <filesystem>
#include <type_traits>
#include <random>
#include <set>
#include <future>

#include "TFile.h"
#include "TTree.h"
#include "TFolder.h"
#include "TH2D.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h" 
#include "TClonesArray.h"

#include "util.hpp"
#include "util.h"
#include "data_strcut_cint.h"
#include "clusters.hpp"

namespace constant{
constexpr static const std::size_t s_det_strips_x = 320;
constexpr static const std::size_t s_det_strips_y = 320;
constexpr static const std::size_t s_max_hit_strips = 200;
constexpr static const std::size_t s_min_hit_strips = 3;
}

static std::default_random_engine s_reng;
//static clusters<uint16_t> s_clusters_x;
//static clusters<uint16_t> s_clusters_y;
namespace util{
void same_grp_min_max(TGraph* x, TGraph* y){
  auto min = std::min(x->GetYaxis()->GetXmin(), y->GetYaxis()->GetXmin());
  auto max = std::max(x->GetYaxis()->GetXmax(), y->GetYaxis()->GetXmax());
  x->GetYaxis()->SetRangeUser(min,max);
  y->GetYaxis()->SetRangeUser(min,max);

}
template <class _tp>
void make_t_grpah(TGraph*& grp, _tp first, _tp last){
  grp = new TGraph();
  std::size_t index=0;
  for (auto iter = first; iter != last; ++iter){ grp->SetPoint(index,index,*iter); ++index; }
  //grp->SaveAs("temp.root");
}
}
template <class _tp>
void clear_object(_tp& d){
  if constexpr (std::is_same<_tp, entry_tm>::value){
    d.start_times.clear();
    d.adcs.clear();
    d.det_ids.clear();
  }else if constexpr(std::is_same<_tp,entry_strip>::value){
    d.layer_id = (std::uint8_t)(-1);
    d.dim_id = (std::uint8_t)(-1);
    d.channel_id = (std::uint16_t)(-1);
    d.p0 = d.p1 = d.p2 = d.p3 = 0.;
    d.raw_evt_id = 0;
    
  }
}

typedef std::map<uint64_t
  ,std::unordered_map<uint16_t,std::vector<uint16_t>>> in_memory_map_t;
void load_in_momery(in_memory_map_t& mp, uint64_t evt_id, entry_new* data){
  if (mp.find(evt_id)!=mp.end()) return;
  mp[evt_id];
  for (std::size_t index=0; auto&& x : data->global_ids){
    mp.at(evt_id).emplace(x,data->adcs[index++]);
  }
}

struct strip_info_t{
  float mean=0.;
  float sigma=0.;
  uint16_t max;
  uint16_t dirft_time;
};

struct point_t{
  float x;
  float y;
  float z;
};

struct point_t_comparer{
  typedef bool result_t;
  result_t operator()(point_t const& a, point_t const& b) const{
    return a.x<b.x; } };


TH2F* draw_track(std::vector<point_t> const& data
    , std::string const& name){
  //FIXME
  auto* ret = new TH2F(name.c_str(),name.c_str()
      ,320,0,320,250,0,2500);
  ret->SetStats(false);
  ret->SetDirectory(nullptr);
  for (auto&& x : data) ret->Fill(x.x,x.y,x.z);
  return ret;
}

TGraph* draw_2d_track(std::vector<point_t> const& data
    ,std::string const& name){
  std::set<point_t,point_t_comparer> pts;
  for (auto&& x : data) pts.insert(x);
  TGraph* grp = new TGraph();
  grp->SetName(name.c_str());
  grp->SetTitle(name.c_str());
  grp->SetLineColor(kBlue);
  grp->SetLineWidth(2);
  grp->SetMarkerSize(0.5);
  grp->SetMarkerStyle(4);
  grp->GetXaxis()->SetTitle("Strip ID");
  grp->GetYaxis()->SetTitle("Relative Drift Time");
  for (std::size_t index=0; auto&& x : pts) grp->SetPoint(index++,x.x,x.y);
  return grp;
}

struct layer_hits_t{
  typedef uint16_t hit_info_t;
  std::vector<uint16_t> m_strip_dis_x, m_strip_dis_y;
  std::vector<point_t> m_px, m_py;
  clusters<hit_info_t> m_clusterx, m_clustery;
  layer_hits_t(){
    m_clusterx.s_set_tolerate(30);
    m_clustery.s_set_tolerate(30);
    m_strip_dis_x.resize(s_strips_per_dim_x);
    m_strip_dis_y.resize(s_strips_per_dim_y);
  }
  ~layer_hits_t() noexcept = default;

  void find_cluster(){
    m_clusterx.calc(std::begin(m_strip_dis_x)
        ,std::end(m_strip_dis_x)
        ,[](uint16_t v){return v>0;}
        ,[](uint16_t v){return v;});
    m_clusterx.hadd();
    m_clustery.calc(std::begin(m_strip_dis_y)
        ,std::end(m_strip_dis_y)
        ,[](uint16_t v){return v>0;}
        ,[](uint16_t v){return v;});
    m_clustery.hadd();
  }
  std::vector<TH2F*> draw_tracks(std::string const& name){
    std::vector<TH2F*> ret;
    if (m_clusterx.number()==1 
        && m_clustery.number()==1
        /* FIXME TODO */
        && (m_clusterx.at(0).size()>10 || m_clustery.at(0).size()>10)
        ){
      std::stringstream sstr("");
      sstr<<name<<"-XZ";
      ret.emplace_back(draw_track(m_px,sstr.str()));
      sstr = std::stringstream("");
      sstr<<name<<"-YZ";
      ret.emplace_back(draw_track(m_py,sstr.str()));
    }
    return ret;
  }
  std::vector<TGraph*> draw_2d_tracks(std::string const& name){
    std::vector<TGraph*> ret;
    if (m_clusterx.number()==1 && m_clustery.number()==1){
      std::stringstream sstr("");
      sstr<<name<<"-XZ";
      ret.emplace_back(draw_2d_track(m_px,sstr.str()));
      sstr = std::stringstream("");
      sstr<<name<<"-YZ";
      ret.emplace_back(draw_2d_track(m_py,sstr.str()));
    }
    return ret;
  }

  //temporary function
  std::optional<std::pair<double,double>> hit_position() const{
    if (m_px.size()>=3 && m_py.size()>=3){
      auto iterx_max = std::max_element(std::begin(m_px),std::end(m_px)
          ,[](auto a, auto b){return a.y<b.y;});
      auto itery_max = std::max_element(std::begin(m_py),std::end(m_py)
          ,[](auto a, auto b){return a.y<b.y;});
      return std::make_optional(
          std::make_pair((*iterx_max).x,(*itery_max).x));
    }
    return std::nullopt;
  }

  void clear() { 
    m_px.clear(), m_py.clear();
    std::fill(std::begin(m_strip_dis_x),std::end(m_strip_dis_x),0);
    std::fill(std::begin(m_strip_dis_y),std::end(m_strip_dis_y),0);
  }

  static constexpr std::size_t const s_strips_per_dim_x = 320;
  static constexpr std::size_t const s_strips_per_dim_y = 320;
};
template <class _iter>
void draw_graph(TGraph& g, std::string const& name, _iter begin, _iter end){
  g.SetName(name.c_str());
  g.SetTitle(name.c_str());
  int distance = std::distance(begin,end);
  for (int i=0; i<distance; ++i) g.SetPoint(i,i,*std::next(begin,i)); }

void draw_wave(uint64_t evt_id, entry_tm* data, TFolder* pp=nullptr){
  if (!data) return;
  std::map<std::string,TGraph> grp_map; grp_map.clear();
  std::stringstream sstr("");
  sstr<<"Event"<<evt_id;
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
  if (pp) { pp->Add(&folder); pp->Write();}
  else folder.Write();
}


using std::cout; using std::endl;
using std::string; using std::vector;

TFolder* waves_view(std::string const& name, entry_new* data){
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




TGraph* draw_track_grp(){
  return nullptr;
}

//---------------------------------------------------------------------
//temporary !!!!
#include "TF1.h"
#include "TF2.h"
#include <optional>
#include <regex>
#include <unordered_map>
namespace service{
bool add_points(point_t* pnts, std::size_t sz, TH1F* his){
  if (sz<4) return false;
  auto const& sample = [=](std::size_t number){
    std::set<std::size_t> cna;
    std::uniform_int_distribution<std::size_t> di(0,sz-1);
    while(cna.size()!=number) cna.insert(di(s_reng));
    return cna; };
  std::vector<float> px, py;
  std::vector<float> sps_px, sps_py;
  auto sp = sample(2);
  for(int i=0; i<sz; ++i){
    if (sp.find(i)!=sp.end()) 
      sps_px.emplace_back(pnts[i].x), sps_py.emplace_back(pnts[i].y);
    else px.emplace_back(pnts[i].x), py.emplace_back(pnts[i].y);
  }
  auto kb = util::least_squart_line_fit(
      std::begin(px), std::end(px)
      ,std::begin(py), std::end(py)
      );
  if (kb){
    double k = kb->first; double b = kb->second;
    for (std::size_t i=0; i<sps_px.size(); ++i)
      his->Fill((sps_px.at(i)-((sps_py.at(i)-b)/k))*std::sqrt(k*k/(1+k*k)));
    return true;
  }
  return false;
}

 

}
//--------------------------------------------------------------------

//--------------------------------------------------------------------


int main(int argc, char* argv[]){
  if (argc<3) throw std::invalid_argument("...");
  std::string fname = argv[1];
  std::string baseline_name = argv[2];
  std::string fec2det_name = argv[4];
  std::cout<<"fname: "<<fname<<std::endl;
  std::cout<<"baseline_name: "<<baseline_name<<std::endl;
  util::id_trans id_to_det(fec2det_name);
  info_out(fec2det_name);
  if (!std::filesystem::exists(fname.c_str())
      || !std::filesystem::exists(baseline_name.c_str()))
    throw std::invalid_argument("... file no exists");
  std::map<std::pair<int,int>,std::pair<float,float>> baseline_map;
  std::ifstream prestal_fin(baseline_name.c_str());
  while(!prestal_fin.eof()){
    std::string sbuf;
    std::getline(prestal_fin,sbuf);
    if(!sbuf.empty() && sbuf[0] != '#'){
      std::stringstream sstr(sbuf.c_str());
      int a, d; float b, c,e;
      sstr>>a>>d>>b>>c>>e;
      baseline_map[std::make_pair(a,d)]=std::make_pair(b,c);
    }
  }
  std::cout<<baseline_map.size()<<std::endl;
  auto* fin = new TFile(fname.c_str());
  auto* tree = (TTree*)fin->Get("CollectionTree");
  entry_new* data = new entry_new;
  tree->SetBranchAddress("data",std::addressof(data));
  typedef struct{
    uint16_t max;
    uint16_t max_postion;
    uint64_t evt_id;
    uint64_t time_stamp;
    int id;
    bool is_valid = false;
    double fit_parameter[4] = {0.,0.,0.,0.};
    bool is_fit = false;
    double fit_cdn;
  }max_id_t;

  std::multimap<double,max_id_t> tm_vs_adc;
  entry_tm data_tm;
  entry_strip data_strip;

  util::t_object_factory_v0 t_object_factory{};
  t_object_factory
    .register_element<TH1F>("residualx",1000,-5,5)
    .register_element<TH1F>("residualy",1000,-5,5)
    .register_element<TH1F>("residualx_L0",1000,-5,5)
    .register_element<TH1F>("residualy_L0",1000,-5,5)
    .register_element<TH2F>("HitMap_L0",320,0,320,320,0,320)
    .register_element<TH2F>("HitMap_L1",320,0,320,320,0,320)
    .register_element<TH1F>("spectrum_L0",4000,0,40000)
    .register_element<TH1F>("spectrum_L1",4000,0,40000)
    .register_element<TH1F>("spectrum",8000,0,80000)
    //.register_element<TH2F>("timevsadc",2000,0,20000)
    ;

  std::string out_file_name = fname.substr(0,fname.find_last_of("_")+1)+"fit.root";
  TFile* rfout = new TFile(out_file_name.c_str(),"recreate");
  TTree* data_det_tree = new TTree("CollectionTree","CollectionTree");
  int evt_no = 0;
  data_det_tree->Branch("event_id",std::addressof(evt_no)); 
  auto* strip_hits = new TClonesArray("entry_strip");
  data_det_tree->Branch("hit_strips",std::addressof(strip_hits),256000,0);
  strip_hits->BypassStreamer();

  TH1F spectrum("spectrum","spectrum",2000,0,20000);
  spectrum.SetDirectory(nullptr);
  std::multimap<int,std::string> entries_table;
  std::ofstream fout("timevsadc.txt");

  float compres = 2.;
  uint32_t ww = std::stoi(argv[3]);
  std::cout<<argv[3]<<std::endl;
  std::cout<<"ww: "<<ww<<std::endl;

  TFolder* track_fl = new TFolder("track","track");
  TFolder* wave_fl = new TFolder("wave","wave");
  auto entries = tree->GetEntries();
  info_out(entries);

  //in_memory_map_t in_memory_map;
  std::unordered_map<int16_t,layer_hits_t> layers_hits;
  layers_hits[0];
  layers_hits[1];

  rfout->cd();

  TGraph* grp_buf=nullptr;
  double fit_para[4] = {-1,-1,-1,-1};
  double const buf_distance = 5000.;

  double at_rate = 0.2;
  double at_buf;

  //ui::Xslider slider(0,entries);
  //slider.set_label("fit wave");
  //auto __attribute__((unused)) ft0 = std::async(&ui::Xslider::start,std::ref(slider));

  //entries = entries>100 ? 1000 : entries;

  for (int j=0; j<std::ceil(entries/buf_distance); ++j){
    int e_start = j*buf_distance;
    int e_end = (j+1)*buf_distance > entries ? entries : (j+1)*buf_distance;
    std::cout<<"progress: "<<j*buf_distance<<"/"<<entries<<"\n";

    for (int i=e_start; i<e_end; ++i){
      //slider.progress(j*buf_distance+i);
      tree->GetEntry(i);
      std::map<int,uint64_t> ts_map;
      for (std::size_t index=0; auto&& x : data->fec_ids)
        ts_map[x] = data->time_stamps[index++];
      info_out("................");
      for (std::size_t index=0; auto&& x : data->global_ids){
        auto const& adcs = data->adcs[index++];
        int fec_id = x>>8;
        int channel_id = x&0xFF;
        auto ms = baseline_map.at(std::make_pair(fec_id,channel_id));
        if (ts_map.find(fec_id)==ts_map.end()) continue;
        uint64_t ts = ts_map.at(fec_id);
        max_id_t max_id;
        max_id.is_valid = wave_filter(adcs,ms,3,5,2.5,50,4);
        max_id.id = x;
        max_id.evt_id = i;
        if (max_id.is_valid){
          auto iter = std::max_element(std::begin(adcs),std::end(adcs));
          max_id.max = *iter;
          max_id.max_postion = std::distance(std::begin(adcs),iter);
          max_id.time_stamp = ts;
          util::make_t_grpah(grp_buf,std::begin(adcs),std::end(adcs));
          auto fit_result = util::fit_wave(grp_buf,fit_para,at_rate,at_buf);
          std::cout<<j*buf_distance+i<<" "<<fec_id<<" "<<channel_id
            <<";"<<fit_para[0]
            <<";"<<fit_para[1]
            <<";"<<fit_para[2]
            <<";"<<fit_para[3]
            <<" "<<at_buf
            <<std::endl;
          delete grp_buf;
          //grp_buf->Write();
          //double arrive_time = 0;
          double arrive_time = std::round(ts*8.33)+(at_buf-624.)*25.;
          std::copy(fit_para,fit_para+4,max_id.fit_parameter);
          max_id.is_fit = fit_result.first;
          max_id.fit_cdn = fit_result.second;

          //auto opt_fit = util::fit_wave(grp_buf,fit_para);
          ////std::cout<<(int)fec_id<<" "<<(int)channel_id<<" "<<*opt_fit<<"\n";
          //if (opt_fit && *opt_fit<1000){
          //  std::copy(fit_para,fit_para+4,max_id.fit_parameter);
          //  fit_para[0] = fit_para[1] = fit_para[2] = fit_para[3] = -1.;
          //}
          tm_vs_adc.emplace(arrive_time,max_id);
        }
      }
      //info_out("==========================");
    }
    //std::cout<<"timevsadc: "<<tm_vs_adc.size()<<std::endl;

    auto iter_prev = tm_vs_adc.begin();
    for (auto iter = tm_vs_adc.begin(); iter != tm_vs_adc.end();){
      //fout<<iter->first<<" "<<iter->second.max<<"\n";
      //if (iter->first/1.e+9<200.){
      // t_object_factory.get<TH2F>("timevsadc")->Fill(iter->first/1.e+9,iter->second.max);
      //}  
      auto id = iter->second.id;
      auto iter_next = std::next(iter);
      float sum_adc = 0.f, sum_adc_l0 = 0.f, sum_adc_l1 = 0.f;
      if (iter_next->first-iter->first>ww
          && std::distance(iter_prev,iter_next)>=constant::s_min_hit_strips
          && std::distance(iter_prev,iter_next)<=constant::s_max_hit_strips
          ){
        //info_out(std::distance(iter_prev,iter_next));
        int hit_count=0;
        for (auto iter_tmp = iter_prev; iter_tmp!=iter_next; ++iter_tmp){
          if (iter_tmp->second.is_valid){
            id = iter_tmp->second.id;
            auto ms = baseline_map.at(std::make_pair(id>>8,id&0xFF));
            if (iter_tmp->second.max<=(ms.first+compres*ms.second)) continue;
            sum_adc += iter_tmp->second.max - ms.first;
            auto det_id = id_to_det.tran(std::make_pair(id>>8,id&0xFF));
            data_strip.layer_id = det_id.m_layer_id;
            data_strip.dim_id = det_id.m_dimension_id;
            data_strip.channel_id = det_id.m_channel_id;
            data_strip.p0 = iter_tmp->second.fit_parameter[0];
            data_strip.p1 = iter_tmp->second.fit_parameter[1];
            data_strip.p2 = iter_tmp->second.fit_parameter[2];
            data_strip.p3 = iter_tmp->second.fit_parameter[3];
            data_strip.time_stamp = iter_tmp->second.time_stamp;
            data_strip.raw_evt_id = iter_tmp->second.evt_id;
            data_strip.peak = iter_tmp->second.max;
            data_strip.peak_position = iter_tmp->second.max_postion;
            data_strip.mean = ms.first;
            data_strip.sigma = ms.second;
            //data_strip.is_fit = iter_tmp-

            //std::cout
            //  <<data_strip.p0<<" "
            //  <<data_strip.p1<<" "
            //  <<data_strip.p2<<" "
            //  <<data_strip.p3<<" "
            //  <<std::endl;


            //std::cout
            //  <<data_strip.p0<<" " 
            //  <<data_strip.p1<<" "
            //  <<data_strip.p2<<" "
            //  <<data_strip.p3<<" "
            //  <<"\n";
            new (strip_hits->operator[](hit_count++)) entry_strip(data_strip);
              
            //auto evt_id = iter_tmp->second.evt_id;

            //if (in_memory_map.find(evt_id)==in_memory_map.end()){
            //  tree->GetEntry(evt_id);
            //  load_in_momery(in_memory_map,evt_id,data);
            //}
            if (det_id.m_layer_id==0){
              sum_adc_l0 += iter_tmp->second.max - ms.first;
              if (det_id.m_dimension_id==0){
                layers_hits.at(0).m_strip_dis_x[det_id.m_channel_id] = iter_tmp->second.max;
                point_t pt;
                pt.x = det_id.m_channel_id;
                pt.y = iter_tmp->first-iter_prev->first;
                pt.z = iter_tmp->second.max;
                layers_hits.at(0).m_px.emplace_back(pt);
              }else if (det_id.m_dimension_id==1){
                layers_hits.at(0).m_strip_dis_y[det_id.m_channel_id] = iter_tmp->second.max;
                point_t pt;
                pt.x = det_id.m_channel_id;
                pt.y = iter_tmp->first-iter_prev->first;
                pt.z = iter_tmp->second.max;
                layers_hits.at(0).m_py.emplace_back(pt);
              }
            }
            else if (det_id.m_layer_id==1){
              //info_out("l1");
              sum_adc_l1 += iter_tmp->second.max - ms.first;
              if (det_id.m_dimension_id==0){
                layers_hits.at(1).m_strip_dis_x[det_id.m_channel_id] = iter_tmp->second.max;
                point_t pt;
                pt.x = det_id.m_channel_id;
                pt.y = iter_tmp->first-iter_prev->first;
                pt.z = iter_tmp->second.max;
                layers_hits.at(1).m_px.emplace_back(pt);
              }else if (det_id.m_dimension_id==1){
                layers_hits.at(1).m_strip_dis_y[det_id.m_channel_id] = iter_tmp->second.max;
                point_t pt;
                pt.x = det_id.m_channel_id;
                pt.y = iter_tmp->first-iter_prev->first;
                pt.z = iter_tmp->second.max;
                layers_hits.at(1).m_py.emplace_back(pt);
              }
            }
            //if (in_memory_map.size()>=20)
            //  in_memory_map.erase(std::begin(in_memory_map)
            //      ,std::next(std::begin(in_memory_map),10));
          }
        }
        //info_out(hit_count);
        if (sum_adc_l0>0){
          auto& ref = layers_hits.at(0);
          auto pos = ref.hit_position();
          if (pos) (*t_object_factory.get<TH2F>("HitMap_L0"))->Fill(pos->first,pos->second);
          if (ref.m_px.size()>=5 && ref.m_py.size()>=5){
            service::add_points(
                ref.m_px.data()
                ,ref.m_px.size()
                ,*t_object_factory.get<TH1F>("residualx_L0"));
            service::add_points(
                ref.m_py.data()
                ,ref.m_py.size()
                ,*t_object_factory.get<TH1F>("residualy_L0"));
          }
        }
        if (sum_adc_l0>0 && evt_no<400){
          std::stringstream sstr("");
          sstr<<"Event-"<<evt_no<<"-Layer0";
          TCanvas* canvas = new TCanvas(sstr.str().c_str(),sstr.str().c_str(),1500,900);
          canvas->Divide(2,2);
          canvas->cd();
          sstr = std::stringstream(""); sstr<<"Event-L0-"<<evt_no;
          layers_hits.at(0).find_cluster();
          auto tracks_l0 = layers_hits.at(0).draw_tracks(sstr.str());

          if (tracks_l0.size()==2){
            canvas->cd(1); tracks_l0[0]->Draw("colz");
            canvas->cd(2); tracks_l0[1]->Draw("colz");

            //std::cout<<"Track0: "
            //  <<tracks_l0[0]->GetEntries()<<" "
            //  <<tracks_l0[1]->GetEntries()<<" "
            //  <<"\n";

            auto grps = layers_hits.at(0).draw_2d_tracks(sstr.str());
            util::same_grp_min_max(grps.at(0),grps.at(1));
            canvas->cd(3); grps[0]->Draw();
            canvas->cd(4); grps[1]->Draw();
          }
          if (tracks_l0.size()) track_fl->Add(canvas);
        }
        if (sum_adc_l1>0){
          auto& ref = layers_hits.at(1);
          auto pos = ref.hit_position();
          if (pos) (*t_object_factory.get<TH2F>("HitMap_L1"))->Fill(pos->first,pos->second);
          if (ref.m_px.size()>=5 && ref.m_py.size()>=5){
            service::add_points(
                ref.m_px.data()
                ,ref.m_px.size()
                ,*t_object_factory.get<TH1F>("residualx"));
            service::add_points(
                ref.m_py.data()
                ,ref.m_py.size()
                ,*t_object_factory.get<TH1F>("residualy"));
          }
        }
        if (sum_adc_l1>0 && evt_no<400){
          std::stringstream sstr("");
          sstr<<"Event-"<<evt_no<<"-Layer1";
          TCanvas* canvas = new TCanvas(sstr.str().c_str(),sstr.str().c_str(),1500,900);
          canvas->Divide(2,2);
          canvas->cd();
          sstr = std::stringstream(""); sstr<<"Event-L1-"<<evt_no;
          layers_hits.at(1).find_cluster();
          auto tracks_l1 = layers_hits.at(1).draw_tracks(sstr.str());
          auto pos = layers_hits.at(1).hit_position();
          if (pos) (*t_object_factory.get<TH2F>("HitMap_L1"))->Fill(pos->first,pos->second);

          if (tracks_l1.size()==2){
            canvas->cd(1); tracks_l1[0]->Draw("colz");
            canvas->cd(2); tracks_l1[1]->Draw("colz");

            //std::cout<<"Track1: "
            //  <<tracks_l1[0]->GetEntries()<<" "
            //  <<tracks_l1[1]->GetEntries()<<" "
            //  <<"\n";

            auto grps = layers_hits.at(1).draw_2d_tracks(sstr.str());
            util::same_grp_min_max(grps.at(0),grps.at(1));
            canvas->cd(3); grps[0]->Draw();
            canvas->cd(4); grps[1]->Draw();
          }
          if (tracks_l1.size()) track_fl->Add(canvas);
        }
        if (sum_adc>0.){
          //if (evt_no%1000==0) std::cout<<"progress\n";
          (*t_object_factory.get<TH1F>("spectrum"))->Fill(sum_adc);
          if (sum_adc_l0>0) (*t_object_factory.get<TH1F>("spectrum_L0"))->Fill(sum_adc_l0);
          if (sum_adc_l1>0) (*t_object_factory.get<TH1F>("spectrum_L1"))->Fill(sum_adc_l1);
          data_det_tree->Fill();
          strip_hits->Clear();
          evt_no++;
        }

        clear_object(data_strip);
        iter_prev = iter_next;
        iter = iter_prev;
        for (auto&& [x,y] : layers_hits) y.clear();
      }else{
        iter++;

      }
    }
    tm_vs_adc.clear();
  }
  //slider.stop();


  //his2d->Write();

  //t_object_factory
  //  .write_to(rfout,"residual.*")
  //  .write_to(rfout,"HitMap_L.*")
  //  .write_to(rfout,"spectrum_L.*")
  //  .write_to(rfout,"spectrum")
  //  ;
  //track_fl->Write();
  
  rfout->cd();
  data_det_tree->Write();
  fin->Close();
  rfout->Write();
  rfout->Close();
  fout.close();
  return 0;
}

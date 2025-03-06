//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: wy187110@mail.ustc.edu.cn     
//^-^ Time: 2024-11-20 16:15:18   Posi: Hefei
//^-^ File: draw_time_dis.cpp
//--------------------------------------------------------------------
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
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <filesystem>

#include "TFile.h"
#include "TTree.h"
#include "TFolder.h"
#include "TH2D.h"
#include "TH2F.h"
#include "TGraph.h"

#include "util.hpp"
#include "data_strcut_cint.h"
#include "clusters.hpp"

//static clusters<uint16_t> s_clusters_x;
//static clusters<uint16_t> s_clusters_y;
void clear(entry_tm& d){
  d.start_times.clear();
  d.adcs.clear();
  d.det_ids.clear();
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
//static std::vector<strip_info_t> strip_dis_x;
//static std::vector<strip_info_t> strip_dis_y;
static std::vector<uint16_t> strip_dis_x;
static std::vector<uint16_t> strip_dis_y;
struct point_t{
  float x;
  float y;
  float z;
  //point_t(float a, float b, float c):x(a),y(b),z(c) {}
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
    ,std::pair<float,float> ms, int scale, int comp0, int comp1,int width=20){
  auto min_iter = std::min_element(adcs.begin(),adcs.end());
  auto max_iter = std::max_element(adcs.begin(),adcs.end());
  if (*min_iter<ms.first-comp0*ms.second) return false;
  if (*max_iter<ms.first+comp1*ms.second) return false;
  int distance = (int)std::distance(adcs.begin(),max_iter);
  uint32_t sum_max=0;
  //info_out(distance); info_out(width);
  size_t beg = distance-width<=0 ? 0 : distance-width;
  size_t end = distance+width>1024 ? 1024 : distance+width;
  size_t range = end-beg;
  //info_out(beg); info_out(end);
  for (size_t i=beg; i<end; ++i){
    sum_max += adcs[i];
  }
  float peak_sz = (float)sum_max/range;
  //info_out(max_iter);
  if (peak_sz<ms.first+scale*ms.second) return false;
  if (peak_sz<0.8**max_iter) return false;
  return true;
}

template <class _fun_t,class... _args>
TFolder* waves_view_filter(std::string const& name, entry_new* data
    ,std::map<std::pair<int,int>,std::pair<float,float>> baseline_map
    ,_fun_t&& fun, _args&&... params){
  auto* folder = new TFolder(name.c_str(),name.c_str());
  auto& ids = data->global_ids;
  auto& adcs = data->adcs;
  std::map<std::string,TGraph*> grps;
  for (int i=0; i<ids.size(); ++i){
    auto layer_id = uint8_t(ids[i]>>8);
    auto channel_id = uint8_t(ids[i]&0xFF);
    auto ms = baseline_map[std::make_pair(layer_id,channel_id)];
    auto* graph = new TGraph();
    auto const& wave = adcs[i];
    auto max_adc = *std::max_element(std::begin(adcs[i]),std::end(adcs[i]));
    bool ok = fun(wave,ms,std::forward<_args>(params)...);
    if (ok){
      std::stringstream sstr("");
      sstr<<"wave"<<(int)layer_id<<"-"<<(int)channel_id;
      graph->SetName(sstr.str().c_str());
      graph->SetTitle(sstr.str().c_str());
      for (std::size_t index=0; auto&& x : adcs[i])
        graph->SetPoint(index,index,x), index++;
      grps[sstr.str()] = graph;
      //if (sstr.str()=="wave6-126"){
      //  info_out("============");
      //  bool ok = fun(wave,ms,std::forward<_args>(params)...);
      //  info_out("============");


      //}
    }else{
      std::stringstream sstr("");
      sstr<<"wave"<<(int)layer_id<<"-"<<(int)channel_id<<"-UWF";
      graph->SetName(sstr.str().c_str());
      graph->SetTitle(sstr.str().c_str());
      for (std::size_t index=0; auto&& x : adcs[i])
        graph->SetPoint(index,index,x), index++;
      grps[sstr.str()] = graph;
      //if (sstr.str()=="wave6-126-UWF"){
      //  info_out("============");
      //  bool ok = fun(wave,ms,std::forward<_args>(params)...);
      //  info_out("============");


      //}
    }
  }
  for (auto&& [x,y] : grps) folder->Add(y);
  return folder;
}



TGraph* draw_track_grp(){
  return nullptr;
}

TH2F* draw_track(std::vector<point_t> const& data
    , std::string const& name){
  auto* ret = new TH2F(name.c_str(),name.c_str(),384,0,384,180,0,18000);
  for (auto&& x : data){
    ret->Fill(x.x,x.y,x.z);
  }
  return ret;
}
int main(int argc, char* argv[]){
  auto* hit_map = new TH2F("hit_map","hit_map",3840,0,384,3840,0,384);
  strip_dis_x.resize(384);
  strip_dis_y.resize(384);
  if (argc<3)
    throw std::invalid_argument("...");
  std::string fname = argv[1];
  std::string baseline_name = argv[2];
  std::string fec2det_name = argv[4];
  std::cout<<"fname: "<<fname<<std::endl;
  std::cout<<"baseline_name: "<<baseline_name<<std::endl;

  std::ifstream map_file(fec2det_name.c_str());
  std::map<std::pair<uint16_t,uint16_t>,std::pair<uint32_t,uint32_t>> fec2det_map;
  while(!map_file.eof()){
    std::string sbuf;
    std::getline(map_file,sbuf);
    //util::trim_space(sbuf);
    if (!sbuf.empty() && sbuf[0] != '#'){
      std::stringstream sstr(sbuf.c_str());
      int a, b, c, d;
      sstr>>a>>b>>c>>d;
      fec2det_map[std::make_pair(a,b)] = std::make_pair(c,d);
    }
  }

  std::cout<<fec2det_map.size()<<std::endl;



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
    uint64_t evt_id;
    int id;
    bool is_valid = true;
  }max_id_t;

  std::multimap<uint64_t,max_id_t> tm_vs_adc;
  entry_tm data_tm;
  TFile* rfout = new TFile("timevsadc.root","recreate");
  TTree* data_tm_tree = new TTree("CollectionTree","CollectionTree");
  data_tm_tree->Branch("data",std::addressof(data_tm));
  //auto* rrfout = new TFile();
  //
  auto* his_tm_disx = new TH1F("drift_tm_disx","drift_tm_disx",200,0,20000);
  auto* his_tm_disy = new TH1F("drift_tm_disy","drift_tm_disy",200,0,20000);

  TH2F* his2d = new TH2F("timevsadc","timevsadc",1000000,0,10,50,0,5000);
  TH1F spectrum("spectrum","spectrum",2000,0,200000);
  TH1F* spectrum_sub_view = new TH1F("spectrum_sub_view"
      ,"spectrum_sub_view",2000,0,200000);
  TH1F* spectrum_sub_signale_view = new TH1F("spectrum_sub_signale_view"
      ,"spectrum_sub_signale_view",2000,0,200000);
  auto* hitsVsc = new TH2F("HitsVsCharge","HitsVsCharge",100,0,200,2000,0,200000);
  auto* hitsVsc_sub = new TH2F("HitsVsCharge_sub","HitsVsCharge_sub",100,0,200,2000,0,200000);
  std::multimap<int,std::string> entries_table;

  std::ofstream fout("timevsadc.txt");

  float compres =3.;
  uint32_t ww = std::stoi(argv[3]);
  std::cout<<argv[3]<<std::endl;
  std::cout<<"ww: "<<ww<<std::endl;




  TFolder* track_fl = new TFolder("track","track");
  TFolder* wave_fl = new TFolder("wave","wave");
  int evt_no = 0;
  auto entries = tree->GetEntries();
  info_out(entries);


//  {
//    //rfout->cd();
//    for (int i=0; i<30; ++i){
//      if (i%50==0) info_out("AA");
//      tree->GetEntry(i);
//      std::stringstream sstr("");
//      sstr<<"Event-"<<i<<std::endl;
//      waves_view(sstr.str(),data)->Write();
//      sstr = std::stringstream("");
//      sstr<<"Event-WF-"<<i<<std::endl;
//      waves_view_filter(sstr.str(),data,baseline_map
//          ,&wave_filter,5,5,8,5)->Write();
//    }
//    //rfout->Write();
//    //rfout->Close();
//    //fin->Close();
//  }


  in_memory_map_t in_memory_map;
  for (int j=0; j<std::ceil(entries/5000.); ++j) {
    int e_start = j*5000;
    int e_end = (j+1)*5000 > entries ? entries : (j+1)*5000;
    info_out(e_end-e_start);

    for (int i=e_start; i<e_end; ++i){
      tree->GetEntry(i);
      std::map<int,uint64_t> ts_map;
      for (std::size_t index=0; auto&& x : data->fec_ids)
        ts_map[x] = data->time_stamps[index++];
      for (std::size_t index=0; auto&& x : data->global_ids){
        auto const& adcs = data->adcs[index++];
        int fec_id = x>>8;
        int channel_id = x&0xFF;
        auto ms = baseline_map.at(std::make_pair(fec_id,channel_id));
        if (ts_map.find(fec_id)==ts_map.end()) continue;
        uint64_t ts = ts_map.at(fec_id);
        auto max = std::max_element(std::begin(adcs),std::end(adcs));
        uint16_t max_position = std::distance(std::begin(adcs),max);
        uint64_t ts_peak = std::round(ts*8.33) + (max_position-624)*25;
        max_id_t max_id;
        max_id.max = *max;
        max_id.id = x;
        max_id.evt_id = i;
        max_id.is_valid = wave_filter(adcs,ms,5,5,8,5);
        tm_vs_adc.emplace(ts_peak,max_id);
      }
    }
    std::cout<<"timevsadc: "<<tm_vs_adc.size()<<std::endl;

    auto iter_prev = tm_vs_adc.begin();
    for (auto iter =tm_vs_adc.begin(); iter != tm_vs_adc.end();){
      fout<<iter->first<<" "<<iter->second.max<<"\n";
      if (iter->first/1.e+9<20.){
        his2d->Fill(iter->first/1.e+9,iter->second.max);
      }  
         
      auto id = iter->second.id;
      auto iter_next = std::next(iter);
      float sum_adc = 0.f;
      if (iter_next->first-iter->first>ww){
        std::vector<point_t> px(0), py(0);
        for (auto iter_tmp = iter_prev; iter_tmp!=iter_next; ++iter_tmp){
          id = iter_tmp->second.id;
          auto ms = baseline_map.at(std::make_pair(id>>8,id&0xFF));


          if (iter_tmp->second.max<=(ms.first+compres*ms.second)) continue;
          sum_adc += iter_tmp->second.max - ms.first;
          auto det_id = fec2det_map.at(std::make_pair(id>>8,id&0xFF));
          if (iter_tmp->second.is_valid){
            auto evt_id = iter_tmp->second.evt_id;
            if (in_memory_map.find(evt_id)==in_memory_map.end()){
              tree->GetEntry(evt_id);
              load_in_momery(in_memory_map,evt_id,data);
            }
            if (det_id.first==0){
              strip_dis_x[det_id.second] = iter_tmp->second.max;
              data_tm.det_ids.emplace_back((0<<16)+det_id.second);
              data_tm.start_times.emplace_back(iter_prev->first);
              data_tm.adcs.emplace_back(in_memory_map.at(evt_id).at(iter_tmp->second.id));

              
              point_t pt;
              pt.x = det_id.second;
              pt.y = iter_tmp->first-iter_prev->first;
              pt.z = iter_tmp->second.max;
              px.emplace_back(pt);

            }else if(det_id.first==1){
              strip_dis_y[det_id.second] = iter_tmp->second.max;
              data_tm.det_ids.emplace_back((1<<16)+det_id.second);
              data_tm.start_times.emplace_back(iter_prev->first);
              data_tm.adcs.emplace_back(in_memory_map.at(evt_id).at(iter_tmp->second.id));

              point_t pt;
              pt.x = det_id.second;
              pt.y = iter_tmp->first-iter_prev->first;
              pt.z = iter_tmp->second.max;
              py.emplace_back(pt);
            }
            if (in_memory_map.size()>=20)
              in_memory_map.erase(std::begin(in_memory_map)
                  ,std::next(std::begin(in_memory_map),10));
          }
        }
        if (sum_adc>0.){
          if (evt_no%1000==0) std::cout<<"progress\n";
          spectrum.Fill(sum_adc);
          //clusters<point_t> clusters_x, clusters_y;
          clusters<uint16_t> clusters_x, clusters_y;
          clusters_x.s_set_tolerate(20);
          clusters_y.s_set_tolerate(20);
          clusters_x.calc(std::begin(strip_dis_x)
              ,std::end(strip_dis_x)
              ,[](uint16_t v){return v>0;}
              ,[](uint16_t v){return v;});
          clusters_x.hadd();
          clusters_y.calc(std::begin(strip_dis_y)
              ,std::end(strip_dis_y)
              ,[](uint16_t v){return v>0;}
              ,[](uint16_t v){return v;});
          clusters_y.hadd();
          if (clusters_x.number()==1 && clusters_y.number()==1){
            double x = std::begin(clusters_x.m_data)->cog([](uint16_t v){return v;});
            double y = std::begin(clusters_y.m_data)->cog([](uint16_t v){return v;});
            size_t sx = std::begin(clusters_x.m_data)->size([](uint16_t v){return v>0;});
            size_t sy = std::begin(clusters_x.m_data)->size([](uint16_t v){return v>0;});
            hitsVsc->Fill(sx+sy,sum_adc);
            
            //if (x>182 && x<185
            //    && y>202 && y<204){
            //  spectrum_sub_view->Fill(sum_adc);
            //}
            //if (sx+sy>=100 && sum_adc>=30000 && evt_no<300){
            //if (sx+sy>=30 && sx+sy<45 && sum_adc>=4800 && sum_adc<=7400/* && evt_no<300*/){
            if (
                sum_adc>1000
                && sum_adc>36000
                //&& sx+sy>=30 && sx+sy<45
                //&& sum_adc>=5500 && sum_adc<=6000
                 ){
              if (px.size()>3 || py.size()>3){
                std::size_t out_range_ctx = 0;
                std::size_t out_range_cty = 0;
                std::size_t min_timx=0;
                std::size_t min_timy=0;
                std::size_t max_timx=0;
                std::size_t max_timy=0;
                for (auto&& x : px){
                  if (x.y>max_timx) max_timx = x.y;
                  if (x.y<min_timx) min_timx = x.y;
                  if (x.x<25 || x.x>384-25) out_range_ctx++;
                }
                for (auto&& x : py){
                  if (x.y>max_timy) max_timy = x.y;
                  if (x.y<min_timy) min_timy = x.y;
                  if (x.x<25 || x.x>384-25) out_range_cty++;
                }
                his_tm_disx->Fill(max_timx-min_timx);
                his_tm_disy->Fill(max_timy-min_timy);
                if (
                    true
                    //&& out_range_ctx<2 && out_range_cty<2
                    //&& max_timx-min_timx < 2000
                    //&& max_timy-min_timy < 2000
                    && max_timx-min_timx > 6000
                    && max_timy-min_timy > 6000
                     ){
                  spectrum_sub_signale_view->Fill(sum_adc);
                  hitsVsc_sub->Fill(sx+sy,sum_adc);


                  std::stringstream sstr("");
                  sstr<<"Event-"<<evt_no<<"-XZ";
                  track_fl->Add(draw_track(px,sstr.str()));
                  std::stringstream sstr1("");
                  sstr1.clear();
                  sstr1<<"Event-"<<evt_no<<"-YZ";
                  track_fl->Add(draw_track(py,sstr1.str()));
                  //draw_wave(evt_no,&data_tm);
                }
              }

            }
            //std::cout<<x<<" "<<y<<std::endl;
            //hit_map->Fill(float(x),float(y));
          }

          data_tm_tree->Fill();
          evt_no++;
        }
        clear(data_tm);
        std::fill(std::begin(strip_dis_x),std::end(strip_dis_x),0);
        std::fill(std::begin(strip_dis_y),std::end(strip_dis_y),0);

        iter_prev = iter_next;
        iter = iter_prev;

      }else{
        iter++;

      }

      //auto ms =baseline_map.at(std::make_pair(id>>8,id&0xFF));
      //if((ms.first+compres*ms.second) <= iter->second.max){
      //  auto time_start = iter->first;
      //  auto iter_end =tm_vs_adc.upper_bound(time_start+ww);
      //  uint32_t sum_adc = 0;
      //
      //  for (auto iter_tmp=iter; iter_tmp!=iter_end; ++iter_tmp){
      //    id = iter_tmp->second.id;
      //    ms =baseline_map.at(std::make_pair(id>>8,id&0xFF));
      //    sum_adc += iter_tmp->second.max - ms.first;
      //    
      //  }
      //  spectrum->Fill(sum_adc);
      //  if (iter_end==iter){ iter++;continue;}
      //  iter = iter_end;
      //  continue;
      //}
      //iter++;
    }
    tm_vs_adc.clear();
    //break;
  }
  //rfout->cd();
  his2d->Write();
  spectrum.Write();
  //spectrum->Write();
  hit_map->Write();
  hitsVsc->Write();
  spectrum_sub_signale_view->Write();
  hitsVsc_sub->Write();
  spectrum_sub_view->Write();
  his_tm_disx->Write();
  his_tm_disy->Write();

  track_fl->Write();
  fin->Close();
  rfout->Write();
  rfout->Close();

  //for (auto&& [x,y] : entries_table)
  //  fout<<x<<" "<<y<<std::endl;

  fout.close();
  return 0;
}

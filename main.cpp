//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: 2835516101@qq.com     
//^-^ Time: 2024-07-22 21:39:35   Posi: Hefei
//^-^ File: temp1.cpp
//--------------------------------------------------------------------
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <filesystem>

#include "TFolder.h"
#include "TFile.h"
#include "TGraph.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TF1.h"
#include "TSystem.h"

#include "util.hpp"
#include "clusters.hpp"
#include "data_strcut_cint.h"
#include "unpack.h"
#include "wave_handler.h"

static std::size_t x_strips = 375;
static std::size_t y_strips = 375;

uint16_t make_id(uint8_t fec_id, uint8_t channel_id){ return (fec_id<<8) + channel_id; }
uint32_t make_det_id(uint16_t isx, uint16_t channel_id){ return (isx<<16) + channel_id; }
std::pair<uint16_t,uint16_t> get_dec_id(uint32_t id){
  return std::make_pair(uint16_t(id>>16), uint16_t(id&0xFFFF)); }
  
void hit_positon(entry_new* entry
    ,std::map<uint16_t,uint32_t> const& f2dmap
    ,std::unordered_map<uint16_t,std::pair<float,float>> const& pmap
    ,TClonesArray* storex
    ,TClonesArray* storey
    ){

  std::vector<wave_t> detx(x_strips), dety(y_strips);
  for (int j=0; j<entry->global_ids.size(); ++j){
    auto x = entry->global_ids.at(j);
    auto det_channel_id = get_dec_id(f2dmap.at(x));
    auto& ref = entry->adcs[j];
    if (det_channel_id.first==0){
      if (det_channel_id.second>x_strips-1){
      }
      else{
        auto& wave_ref = detx.at(det_channel_id.second);
        wave_ref.strip_id = det_channel_id.second;
        wave_ref.mean = pmap.at(x).first;
        wave_ref.sigma = pmap.at(x).second;
        wave_ref.wave.resize(ref.size());
        std::copy(ref.begin(),ref.end(),wave_ref.wave.begin());
      }
    }
    if (det_channel_id.first==1){
      if (det_channel_id.second>y_strips-1){
      }
      else{
        auto& wave_ref = dety.at(det_channel_id.second);
        wave_ref.strip_id = det_channel_id.second;
        wave_ref.mean = pmap.at(x).first;
        wave_ref.sigma = pmap.at(x).second;
        wave_ref.wave.resize(ref.size());
        std::copy(ref.begin(),ref.end(),wave_ref.wave.begin());
      }
    }
    std::stringstream sstr("");
    sstr<<"wave"<<det_channel_id.first<<"-"<<det_channel_id.second;
    TH1F his(sstr.str().c_str(),sstr.str().c_str(),1024,0,1024);
    for (int i=0; i<ref.size(); ++i){
      his.SetBinContent(i+1,ref[i]);
    }
    his.Write();
  } 
  float offset = 3.;
  auto const& judge_threshold = [](wave_t& v, float offset)->bool{
    if(v.mean>=1.e-3 && v.sigma>=1.e-3 && !v.wave.empty()){
      uint16_t thr = std::round(offset*v.sigma);
      auto max = std::max_element(v.wave.begin(),v.wave.end());

      //FIXME
      v._is_calc = true;
      v.peak = *max;
      v.peak_position = std::distance(v.wave.begin(),max);
      return *max-v.mean>=thr;
    }
    return false;
  };

  auto const& get_char = [](wave_t const& v){
    wave_char ret;
    if (v._is_calc){
      ret.peak = v.peak;
      ret.peak_position = v.peak_position;
    }
    return ret;
  };
  clusters<wave_char> clusters_x, clusters_y;
  clusters_x.calc(detx.begin(),detx.end()
      ,judge_threshold
      ,get_char
      ,offset);

  std::size_t index=0;
  if (storex){
    for (auto iter = clusters_x.m_data.begin(); iter!=clusters_x.m_data.end(); ++iter){
      new ((*storex)[index]) cluster();
      auto* store = (cluster*)storex->At(index);
      store->range.first = iter->m_range.first;
      store->range.second= iter->m_range.second;
      store->amp.resize(iter->m_data.size());
      store->amp_time.resize(iter->m_data.size());
      for (int i=0; i<iter->m_data.size(); ++i){
        store->amp[i] = iter->m_data[i].peak;
        store->amp_time[i] = iter->m_data[i].peak_position;
      }
      index++;
    }
  }
  //info_out(clusters_x.m_data.size());
  clusters_y.calc(dety.begin(),dety.end()
      ,judge_threshold
      ,get_char
      ,offset);

  index=0;
  if (storey){
    info_out(clusters_y.m_data.size());
    for (auto iter = clusters_y.m_data.begin(); iter!=clusters_y.m_data.end(); ++iter){
      new ((*storey)[index]) cluster();
      auto* store = (cluster*)storey->At(index);
      store->range.first = iter->m_range.first;
      store->range.second= iter->m_range.second;
      store->amp.resize(iter->m_data.size());
      store->amp_time.resize(iter->m_data.size());
      for (int i=0; i<iter->m_data.size(); ++i){
        store->amp[i] = iter->m_data[i].peak;
        store->amp_time[i] = iter->m_data[i].peak_position;
      }
      index++;
    }
  }




  //gFile->Write();
}


struct lcs_t{
  int layer_id=0;
  int channel_id=0;
  float compres=3;
};
struct lc_adc_t{
  uint16_t layer_id=0;
  uint16_t channel_id=0;
  uint16_t adc_t=0;
};
static uint8_t rise_step = 0x0;
static uint16_t delay_time = 0x258;
static uint8_t wait_cycle = 10;
lcs_t parse_lcs(std::string str){
  lcs_t rt;
  int index=0;
  while(str.find_first_of('>')!=std::string::npos){
    std::string v_str = str.substr(str.find_first_of('<')+1,str.find_first_of('>')-1);
    if (index==0) rt.layer_id=std::stoi(v_str.c_str());
    else if (index==1) rt.channel_id= std::stoi(v_str.c_str());
    else if (index==2) rt.compres=std::stof(v_str.c_str());
    str = str.substr(str.find_first_of('>')+1);
    index++; }
  return rt; }
lc_adc_t parse_lc_adc(std::string str){
  lc_adc_t rt;
  int index=0;
  while(str.find_first_of('>')!=std::string::npos){
    std::string v_str = str.substr(str.find_first_of('<')+1,str.find_first_of('>')-1);
    if (index==0) rt.layer_id=std::stoul(v_str.c_str());
    else if (index==1) rt.channel_id= std::stoul(v_str.c_str());
    else if (index==2){
      std::stringstream sstr(""); sstr<<v_str;
      sstr>>std::hex>>rt.adc_t;
    }
    str = str.substr(str.find_first_of('>')+1);
    index++; }
  return rt; }
std::unordered_map<int,float> compres_table;
typedef std::unordered_map<int,std::pair<float,float>> mean_and_rms_t;
void generate_configs(
  std::string const& path
  ,mean_and_rms_t const& maps
  ,float compres
  ){
  float compres_back = compres;
  using namespace util;
  char* frame_buf = new char[9];
  std::vector<char> frame(0);
  for (auto [x,y] : maps){
    uint16_t gid = x;
    if (compres_table.find(gid) != compres_table.end()) compres=compres_table.at(gid);
    else compres=compres_back;
    uint8_t fec_id= uint8_t(x>>8);
    uint8_t channel_id = uint8_t(x&0xFF);
    std::stringstream sstr("");
    sstr<<"L"<<(int)fec_id<<"CompressThreshold_"<<(int)channel_id<<".dat";
    if (!std::filesystem::exists(path.c_str()))
      std::filesystem::create_directory(path.c_str());
    std::string fname=path+"/"+sstr.str();
    std::ofstream fout(fname.c_str(),std::ios::out|std::ios::binary);

    head_write(frame_buf,e_command_address::k_TThreshold);
    head_write(frame_buf+4,e_command_address::k_Reset,0x40);
    frame_buf[8] = 0x83;
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);

    generate_frame(e_command_address::k_FEID,frame_buf,fec_id);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    generate_frame(e_command_address::k_ChannelID,frame_buf,channel_id);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    generate_frame(e_command_address::k_Threshold_Tag,frame_buf);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    for (int i=0; i<9; ++i) frame_buf[i]=0;
    generate_frame(e_command_address::k_Threshold_Value,frame_buf
        ,(uint16_t)std::floor(y.first)
        ,(uint16_t)std::floor(y.second)
        ,compres);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    for (int i=0; i<9; ++i) frame_buf[i]=0;
    generate_frame(e_command_address::k_Config,frame_buf);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    fout.write(frame.data(),frame.size());
    frame.clear();
    fout.close();
  }
  delete[] frame_buf;
}


typedef std::unordered_map<uint16_t,uint16_t> tt_map_t;
tt_map_t tt_map;
void generate_tt(std::string const& path
    ,tt_map_t const& map){
  std::string fname = "HitThreshold.dat";
  fname = path+"/"+fname;
  auto* data = new char[map.size()*36+9];
  char* begin = data;
  using namespace util;
  head_write(data,e_command_address::k_TThreshold);
  head_write(data+4,e_command_address::k_Confirm,0x40);
  data[8] = 0x83;
  data+=9;
  for (auto [x,y] : map){
    uint8_t fec_id= uint8_t(x>>8);
    uint8_t channel_id = uint8_t(x&0xFF);
    generate_frame_trigger(data,fec_id,channel_id,y);
  }
  if (map.size()==0) data+=9;
  std::ofstream fout(fname.c_str(),std::ios::binary|std::ios::out);
  fout.write(begin,map.size()*36+9);
  fout.close();
  delete[] begin; }
void generate_mis(std::string const& path){
  using namespace util;
  char* data = new char[100];
  std::string fname = "Baseline.dat";
  {
  fname = path+"/"+fname;
  std::ofstream fout(fname.c_str(),std::ios::binary|std::ios::out);
  fout.write(data,generate_frame_baseline(data,{0,0,0}));
  fout.close();
  }

  {
  fname = "HitWidthCycle.dat";
  fname = path+"/"+fname;
  std::ofstream fout(fname.c_str(),std::ios::binary|std::ios::out);

  std::cout<<__LINE__<<" "<<(int)wait_cycle<<std::endl;
  fout.write(data,generate_frame_hitwidth_cycle(data,wait_cycle));
  fout.close();
  }

  {
  fname = "IIRFilter.dat";
  fname = path+"/"+fname;
  std::ofstream fout(fname.c_str(),std::ios::binary|std::ios::out);
  fout.write(data,generate_frame_irrfilter(data,{0,0,0,0,0,0,0,0}));
  fout.close();
  }

  {
  fname = "TrigRiseStep.dat";
  fname = path+"/"+fname;
  std::ofstream fout(fname.c_str(),std::ios::binary|std::ios::out);
  fout.write(data,generate_frame_trig_rise_step(data,rise_step));
  fout.close();
  }

  {
  fname = "TrigDelayCycle.dat";
  fname = path+"/"+fname;
  std::ofstream fout(fname.c_str(),std::ios::binary|std::ios::out);
  std::cout<<__LINE__<<" "<<delay_time<<std::endl;
  fout.write(data,generate_frame_trig_delay_cycle(data,delay_time));
  fout.close();
  }

  {
  fname = "NHitChannel.dat";
  fname = path+"/"+fname;
  std::ofstream fout(fname.c_str(),std::ios::binary|std::ios::out);
  fout.write(data,generate_frame_nhit_channel(data,0x04));
  fout.close();
  }
  delete[] data; }



using std::cout; using std::endl; using std::string; using std::vector;
int main(int argc, char* argv[]){
  {
    TTree* data_tree = new TTree("CollectionTree","CollectionTree");
    entry_new entry_buffer;
    data_tree->Branch("data",std::addressof(entry_buffer));
    auto argv_map = util::read_argv("unpack");
    std::string dat_name=argv_map["filename"];
    std::string entry_out_file = dat_name.substr(
        0,dat_name.find_last_of("."))+"_entry.root";
    TFile* fout = new TFile(entry_out_file.c_str(),"recreate");
    
    //TODO !!! read-write-queue needed! //FIXME
    std::ifstream fin(dat_name.c_str(),std::ios::binary);
    fin.seekg(0,std::ios_base::end);
    size_t fsz = fin.tellg();
    fin.seekg(0,std::ios_base::beg);
    char* data = new char[fsz];
    fin.read(data,fsz);
    char* iter_beg = data;
    fin.close();

    typedef typename util::terminal_color tc;
    using namespace util;
    std::cout<<terminal_color(
        tc::display_mode::k_underline
        ,tc::f_color::k_white
        ,tc::b_color::k_blue)
      <<"UNPACK" <<util::terminal_reset() <<std::endl;

    waveform_by_entry wf;
    wf.fec_count(std::stoul(argv_map["fec_count"]));
    wf.set_store(entry_buffer);
    wf.set_tree(data_tree);
    wf.do_parse(iter_beg,iter_beg+fsz);
    fout->cd();
    data_tree->Write(); 
    fout->Write(); fout->Close(); 
    delete[] data;
    std::cout<<"Raw Root Store: "<<entry_out_file<<"\n";
  }
  {
    typedef typename util::terminal_color tc;
    std::cout<<util::terminal_color(
        tc::display_mode::k_underline
        ,tc::f_color::k_white
        ,tc::b_color::k_blue)
      <<"CONFIG and PRESTAL" <<util::terminal_reset() <<std::endl;
    auto argv_map = util::read_argv("config");
    auto file_replace = util::read_argv("self-def");
    tt_map.clear();
    compres_table.clear();
    for (auto&& x : argv_map){
      if(x.first[0]=='L' && x.second=="T"){
        auto ps = parse_lcs(x.first);
        int global_ids=((uint16_t)ps.layer_id<<8)+(uint16_t)ps.channel_id;
        compres_table[global_ids]=ps.compres;
      }
      if (x.first.substr(0,2)=="TT" && x.second=="T"){
        auto ps = parse_lc_adc(x.first);
        uint16_t global_ids=((uint16_t)ps.layer_id<<8)+(uint16_t)ps.channel_id;
        tt_map[global_ids] = ps.adc_t;
      }
      if (x.first=="Rise Step Trigger Threshold"){
        uint32_t value = std::stoul(x.second);
        if (value>0xFF){
          std::cerr<<"Warning!! rise step trigger threshold set overflow"<<std::endl;
          rise_step = 0xFF;
          continue; }
        rise_step = (uint8_t)value;
      }
      if (x.first=="Trigger Delay Time"){
        uint32_t value = std::stoul(x.second);
        if (value>0xFFFF){
          std::cerr<<"Warning!! delay time set overflow"<<std::endl;
          delay_time = 0xFFFF;
          continue; }
        delay_time = (uint16_t)value;
      }
      if (x.first=="Trigger Wait Cycle"){
        uint32_t value = std::stoul(x.second);
        if (value>0xFF){
          std::cerr<<"Warning!! trigger wait cycle set overflow"<<std::endl;
          wait_cycle = 0xFF;
          continue; }
        wait_cycle = (uint8_t)value;
      }
    }
    std::string entry_out_file=argv_map["filename"];
    TFile* rfin = new TFile(entry_out_file.c_str());
    auto* fout = new TFile("generate_config.root","recreate");
    auto* data_tree  = static_cast<TTree*>(rfin->Get("CollectionTree"));
    entry_new* entry_buffer_ptr = new entry_new;
    data_tree->SetBranchAddress("data",std::addressof(entry_buffer_ptr));
    //std::unordered_map<int,TH1I*> baseline_map;
    std::map<int,TH1I*> baseline_map;
    for (long long i=0; i<data_tree->GetEntries(); ++i){
      data_tree->GetEntry(i);
      for (int j=0; j<entry_buffer_ptr->global_ids.size(); ++j){
        int x = entry_buffer_ptr->global_ids.at(j);
        if (baseline_map.find(x)==baseline_map.end()){
          std::stringstream sstr(""); sstr<<"baseline-"<<x;
          auto const& ref = entry_buffer_ptr->adcs[j];
          uint16_t min = *util::min_element(std::begin(ref),std::end(ref));
          uint16_t max = *util::max_element(std::begin(ref),std::end(ref));
          min -= 10; max += 10;
          baseline_map[x] = new TH1I(sstr.str().c_str(),sstr.str().c_str()
              ,max-min,min,max); }
        for (auto&& adc : entry_buffer_ptr->adcs[j]){ baseline_map[x]->Fill(adc); }
      }
    }
    fout->cd();
    std::unordered_map<int,std::pair<float,float>> mean_and_rms;
    std::string prestal_name = "prestal.txt";
    std::ofstream prestal_txt(prestal_name.c_str());







    for (auto iter = baseline_map.begin(); iter != baseline_map.end(); ++iter) {
      TF1 f_gaus("f_gaus","gaus"
          ,iter->second->GetMean()-3*iter->second->GetRMS()
          ,iter->second->GetMean()+3*iter->second->GetRMS());
      iter->second->Fit(&f_gaus,"RQ");
      mean_and_rms[iter->first].first=iter->second->GetMean();
      mean_and_rms[iter->first].second=iter->second->GetRMS();
      prestal_txt<<iter->first<<" "<<f_gaus.GetParameter(1)<<" "<<f_gaus.GetParameter(2)<<"\n";
    }
    prestal_txt.close();
    generate_configs(argv_map["path"],mean_and_rms,std::stof(argv_map["sigma-compres"]));
    generate_tt(argv_map["path"],tt_map);
    generate_mis(argv_map["path"]);
    for (auto iter = file_replace.begin(); iter != file_replace.end(); ++iter){
      if (iter->first=="NHitChannel"){
        std::string to = argv_map["path"]+"/NHitChannel.dat";
        int s = gSystem->CopyFile(iter->second.c_str(),to.c_str(),true);
        if (s!=0) std::cerr<<"CopyFile("<<iter->second<<","<<to<<") Failed\n";
      }else if(iter->first=="Baseline"){
      }
    }
    TH1I mean_dis("mean","mean",3000,0,3000);
    TH1F rms_dis("rms","rms",3000,0,3000);
    for (auto [x,y] : mean_and_rms){
      mean_dis.SetBinContent(x,y.first);
      rms_dis.SetBinContent(x,y.second); }
    mean_dis.Write(); rms_dis.Write();
    rfin->Close();
    fout->Write(); fout->Close();
    delete entry_buffer_ptr;
  }

  {
    auto argv = util::read_argv("map");
    if (argv.find("mapname") == argv.end()){
      std::cerr<<"[ERROR] No access map name"<<std::endl;
      return -1; }
    std::string map_file_name=argv["mapname"];
    std::ifstream map_file(map_file_name.c_str());
    std::map<uint16_t,uint32_t> fec2det_map;
    std::string sbuf;
    while(!map_file.eof()){
      std::getline(map_file,sbuf);
      util::trim_space(sbuf);
      if (!sbuf.empty() && sbuf[0] != '#'){
        std::stringstream sstr(sbuf.c_str());
        int a, b, c, d;
        sstr>>a>>b>>c>>d;
        fec2det_map[make_id(uint8_t(a),uint8_t(b))] = make_det_id(uint16_t(c),uint16_t(d));
      }
    }

    std::unordered_map<uint16_t,std::pair<float,float>> prestal_map;
    std::string prestal_file = argv["prestal"];
    info_out(prestal_file);
    std::ifstream prestal_fin(prestal_file.c_str());
    if (!prestal_fin.is_open()){
      std::cerr<<"..."<<std::endl;
      return 0;
    }
    while(!prestal_fin.eof()){
      std::getline(prestal_fin,sbuf);
      util::trim_space(sbuf);
      if(!sbuf.empty() && sbuf[0] != '#'){
        std::stringstream sstr(sbuf.c_str());
        int a; float b, c;
        sstr>>a>>b>>c;
        prestal_map[a]=std::make_pair(b,c);
      }
    }
    prestal_fin.close();
    std::string root_file_name=argv["filename"];
    entry_new* entry_buffer_ptr = new entry_new;

    auto* fin = new TFile(root_file_name.c_str());
    auto* data_tree  = static_cast<TTree*>(fin->Get("CollectionTree"));
    data_tree->SetBranchAddress("data",std::addressof(entry_buffer_ptr));
    std::string fout_name = root_file_name.substr(
        0,root_file_name.find_last_of("_"))+"_reco.root";
    auto* fout = new TFile(fout_name.c_str(),"recreate");

    auto* tree = new TTree("CollectionTree","CollectionTree");

    TClonesArray* cluster_x = new TClonesArray("cluster",10);
    TClonesArray& cluster_x_ref = *cluster_x;
    tree->Branch("clusters_x",std::addressof(cluster_x),256000,0);
    cluster_x->BypassStreamer();
    TClonesArray* cluster_y = new TClonesArray("cluster",10);
    TClonesArray& cluster_y_ref = *cluster_y;
    tree->Branch("clusters_y",std::addressof(cluster_y),256000,0);
    cluster_y->BypassStreamer();


    for (long long i=0; i<data_tree->GetEntries(); ++i){
      data_tree->GetEntry(i);
      cluster_x->Clear();
      cluster_y->Clear();
      hit_positon(entry_buffer_ptr,fec2det_map,prestal_map
          ,cluster_x,cluster_y);
      tree->Fill();
      //break;
    }

    fout->cd();
    tree->Write();
    fout->Write();fout->Close();

    typedef typename util::terminal_color tc;
    using namespace util;
    std::cout<<terminal_color(
        tc::display_mode::k_underline
        ,tc::f_color::k_white
        ,tc::b_color::k_blue)
      <<"fec2det MAP" <<util::terminal_reset() <<std::endl;
    std::cout<<"Reco Root Store: "<<fout_name<<"\n";
  }

  return 0;
}

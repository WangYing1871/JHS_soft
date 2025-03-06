//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: wy187110@mail.ustc.edu.cn     
//^-^ Time: 2024-12-11 16:04:00   Posi: Hefei
//^-^ File: main.cpp
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
#include <future>
#include <filesystem>

#include "TFolder.h"
#include "TFile.h"
#include "TGraph.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TF1.h"
#include "TSystem.h"

#include "util.hpp"
#include "clusters.hpp"
#include "data_strcut_cint.h"
#include "unpack.h"
#include "wave_handler.h"

static bool use_ini=false;

std::string get_title(std::string const& name){
  auto p0 = name.find_last_of('.');
  if (p0 != std::string::npos){
    auto p1 = name.find_last_of('/');
    if (p1!=std::string::npos){
      return name.substr(p1+1,p0-p1-1);
    }
    return name.substr(0,p0);
  }
  return name;
}
namespace service{

}

using std::cout; using std::endl; using std::string; using std::vector;
int main(int argc, char* argv[]){

  //info_out(get_title(argv[1]));
  // return 0;
  util::getopt opt_parser;
  opt_parser.run(argc,argv);
  std::string dat_name;

  if (opt_parser.vm.count("config")){
    use_ini = true;
    goto step0;
  }


step0:
  {
    int fec_count;
    int mode=-1;
    opt_parser.count("mode",mode);

    if (use_ini){
      auto argv_map = util::read_argv("unpack");
      if (argv_map.find("Enable")==argv_map.end() || argv_map.at("Enable")!="T")
      goto step1;
      dat_name=argv_map["filename"];
      fec_count = std::stoul(argv_map["fec_count"]);
    }else{
      if (!opt_parser.count("file",dat_name) && argc != 0){
        std::cerr<<"No input dat file. exit...\n"
          <<"HELP:\n"
          <<opt_parser.desc
          <<"\n"
          ;
        return -1;
      }
      if (!std::filesystem::exists(dat_name)){
        std::cerr<<" File: "<<dat_name<<" not exists. exit...\n";
        return -1;
      }
      if (!opt_parser.count("board",fec_count)){
        std::cerr<<"No input fec_count. exit...\n";
        return -1;
      }
    }

    TTree* data_tree = new TTree("CollectionTree","CollectionTree");
    entry_new entry_buffer;
    data_tree->Branch("data",std::addressof(entry_buffer));
    std::string entry_out_file = dat_name.substr(
        0,dat_name.find_last_of("."))+"_entry.root";
    TFile* fout = new TFile(entry_out_file.c_str(),"recreate");
    
    //TODO !!! read-write-queue needed! //FIXME
    std::ifstream fin(dat_name.c_str(),std::ios::binary);
    fin.seekg(0,std::ios_base::end);
    size_t fsz = fin.tellg();
    info_out(fsz);
    waveform_by_entry wf;
    wf.fec_count(fec_count);
    wf.set_store(entry_buffer);
    wf.set_tree(data_tree);
    wf.set_mode(mode);

    std::size_t oneG = (std::size_t)1024*1024*1024;
    for(int i=0; i<std::ceil(fsz/(float)oneG); ++i){
    //for(int i=0; i<4; ++i){
      fin.seekg(i*oneG,std::ios_base::beg);
      char* data = new char[oneG];
      fin.read(data,oneG);
      auto readed = fin.gcount();
      info_out(readed);
      char* iter_beg = data;
      wf.do_parse(iter_beg,iter_beg+readed);
      delete[] data;
    }

    fin.close();
    fout->cd();
    data_tree->Write(); 
    fout->Write(); fout->Close(); 
    std::cout<<"Raw Root Store: "<<entry_out_file<<"\n";
    typedef typename util::terminal_color tc;
    using namespace util;
    std::cout<<terminal_color(
        tc::display_mode::k_underline
        ,tc::f_color::k_white
        ,tc::b_color::k_blue)
      <<"---->UNPACK DONE<----" <<util::terminal_reset() <<std::endl;

    if (mode==1)
      return 0;
  }

step1:
  {
    std::unordered_map<std::string,std::string> argv_map;
    std::unordered_map<int,float> compres_table;
    util::tt_map_t tt_map;
    util::mis_param_t mp;
    if (use_ini){
      argv_map = util::read_argv("config");
      for (auto&& x : argv_map){
        if(x.first[0]=='L' && x.second=="T"){
          auto ps = util::parse_lcs(x.first);
          //info_out(ps.size());
          //int global_ids=((uint16_t)ps.layer_id<<8)+(uint16_t)ps.channel_id;
          for (auto&& x : ps){
            compres_table[(x.layer_id<<8)+x.channel_id]=x.compres;
          }
        }
        if (x.first.substr(0,2)=="TT" && x.second=="T"){
          auto ps = util::parse_lc_adc(x.first);
          //uint16_t global_ids=((uint16_t)ps.layer_id<<8)+(uint16_t)ps.channel_id;
          //tt_map[global_ids] = ps.adc_t;
          for (auto&& x : ps)
            tt_map[(x.layer_id<<8)+x.channel_id]=x.adc_t;
        }
        if (x.first=="Rise Step Trigger Threshold"){
          uint32_t value = std::stoul(x.second);
          if (value>0xFF){
            std::cerr<<"Warning!! rise step trigger threshold set overflow"<<std::endl;
            mp.s_rise_step = 0xFF;
            continue; }
          mp.s_rise_step = (uint8_t)value;
        }
        if (x.first=="Trigger Delay Time"){
          uint32_t value = std::stoul(x.second);
          if (value>0xFFFF){
            std::cerr<<"Warning!! delay time set overflow"<<std::endl;
            mp.s_delay_time = 0xFFFF;
            continue; }
          mp.s_delay_time = (uint16_t)value;
        }
        if (x.first=="Trigger Wait Cycle"){
          uint32_t value = std::stoul(x.second);
          if (value>0xFF){
            std::cerr<<"Warning!! trigger wait cycle set overflow"<<std::endl;
            mp.s_wait_cycle = 0xFF;
            continue; }
          mp.s_wait_cycle = (uint8_t)value;
        }
        if (x.first=="NHitChannel"){
          info_out(std::stoul(x.second));
          mp.s_nhit_strips = (uint8_t)std::stoul(x.second);
        }

      }
    }
    std::string entry_out_file="";
    std::string prestal_name ="";
    std::string baseline_name = "baseline.root";
    if (!std::filesystem::exists("calc"))
      std::filesystem::create_directory("calc");
    if (use_ini){
      if (argv_map.find("Enable")==argv_map.end() || argv_map.at("Enable")!="T")
        goto step2;
      entry_out_file=argv_map["filename"];
      if (argv_map.find("prestal_path") != argv_map.end())
        prestal_name = argv_map.at("prestal_path");
    }else{
      opt_parser.count("rawfile",entry_out_file);
      if (entry_out_file=="<unpack>")
        entry_out_file=dat_name.substr(0,dat_name.find_last_of('.'))+"_entry.root";
      int mode=-1;
      opt_parser.count("mode",mode);
      if (mode==1) goto step2;
      opt_parser.count("prestal_file",prestal_name);
      if (prestal_name=="prestal.txt"){
        prestal_name = "calc/"+get_title(dat_name)+"_prestal.txt";
        baseline_name = "calc/"+get_title(dat_name)+"_baseline.root";
      }

    }
    TFile* rfin = new TFile(entry_out_file.c_str());
    auto* fout = new TFile(baseline_name.c_str(),"recreate");
    auto* data_tree  = static_cast<TTree*>(rfin->Get("CollectionTree"));
    entry_new* entry_buffer_ptr = new entry_new;
    data_tree->SetBranchAddress("data",std::addressof(entry_buffer_ptr));

    auto const& fill = [](std::string const& name
        ,auto const& count_map
        ,std::size_t bins_unit=2)->TH1I*{
      //info_out(count_map.size()); exit(0);
      auto lower = std::begin(count_map)->first;
      auto upper = std::prev(std::end(count_map))->first;
      //std::cout<<lower<<" "<<upper<<std::endl;
      std::size_t bins = upper-lower+1;
      if (bins%bins_unit!=0) bins = bins/bins_unit*bins_unit+bins_unit;
      auto* ret = new TH1I(name.c_str(),name.c_str(),bins,lower,lower+bins);
      ret->SetDirectory(nullptr);
      std::size_t index=1;
      std::size_t tot_entries = 0;
      for (auto&& [x,y] : count_map){
        auto le = ret->GetBinLowEdge(index);
        while(le!=x) {index++; le = ret->GetBinLowEdge(index);}
        if (index>ret->GetNbinsX()) break;
        ret->SetBinContent(index,y);
        tot_entries += y; }
      ret->SetEntries(tot_entries);
      //info_out("KKK"); exit(0);
      return ret; };
    std::map<uint16_t,std::map<uint16_t,std::size_t>> baseline_map;
    auto entries = data_tree->GetEntries();
    for (decltype(entries) i=0, e=entries; i<e; ++i){
      data_tree->GetEntry(i);
      for (std::size_t j=0; j<entry_buffer_ptr->global_ids.size(); ++j){
        auto gid = entry_buffer_ptr->global_ids.at(j);
        if (baseline_map.find(gid)==baseline_map.end()) baseline_map[gid];
        for (auto&& x : entry_buffer_ptr->adcs[j]) baseline_map.at(gid)[x]++;
      }
    }
    std::map<uint16_t,TH1I*> baseline_his;
    gStyle->SetOptFit(1111);
    for (auto&& [x,y] : baseline_map){
      auto lower = std::begin(y)->first;
      auto upper = std::prev(std::end(y),1)->first;
      std::stringstream sstr("");
      sstr<<"baseline-"<<(int)(x>>8)<<"_"<<(int)(x&0xFF);
      baseline_his.emplace(x,fill(sstr.str(),y));
    }
    gStyle->SetOptFit(0);
    fout->cd();
    std::unordered_map<int,std::pair<float,float>> mean_and_rms;
    //std::map<int,std::pair<float,float>> mean_and_rms;
    info_out(prestal_name);
    std::ofstream prestal_txt(prestal_name.c_str());
    prestal_txt<<"#"<<"Prestal File V0.0.1\t"<<util::time_to_str()<<"\n";
    prestal_txt<<"#"<<"Generate by "<<entry_out_file<<"\n";
    prestal_txt<<"#"<<"fecid channelid mean sigma chi2/ndf\n";
    for (auto iter = baseline_his.begin(); iter != baseline_his.end(); ++iter) {
      iter->second->SetDirectory(nullptr);
      iter->second->Rebin(2);
      TF1 f_gaus("f_gaus","gaus"
          ,iter->second->GetMean()-3*iter->second->GetRMS()
          ,iter->second->GetMean()+3*iter->second->GetRMS());
      iter->second->Fit(&f_gaus,"RQ");
      mean_and_rms[iter->first].first=f_gaus.GetParameter(1);
      mean_and_rms[iter->first].second=f_gaus.GetParameter(2);
      prestal_txt<<(int)reco::get_fec_id(iter->first).first<<
        " "<<(int)reco::get_fec_id(iter->first).second
        <<" "<<f_gaus.GetParameter(1)<<" "
        <<f_gaus.GetParameter(2)
        <<" "<<f_gaus.GetChisquare()/f_gaus.GetNDF()
        <<"\n";
    }

    if (use_ini){
      util::generate_configs(argv_map["path"]
          ,compres_table ,mean_and_rms ,std::stof(argv_map["sigma-compres"]));
      util::generate_tt(argv_map["path"],tt_map);
      util::generate_mis(argv_map["path"],mp);
      auto file_replace = util::read_argv("self-def");
      for (auto iter = file_replace.begin(); iter != file_replace.end(); ++iter){
        if (iter->first=="NHitChannel"){
          std::string to = argv_map["path"]+"/NHitChannel.dat";
          int s = gSystem->CopyFile(iter->second.c_str(),to.c_str(),true);
          if (s!=0) std::cerr<<"CopyFile("<<iter->second<<","<<to<<") Failed\n";
        }else if(iter->first=="Baseline"){
          //...
        }
        //...
      }
    }

    std::map<int,std::pair<float,float>> ms_clone;
    for (auto&&[x,y] : mean_and_rms) ms_clone.emplace(x,y);
    auto bin_begin = std::begin(ms_clone)->first;
    auto bin_end = std::prev(std::end(ms_clone),1)->first;
    TH1I mean_dis("mean","mean",bin_end-bin_begin+20,bin_begin-10, bin_end+10);
    TH1F rms_dis("rms","rms",bin_end-bin_begin+20,bin_begin-10,bin_end+10);
    mean_dis.SetDirectory(nullptr); rms_dis.SetDirectory(nullptr);
    std::map<float,int> map_of_rms;
    std::map<float,int> map_of_mean;
    auto iter_ms = std::begin(ms_clone);
    for (int i=1; i<=mean_dis.GetNbinsX();){
      while((int)mean_dis.GetBinLowEdge(i)<iter_ms->first) i++;
      mean_dis.SetBinContent(i,iter_ms->second.first);
      rms_dis.SetBinContent(i,iter_ms->second.second);
      if (iter_ms==std::end(ms_clone)) break;
      ++i; ++iter_ms;
    }

    for (auto [x,y] : mean_and_rms){
      //mean_dis.SetBinContent(x+1,y.first);
      //rms_dis.SetBinContent(x+1,y.second);
      map_of_rms.emplace(y.second,x);
      map_of_mean.emplace(y.first,x);
    }
    mean_dis.Write(); rms_dis.Write();
    for (auto&& [x,y] : baseline_his) y->Write();

    prestal_txt<<"#RMS dis\n";
    for (auto&& [x,y] : map_of_rms)
      prestal_txt<<"# "<<x<<" "<<y<<" "<<(int)(y>>8)<<" "<<(int)(y&0xFF)<<"\n";
    prestal_txt.flush();

    rfin->Close(); fout->Write(); fout->Close();
    prestal_txt.close();
    delete entry_buffer_ptr;

    typedef typename util::terminal_color tc;
    std::cout<<util::terminal_color(
        tc::display_mode::k_underline
        ,tc::f_color::k_white
        ,tc::b_color::k_blue)
      <<"---->PRESTAL<----" <<util::terminal_reset() <<std::endl;
    std::cout<<"Baseline Root Store: "<<baseline_name<<"\n";
  }

step2:
  {
    std::string map_file_name;
    std::string root_file_name;
    std::string prestal_file;

    if (use_ini){
      auto argv = util::read_argv("map");
      if (argv.find("Enable")==argv.end() || argv.at("Enable")!="T")
        goto step3;
      map_file_name=argv["mapname"];
      prestal_file=argv["prestal"];
      root_file_name=argv["filename"];
    }else{
      opt_parser.count("map",map_file_name);
      opt_parser.count("prestal",prestal_file);
      opt_parser.count("rfile",root_file_name);
      if (root_file_name=="<unpack>")
        root_file_name=dat_name.substr(0,dat_name.find_last_of('.'))+"_entry.root";
      int mode;
      opt_parser.count("mode",mode);
      if (mode==0) goto step3;
      
    }
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
        fec2det_map[reco::make_id(uint8_t(a),uint8_t(b))] = 
          reco::make_det_id(uint16_t(c),uint16_t(d));
      }
    }

    std::unordered_map<uint16_t,std::pair<float,float>> prestal_map;
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
        int a, d; float b, c,e;
        sstr>>a>>d>>b>>c>>e;
        prestal_map[reco::make_id(uint8_t(a),uint8_t(d))]=std::make_pair(b,c);
      }
    }
    prestal_fin.close();
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

    std::string fname00 = "filter.root";
    auto* f_view = new TFile(fname00.c_str(),"recreate");
    info_out(data_tree->GetEntries());
    for (long long i=0; i<data_tree->GetEntries(); ++i){
      data_tree->GetEntry(i);
      cluster_x->Clear();
      cluster_y->Clear();
      reco::hit_positon(i
          ,entry_buffer_ptr,fec2det_map,prestal_map
          ,cluster_x,cluster_y
          //,f_view);
          ,nullptr);
      tree->Fill();
      
    }
    fout->cd();
    tree->Write();
    fout->Write();fout->Close();
    typedef typename util::terminal_color tc;

    f_view->Write();
    f_view->Close();
    using namespace util;
    std::cout<<terminal_color(
        tc::display_mode::k_underline
        ,tc::f_color::k_white
        ,tc::b_color::k_blue)
      <<"fec2det MAP" <<util::terminal_reset() <<std::endl;
    std::cout<<"Reco Root Store: "<<fout_name<<"\n";
  }

step3:

  return 0;
}

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
#include "TH2D.h"
#include "TF1.h"
#include "TSystem.h"

#include "util.hpp"
#include "clusters.hpp"
#include "data_strcut_cint.h"
#include "unpack.h"
#include "wave_handler.h"

#include <QApplication>
#include "mainwindow.h"

static bool use_ini=false;
static bool use_gui=false;

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

using std::cout; using std::endl; using std::string; using std::vector;
int main(int argc, char* argv[]){
  util::getopt opt_parser;
  opt_parser.run(argc,argv);
  std::string dat_name;
  if (opt_parser.vm.count("GUI")
      ) use_gui=true;
  if (use_gui) {
    QApplication app(argc,argv);
    mainwindow window;
    window.show();
    return app.exec();
  }

  if (opt_parser.vm.count("config")){
    use_ini = true;
    goto step0;
  }


step0:
  {
    int fec_count;

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
    std::size_t oneG = (std::size_t)1000*1024*1024;
    fsz = fsz>oneG? oneG : fsz;
    info_out(fsz);
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
    std::cout<<"Raw Root Store: "<<entry_out_file<<"\n";
    typedef typename util::terminal_color tc;
    using namespace util;
    std::cout<<terminal_color(
        tc::display_mode::k_underline
        ,tc::f_color::k_white
        ,tc::b_color::k_blue)
      <<"---->UNPACK DONE<----" <<util::terminal_reset() <<std::endl;
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
          int global_ids=((uint16_t)ps.layer_id<<8)+(uint16_t)ps.channel_id;
          compres_table[global_ids]=ps.compres;
        }
        if (x.first.substr(0,2)=="TT" && x.second=="T"){
          auto ps = util::parse_lc_adc(x.first);
          uint16_t global_ids=((uint16_t)ps.layer_id<<8)+(uint16_t)ps.channel_id;
          tt_map[global_ids] = ps.adc_t;
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
    std::map<int,TH1I*> baseline_map;
    for (long long i=0; i<data_tree->GetEntries(); ++i){
      data_tree->GetEntry(i);
      for (int j=0; j<entry_buffer_ptr->global_ids.size(); ++j){
        int x = entry_buffer_ptr->global_ids.at(j);
        if (baseline_map.find(x)==baseline_map.end()){
          std::stringstream sstr("");
          sstr<<"baseline-"<<(int)(x>>8)<<"_"<<(int)(x&0xFF);
          auto const& ref = entry_buffer_ptr->adcs[j];
          uint16_t min = *util::min_element(std::begin(ref),std::end(ref));
          uint16_t max = *util::max_element(std::begin(ref),std::end(ref));
          min -= 10; max += 10;
          baseline_map[x] = new TH1I(sstr.str().c_str(),sstr.str().c_str()
              ,max-min,min,max);
        }
        for (auto&& adc : entry_buffer_ptr->adcs[j]){ baseline_map[x]->Fill(adc); }
      }
    }
    fout->cd();
    std::unordered_map<int,std::pair<float,float>> mean_and_rms;


    info_out(prestal_name);
    std::ofstream prestal_txt(prestal_name.c_str());
    prestal_txt<<"#"<<"Prestal File V0.0.1\t"<<util::time_to_str()<<"\n";
    prestal_txt<<"#"<<"Generate by "<<entry_out_file<<"\n";
    prestal_txt<<"#"<<"fecid channelid mean sigma chi2/ndf\n";
    for (auto iter = baseline_map.begin(); iter != baseline_map.end(); ++iter) {
      TF1 f_gaus("f_gaus","gaus"
          ,iter->second->GetMean()-3*iter->second->GetRMS()
          ,iter->second->GetMean()+3*iter->second->GetRMS());
      iter->second->Fit(&f_gaus,"RQ");
      mean_and_rms[iter->first].first=iter->second->GetMean();
      mean_and_rms[iter->first].second=iter->second->GetRMS();
      prestal_txt<<(int)reco::get_fec_id(iter->first).first<<
        " "<<(int)reco::get_fec_id(iter->first).second
        <<" "<<f_gaus.GetParameter(1)<<" "<<f_gaus.GetParameter(2)
        <<" "<<f_gaus.GetChisquare()/f_gaus.GetNDF()
        <<"\n";
    }
    prestal_txt.close();

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
    TH1I mean_dis("mean","mean",1800,0,1800);
    TH1F rms_dis("rms","rms",1800,0,1800);
    for (auto [x,y] : mean_and_rms){
      mean_dis.SetBinContent(x,y.first);
      rms_dis.SetBinContent(x,y.second); }
    mean_dis.Write(); rms_dis.Write();
    rfin->Close(); fout->Write(); fout->Close();
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

    for (long long i=0; i<data_tree->GetEntries(); ++i){
      data_tree->GetEntry(i);
      cluster_x->Clear();
      cluster_y->Clear();
      reco::hit_positon(i
          ,entry_buffer_ptr,fec2det_map,prestal_map
          ,cluster_x,cluster_y);
      tree->Fill();
      
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

step3:

  return 0;
}

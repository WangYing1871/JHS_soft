//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: wy187110@mail.ustc.edu.cn     
//^-^ Time: 2024-11-20 16:15:18   Posi: Hefei
//^-^ File: draw_time_dis.cpp
//--------------------------------------------------------------------
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"

#include "data_strcut_cint.h"

using std::cout; using std::endl;
using std::string; using std::vector;
int main(int argc, char* argv[]){
  std::string fname = argv[1];
  auto* fin = new TFile(fname.c_str());
  auto* tree = (TTree*)fin->Get("CollectionTree");
  entry_new* data = new entry_new;
  tree->SetBranchAddress("data",std::addressof(data));

  std::multimap<uint64_t,uint16_t> tm_vs_adc;
  auto entries = tree->GetEntries();
  for (int i=0; i<entries; ++i){
    tree->GetEntry(i);
    std::map<int,uint64_t> ts_map;
    for (std::size_t index=0; auto&& x : data->fec_ids) ts_map[x] = data->time_stamps[index++];
    for (std::size_t index=0; auto&& x : data->global_ids){
      auto const& adcs = data->adcs[index++];
      int fec_id = x>>8;
      //int channel_id = x&0xFF;
      uint64_t ts = ts_map.at(fec_id);
      auto max = std::max_element(std::begin(adcs),std::end(adcs));
      uint16_t max_position = std::distance(std::begin(adcs),max);
      uint64_t ts_peak = std::round(ts*8.33) + (max_position-624)*25;
      tm_vs_adc.emplace(ts_peak,*max);
    }
  }
  for (auto&& [x,y] : tm_vs_adc){
    std::cout<<x<<" "<<y<<std::endl;
  }

  std::cout<<tm_vs_adc.size()<<std::endl;
  return 0;
}

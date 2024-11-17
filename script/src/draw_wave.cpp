//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: wy187110@mail.ustc.edu.cn     
//^-^ Time: 2024-10-31 15:55:10   Posi: Hefei
//^-^ File: draw_wave.cpp
//--------------------------------------------------------------------
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>

#include "TTree.h"
#include "TFile.h"
#include "TFolder.h"
#include "TGraph.h"
#include "data_strcut_cint.h"

//std::map<int,std::vector<uint16_t>> pix_map;


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

using std::cout; using std::endl; using std::string; using std::vector;
int main(int argc, char* argv[]){
  {
    std::string fname = argv[1];
    auto* fin = new TFile(fname.c_str());
    auto* tree = (TTree*)fin->Get("CollectionTree");
    entry_new* data = new entry_new;
    tree->SetBranchAddress("data",std::addressof(data));

    auto* fout = new TFile("wave.root","recreate");
    fout->cd();

    auto entries = tree->GetEntries();

    //for (int i=0; i<tree->GetEntries(); ++i){
    for (int i=0; i<(entries>200 ? 200 : entries); ++i){
    //for (int i=200; i<300; ++i){
      tree->GetEntry(i);
      std::stringstream sstr("");
      sstr<<"Event-"<<i;
      waves_view(sstr.str(),data)->Write();
    }
    fout->Write(); fout->Close();
    fin->Close();
  }
  return 0;
}

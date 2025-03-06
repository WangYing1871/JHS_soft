#include <unordered_map>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "util.h"

std::string version(){
  return "0.0.3";
}

namespace util{

id_trans::id_trans(std::string const& nm){
  namespace fs = std::filesystem;
  if (!fs::exists(nm) && !fs::is_regular_file(nm)){
    info_out("invalid_argument: invaild map file");
    return; }
  std::ifstream map_file(nm.c_str());
  while(!map_file.eof()){
    std::string sbuf;
    std::getline(map_file,sbuf);
    if (!sbuf.empty() && sbuf[0] != '#'){
      std::stringstream sstr(sbuf.c_str());
      int a, b, c, d;
      sstr>>a>>b>>c>>d;
      m_fec2det_map[std::make_pair(a,b)] = std::make_pair(c,d);
      //if (m_fec2det_map.find(std::make_pair(a,b)) != m_fec2det_map.end()){
      //  info_out("CF");
      //  info_out(m_fec2det_map.at(std::make_pair(a,b)).first);
      //  info_out(m_fec2det_map.at(std::make_pair(a,b)).second);
      //}
    }
  }
  map_file.close();
  //info_out(m_fec2det_map.size());
}

det_id_t id_trans::tran(fec_id_t const& v) const{
  static std::unordered_map<uint32_t, std::pair<int16_t,int16_t>> cnt_map ={
    {7,{0,0}}
    ,{4,{0,1}}
    ,{5,{0,2}}
    ,{2,{0,3}}
    ,{8,{0,4}}
    ,{6,{1,0}}
    ,{9,{1,1}}
    ,{0,{1,2}}
    ,{3,{1,3}}
    ,{1,{1,4}}
  };
  det_id_t ret;
  ret.m_layer_id = cnt_map.at(v.first).first;
  auto fec_id = std::pair<uint16_t,uint16_t>{cnt_map.at(v.first).second,v.second};
  //info_out(m_fec2det_map.size());
  if (auto iter = m_fec2det_map.find(fec_id); iter != m_fec2det_map.end()){
    ret.m_dimension_id = iter->second.first;
    ret.m_channel_id = iter->second.second;
    return ret;
  }else{
    info_out("EPT");
    info_out(fec_id.first);
    info_out(fec_id.second);
    exit(0);
  }
  info_out("WARN...");
  return {};
}

}
#include <cassert>
#include "TGraph.h"
#include "TFile.h"
#include "TF1.h"
namespace util{
double get_x(double x0, double y0, double x1, double y1, double z){
  if ((y0-z)*(y1-z)>0) return std::nan("");
  return x0+(x1-x0)*(z-y0)/(y1-y0); }


std::pair<bool,double> fit_wave(TGraph* grp, double* params
    ,double rate, double& arrive_time){
  if (!grp) return std::make_pair(false,std::nan(""));
  constexpr double const p12pk = 27.*std::exp(-3.);
  constexpr std::size_t const bf_max_pnts = 120;
  constexpr std::size_t const af_max_pnts = 10;
  auto const& get_range_and_feature = [=](TGraph* grp, double* p, double* kb = nullptr)
    ->std::pair<int,int>{
    assert(grp);
    std::vector<double> gdata(grp->GetN());
    double _, bufy;
    for (std::size_t i=0, e=grp->GetN(); i<e; ++i)
      grp->GetPoint(i,_,bufy), gdata[i] = bufy;
    auto iter_max = std::max_element(std::begin(gdata),std::end(gdata));
    auto peak = *iter_max;
    auto max_pos = std::distance(std::begin(gdata),iter_max);
    if (max_pos<bf_max_pnts) return std::make_pair(-1,max_pos+af_max_pnts);
    double p0 = std::accumulate(
        std::next(std::begin(gdata),0)
        ,std::next(std::begin(gdata),150)
        ,std::accumulate(
          std::prev(std::end(gdata),150)
          ,std::next(std::end(gdata),0)
          ,0.f)
        )/300.;
    double p1 = (peak-p0)/(27.*std::exp(-3.));
    std::size_t index;
    double hp = (p0+peak)*0.5;
    for (index=max_pos; index>=max_pos-bf_max_pnts; --index)
      if ((gdata[index]-hp)*(gdata[index-1]-hp)<=0) break;
   // if (index==max_pos);
    double hp2 = get_x(index-1,gdata.at(index-1),index,gdata.at(index),hp);
    double k = gdata.at(index)-gdata.at(index-1);
    double p2 = (p0-(hp-k*hp2))/k;
    double p3 = (max_pos-p2)/3.;
    p[0] = p0; p[1] = p1; p[2] = p2; p[3] = p3;
    if (kb) kb[0] = k, kb[1] = hp-k*hp2;
    return std::make_pair(max_pos-bf_max_pnts, max_pos+af_max_pnts);
  };
  auto const& fit_fun = [](double* x, double* p)->double{
    double buf = (x[0]-p[2])/p[3];
    return x[0]<p[2] ? p[0] : p[0]+p[1]*std::pow(buf,3)*std::exp(-buf); };
  double ft[4];
  auto fit_range = get_range_and_feature(grp,ft);
  if (fit_range.first==-1) return std::make_pair(false,std::nan(""));
  float max_position = fit_range.first+bf_max_pnts;
  TF1 f("f",fit_fun,fit_range.first,fit_range.second,4);
  //std::cout<<"ft:"<<ft[0]<<";"<<ft[1]<<";"<<ft[2]<<";"<<ft[3]<<std::endl;
  //f.SetParameter(0,ft[0]); f.SetParLimits(0,550,780);
  //f.SetParameter(1,ft[1]); f.SetParLimits(1,20,4095);
  //f.SetParameter(2,ft[2]); f.SetParLimits(2,320,470);
  //f.SetParameter(3,ft[3]); f.SetParLimits(3,1,40);
  f.SetParameter(0,ft[0]); f.SetParLimits(0,ft[0]-70,ft[0]+70);
  f.SetParameter(1,ft[1]); f.SetParLimits(1,ft[1]*0.7,ft[1]*1.5);
  f.SetParameter(2,ft[2]); f.SetParLimits(2,ft[2]-30,ft[2]+70);
  f.SetParameter(3,ft[3]); f.SetParLimits(3,0.3*ft[3],5*ft[3]);
  grp->Fit(&f,"RQ");
  double p0 = f.GetParameter(0); double p1 = f.GetParameter(1);
  double p2 = f.GetParameter(2); double p3 = f.GetParameter(3);
  params[0] = p0; params[1] = p1; params[2] = p2; params[3] = p3;
  double zpos = p0+rate*p12pk*p1;
  double step = 0.01;
  double index=0.;
  for (double index=p2; index<=p2+3.*p3; index+=step){
    double y0 = f.operator()(index);
    double y1 = f.operator()(index+step);
    if ((y0-zpos)*(y1-zpos)<=0){ arrive_time = get_x(index,y0,index+step,y1,zpos); break; }
  }
  if (index==p2+3*p3) arrive_time = std::nan("");

  return std::make_pair(
      max_position>=f.GetParameter(2)+2
      ,f.GetChisquare()/f.GetNDF()
      );
}

bool t_object_factory_v0::unregister_element(std::string const& /* name */){
  return false;
}

t_object_factory_v0& t_object_factory_v0::write_to(TFile* fout, std::string const& names){
  if (!fout) return *this;
  std::regex reg(names.c_str());
  fout->cd();
  for(auto&& [x,y] : m_elements) if (std::regex_match(x,reg)) y->Write();
  return *this;
}
}

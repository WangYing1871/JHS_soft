//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng           Email: 2835516101@qq.com     
//^-^ Time: 2025-02-25 14:49:30   Posi: JW company@AnHui Soft.D
//^-^ File: track_seek.cpp
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
#include <filesystem>
#include <vector>
#include <sstream>
#include <type_traits>
#include <cassert>
#include <random>
#include <functional>
#include <optional>
#include <set>
#include <iomanip>


#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TTree.h"
#include "TFolder.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TPad.h"

#include "util.h"
#include "data_strcut_cint.h"

#include <eigen3/Eigen/Dense>
namespace util{
std::optional<std::pair<float,float>>
weight_least_squart_line_fit(
    float* dx
    ,float* dy
    ,float* dw
    ,std::size_t dsz
    ){
  if (dsz<2) return std::nullopt;
  Eigen::MatrixXd mx(dsz,2);
  Eigen::VectorXd my(dsz);
  Eigen::MatrixXd mw = Eigen::MatrixXd::Zero(dsz,dsz);
  for (int i=0; i<dsz; ++i){
    mx(i,0) = dx[i], mx(i,1) = 1;
    my(i) = dy[i];
    mw(i,i) = dw[i];
  }
  auto coeff = (mx.transpose()*mw*mx).inverse()*mx.transpose()*mw*my;
  return std::make_optional(
      std::make_pair(coeff(0),coeff(1))
      );
}
}
static std::default_random_engine s_reng;
constexpr static const float s_r2a = 45./std::atan(1);
constexpr static const float s_strips_width = 0.65;
constexpr static const float s_dfift_velocity = 8.94/1000.;
 
namespace service{
struct point_t{
  double x;
  double y;
  point_t(double x, double y):x(x),y(y) {}
  ~point_t() noexcept = default;
};
//bool add_points(point_t* pnts, std::size_t sz, TH1F* his=nullptr, TH1F* his_z=nullptr){
//  if (sz<4) return false;
//  auto const& sample = [=](std::size_t number){
//    std::set<std::size_t> cna;
//    std::uniform_int_distribution<std::size_t> di(0,sz-1);
//    while(cna.size()!=number) cna.insert(di(s_reng));
//    return cna; };
//  std::vector<float> px, py;
//  std::vector<float> sps_px, sps_py;
//  auto sp = sample(2);
//  for(int i=0; i<sz; ++i){
//    if (sp.find(i)!=sp.end()) 
//      sps_px.emplace_back(pnts[i].x), sps_py.emplace_back(pnts[i].y);
//    else px.emplace_back(pnts[i].x), py.emplace_back(pnts[i].y);
//  }
//  auto kb = util::least_squart_line_fit(
//      std::begin(px), std::end(px)
//      ,std::begin(py), std::end(py)
//      );
//  if (kb){
//    double k = kb->first; double b = kb->second;
//    for (std::size_t i=0; i<sps_px.size(); ++i){
//      if (his) his->Fill((sps_px.at(i)-((sps_py.at(i)-b)/k))*std::sqrt(k*k/(1+k*k)));
//      if (his_z){
//        double dz = sps_py.at(i)-k*sps_px.at(i)-b;
//        //info_out(dz);
//        his_z->Fill(dz);
//      }
//    }
//
//    return true;
//  }
//  return false;
//}


std::optional<double> get_first_zero(
    TF1* fptr, double from, double to, double zp,double step=0.01){
  if (!fptr || step<1.e-6 || from<=to) return std::nullopt;
  double start = fptr->operator()(from);
  for (double i=from, e=to-step; i<e; i+=step){
    double y0 = fptr->operator()(i);
    double y1 = fptr->operator()(i+step);
    if ((y0-zp)*(y1-zp)<=0){
      return i+((y1-y0)/step)*(zp-y0);
    }
  }
  return std::nullopt;
}

inline float distance_p2l(float x, float y, float k, float b){
  return (k*x-y+b)/std::sqrt(k*k+1); }

std::ostream& display_entry(size_t evt_id,
    std::vector<entry_strip> const& hits, std::ostream& os = std::cout){
  os<<std::fixed<<std::setprecision(2);
  os<<"ID: "<<evt_id;
  os<<"\nlayer_id: "; for (auto&& x : hits) os<<(int)x.layer_id<<" ";
  os<<"\ndim_id: "; for (auto&& x : hits) os<<(int)x.dim_id<<" ";
  os<<"\nchannel_id: "; for (auto&& x : hits) os<<(int)x.channel_id<<" ";
  os<<"\npeak: "; for (auto&& x : hits) os<<x.peak<<" ";
  os<<"\npeak_position: "; for (auto&& x : hits) os<<x.peak_position<<" ";
  os<<"\np0: "; for (auto&& x : hits) os<<x.p0<<" ";
  os<<"\np1: "; for (auto&& x : hits) os<<x.p1<<" ";
  os<<"\np2: "; for (auto&& x : hits) os<<x.p2<<" ";
  os<<"\np3: "; for (auto&& x : hits) os<<x.p3<<" ";
  os<<"\nmean: "; for (auto&& x : hits) os<<x.mean<<" ";
  return os<<"\n";
}



}
namespace user{
inline double get_x(double x0, double y0, double x1, double y1, double z){
  if ((y0-z)*(y1-z)>0) return std::nan("");
  return x0+(x1-x0)*(z-y0)/(y1-y0); }

template <class _tp, class _up>
std::pair<double,double> fit_pol1(
    _tp first, _tp last
    ,_up yfirst, _up ylast
    ,double* cdn = nullptr
    ){
  auto minx = *std::min_element(first,last);
  auto miny = *std::max_element(first,last);
  auto const& fit_core =[](double* x, double* p){ return x[0]*p[0]+p[1]; };
  TF1 f("f",fit_core,minx,miny,2);
  f.SetParameter(0,1);
  f.SetParameter(1,0);
  TGraph fit_g;
  std::size_t index=0;
  for(auto iter = first; iter != last; ++iter)
    fit_g.SetPoint(index++,*iter,*yfirst++);
  fit_g.Fit(&f,"RQ");
  if (cdn) cdn[0] = f.GetChisquare()/f.GetNDF();
  return std::make_pair(f.GetParameter(0),f.GetParameter(1));
}

bool is_all_one_layer(std::vector<entry_strip> const& hits){
  assert(!hits.empty());
  if (hits.size()<=1) return true;
  auto id = hits.at(0).layer_id;
  return std::find_if(std::next(std::begin(hits)),std::end(hits)
      ,[&](auto a){return a.layer_id!=id;})==std::end(hits);
}
template <class _tp>
std::ostream& display(_tp* ptr, std::ostream& os = std::cout){
  if (!ptr) return os;
  if constexpr(std::is_same<_tp,entry_strip>::value){
    os<<(int)ptr->layer_id<<" "<<(int)ptr->dim_id<<" "
      <<ptr->channel_id<<" "<<ptr->time_stamp<<" "
      <<ptr->raw_evt_id
      <<"\n";
    os<<ptr->p0<<" "<<ptr->p1<<" "<<ptr->p2<<" "<<ptr->p3<<"\n";
  }
  return os;
}

std::size_t get_arrive_time(entry_strip const* ptr){
  if (!ptr) return 0;
  //return std::round(8.33*ptr->time_stamp)+(ptr->p2-624)*25;
  return std::round(8.33*ptr->time_stamp)+(ptr->peak_position-624)*25;
}
double get_peak(entry_strip const* ptr){
  if (!ptr) return -1.;
  //std::cout
  //  <<" "<<ptr->p0
  //  <<" "<<ptr->p1
  //  <<" "<<ptr->p2
  //  <<" "<<ptr->p3
  //  <<std::endl;
  return ptr->p0+ptr->p1*(27.*std::exp(-3.)); }


double get_arrive_time(entry_strip const& hit, double rate=0.2){
  if (hit.is_fit==0) return -1.;
  float ret=0.;
  double p[4] = {hit.p0,hit.p1,hit.p2,hit.p3};
  auto const& fit_fun = [](double* x, double* p)->double{
    double buf = (x[0]-p[2])/p[3];
    return x[0]<p[2] ? p[0] : p[0]+p[1]*std::pow(buf,3)*std::exp(-buf); };
  TF1 f_wave("f_wave",fit_fun,p[2],p[2]+3*p[3],4);
  f_wave.SetParameters(p);
  //std::cout<<p[0]<<" "<<p[1]<<" "<<p[2]<<" "<<p[3]<<std::endl;
  double index=0.;
  double step = 0.01;
  double zpos = rate*27.*std::exp(-3.)*p[1]+p[0];
  double xpos;
  for (index=p[2]; index<=p[2]+3.*p[3]; index+=step){
    double y0 = f_wave.operator()(index);
    double y1 = f_wave.operator()(index+step);
    if ((y0-zpos)*(y1-zpos)<=0){ xpos = get_x(index,y0,index+step,y1,zpos); break;} }
  if (index==p[2]+3) {return -1; }
  return hit.time_stamp*8.33+(xpos-624.)*25.;

}
std::size_t get_arrive_time(double p0, double p1, double p2, double p3
    ,std::size_t start_ts){
  auto max_position = 3*p2+p3;
  auto const& fit_fun = [](double* x, double* p)->double{
    double buf = (x[0]-p[2])/p[3];
    return x[0]<p[2] ? p[0] : p[0]+p[1]*std::pow(buf,3)*std::exp(-buf); };
  TF1 f_wave("f_wave",fit_fun,max_position-150<0 ? 0 : max_position-150,max_position+10,4);
  f_wave.SetParameter(0,p0);
  f_wave.SetParameter(1,p1);
  f_wave.SetParameter(2,p2);
  f_wave.SetParameter(3,p3);
  return std::round(start_ts*8.33)+(p2-624)*25;
}

void get_kx_ky(std::vector<entry_strip> const& hits
    ,double& kx
    ,double& bx
    ,double& ky
    ,double& by
    ,double& cdn_x
    ,double& cdn_y
    ,std::vector<service::point_t>& pxs
    ,std::vector<service::point_t>& pys
    ){
  kx = std::nan("");
  ky = std::nan("");
  cdn_x = std::nan("");
  cdn_y = std::nan("");
  pxs.clear();
  pys.clear();
  std::size_t zero_tm = hits[0].relative_arraive_time;
  std::map<std::uint16_t,std::size_t> xz;
  std::map<std::uint16_t,std::size_t> yz;
  for (auto&& x : hits){
    if (x.p0==0) continue;
    auto arv_tm = get_arrive_time(&x);
    if (x.dim_id==0) xz.emplace(x.channel_id,x.relative_arraive_time);
    else if (x.dim_id==1) yz.emplace(x.channel_id,x.relative_arraive_time);
  }
  for (auto&& [x,y] : xz) y-=zero_tm;
  for (auto&& [x,y] : yz) y-=zero_tm;
  if (xz.size()>10){
    xz.erase(std::begin(xz));
    std::vector<uint16_t> px;
    std::vector<std::size_t> pz;
    for (auto&& [x,y] : xz) {
      px.emplace_back(x), pz.emplace_back(y);
      pxs.emplace_back(service::point_t{(double)x,(double)y});
    }
    double cdn;
    auto kb_rt = fit_pol1(
        std::begin(px),std::end(px)
        ,std::begin(pz),std::end(pz)
        ,&cdn
        );
    kx = kb_rt.first;
    bx = kb_rt.second;
    cdn_x = cdn;
    //if (his_rx){
    //  double k = kb_rt.first;
    //  double b = kb_rt.second;
    //  //for (std::size_t index = 0; auto&& x : px){
    //  //  his_rx->Fill(x-(pz[index++]-b)/k)*std::sqrt(k*k/(1+k*k));
    //  //}
    //}
  }
  if (yz.size()>5){
    yz.erase(std::begin(yz));
    std::vector<uint16_t> px;
    std::vector<std::size_t> pz;
    for (auto&& [x,y] : yz) {
      px.emplace_back(x), pz.emplace_back(y);
      pys.emplace_back(service::point_t{(double)x,(double)y});
    }
    double cdn;
    auto kb_rt = fit_pol1(
        std::begin(px),std::end(px)
        ,std::begin(pz),std::end(pz)
        ,&cdn
        );
    ky = kb_rt.first;
    by = kb_rt.second;
    cdn_y = cdn;
  }
  //info_out(xz.size());
  //info_out(yz.size());
}


double get_sum_adc(std::vector<entry_strip> const& hits){
  double ret=0.;
  for (auto&& x : hits) {
    if (x.p0<600 && x.p3>30) continue;
    //if (x.dim_id==0 && x.channel_id<270) continue;
    //if (x.dim_id==0 && x.channel_id>270) continue;
    //ret += get_peak(&x);
    ret += get_peak(&x)-x.mean;
    //ret += x.peak-x.mean;
    //exit(0);
  }
  return ret;

}
typedef std::vector<entry_strip> detector_entry_t;
struct detector_hits_t{
  typedef detector_hits_t self_t;
  uint16_t hit_strip_x;
  uint16_t hit_strip_y;
  std::uint64_t m_event_id=0;
  double m_edep = 0.;
  double m_edep_fit = 0.;
  std::map<uint16_t,std::pair<double,double>> m_strip_x;
  std::map<uint16_t,std::pair<double,double>> m_strip_y;
  detector_hits_t() = default;
  ~detector_hits_t() noexcept = default;
  void set_hits(std::uint64_t evt_id, detector_entry_t const& raw){
    m_event_id = evt_id;
    if (raw.size()<2) return;
    auto iter_zero = std::find_if(std::begin(raw),std::end(raw),
        [this](auto a){return std::invoke(m_filter,a);});
    double zero_tm = iter_zero->relative_arraive_time;
    for (auto&& x : raw){
      if (!std::invoke(m_filter,x)) continue;
      double arv_tm = x.relative_arraive_time;
      auto pk = x.peak-x.mean;
      auto pk1 = get_peak(&x)-x.p0;
      m_edep += pk;
      m_edep_fit += pk1;
      if (x.dim_id==0){ m_strip_x.emplace(x.channel_id,std::make_pair(arv_tm,pk1)); }
      if (x.dim_id==1){ m_strip_y.emplace(x.channel_id,std::make_pair(arv_tm,pk1)); }
    }
    for(auto&& [x,y] : m_strip_x) y.first-=zero_tm;
    for(auto&& [x,y] : m_strip_y) y.first-=zero_tm;
  }

  std::optional<std::pair<uint16_t,uint16_t>> hit_position() const{
    if (m_strip_x.size()<2 || m_strip_y.size()<2) return std::nullopt;
    double sum_adc_x=0.;
    for (auto&& [_,y] : m_strip_x) sum_adc_x += y.second;
    double sum_adc_y=0.;
    for (auto&& [_,y] : m_strip_y) sum_adc_y += y.second;
    auto iter_max_dft_x = std::max_element(
        std::begin(m_strip_x)
        ,std::end(m_strip_x)
        ,[](auto a, auto b) {return a.second.first<b.second.first;}
        );
    auto iter_max_dft_y = std::max_element(
        std::begin(m_strip_y)
        ,std::end(m_strip_y)
        ,[](auto a, auto b) {return a.second.first<b.second.first;}
        );
    return std::make_optional(
        std::make_pair(iter_max_dft_x->first,iter_max_dft_y->first)
        );
  }

  void draw_xz_yz(TPad* padx, TPad* pady){
    std::stringstream sstr(""); sstr<<"Event_"<<m_event_id<<"_XZ";
    auto* his_xz = new TH2F(sstr.str().c_str(),sstr.str().c_str()
        ,320,0,320,250,0,25);
    his_xz->SetDirectory(nullptr);
    sstr = std::stringstream(""); sstr<<"Event_"<<m_event_id<<"_YZ";
    auto* his_yz = new TH2F(sstr.str().c_str(),sstr.str().c_str()
        ,320,0,320,250,0,25);
    his_yz->SetDirectory(nullptr);
    for (auto&& [x,y] : m_strip_x){
      his_xz->Fill(x,y.first*s_dfift_velocity,y.second);
    }
    for (auto&& [x,y] : m_strip_y){
      his_yz->Fill(x,y.first*s_dfift_velocity,y.second);
    }
    if (padx){
      padx->cd(); his_xz->Draw("COLZ");
    }
    if (pady){
      pady->cd(); his_yz->Draw("COLZ");
    }
  }

  float get_angle_xz(float thr = 0.01) const{
    std::vector<float> data_x;
    std::vector<float> data_y;
    double ex = 0.; for (auto&& [_,x] : m_strip_x) ex += x.second;
    for (auto&&[x,y] : m_strip_x){
      if (y.second/ex>thr){
        data_x.emplace_back(x*s_strips_width);
        data_y.emplace_back(y.first*s_dfift_velocity);
      }
    }
    if (data_x.size()<2) return -10000.;
    //auto kb = *util::least_squart_line_fit(
    //    std::begin(data_x),std::end(data_x)
    //    ,std::begin(data_y),std::end(data_y)
    //    );
    auto kb = *util::least_squart_line_fit(
        std::begin(data_x),std::end(data_x)
        ,std::begin(data_y),std::end(data_y)
        );
    float ck = std::atan(kb.first)*s_r2a;
    //if (ck>60.){
    //  auto kb = *util::least_squart_line_fit(
    //      std::begin(data_y),std::end(data_y)
    //      ,std::begin(data_x),std::end(data_x)
    //      );
    //  return 90-std::atan(kb.first)*s_r2a;
    //}
    //if (ck<-60){
    //  auto kb = *util::least_squart_line_fit(
    //      std::begin(data_y),std::end(data_y)
    //      ,std::begin(data_x),std::end(data_x)
    //      );
    //  return -90-std::atan(kb.first)*s_r2a;
    //}
    return ck;
  }
  float get_angle_yz(float thr = 0.01) const{
    std::vector<float> data_x;
    std::vector<float> data_y;
    double ey = 0.; for (auto&& [_,x] : m_strip_y) ey += x.second;
    for (auto&& [x,y] : m_strip_x){
      if (y.second/ey>thr){
        data_x.emplace_back(x*s_strips_width);
        data_y.emplace_back(y.first*s_dfift_velocity);
      }
    }
    if (data_x.size()<2) return -10000.;
    auto kb = *util::least_squart_line_fit(
        std::begin(data_x),std::end(data_x)
        ,std::begin(data_y),std::end(data_y)
        );
    float ck = std::atan(kb.first)*s_r2a;
    //if (ck>60.){
    //  auto kb = *util::least_squart_line_fit(
    //      std::begin(data_y),std::end(data_y)
    //      ,std::begin(data_x),std::end(data_x)
    //      );
    //  return 90-std::atan(kb.first)*s_r2a;
    //}
    //if (ck<-60){
    //  auto kb = *util::least_squart_line_fit(
    //      std::begin(data_y),std::end(data_y)
    //      ,std::begin(data_x),std::end(data_x)
    //      );
    //  return -90-std::atan(kb.first)*s_r2a;
    //}
    return ck;
  }


  inline float get_drift_distance_xz() const{
    auto iter_max_dft_x = std::max_element(
        std::begin(m_strip_x)
        ,std::end(m_strip_x)
        ,[](auto a, auto b) {return a.second.first<b.second.first;}
        );
    auto iter_min_dft_x = std::max_element(
        std::begin(m_strip_x)
        ,std::end(m_strip_x)
        ,[](auto a, auto b) {return a.second.first>b.second.first;}
        );
    float min_z = iter_min_dft_x->second.first*s_dfift_velocity;
    float max_z = iter_max_dft_x->second.first*s_dfift_velocity;
    return max_z-min_z;
  }
  inline float get_drift_distance_yz() const{
    auto iter_max_dft_y = std::max_element(
        std::begin(m_strip_y)
        ,std::end(m_strip_y)
        ,[](auto a, auto b) {return a.second.first<b.second.first;}
        );
    auto iter_min_dft_y = std::max_element(
        std::begin(m_strip_y)
        ,std::end(m_strip_y)
        ,[](auto a, auto b) {return a.second.first>b.second.first;}
        );
    float min_z = iter_min_dft_y->second.first*s_dfift_velocity;
    float max_z = iter_max_dft_y->second.first*s_dfift_velocity;
    return max_z-min_z;
  }

  void fill_energy_dis(TH1F* hisx, TH1F* hisy) const{
    if (!hisx || !hisy) return;
    double ex = 0.; for (auto&& [_,x] : m_strip_x) ex += x.second;
    double ey = 0.; for (auto&& [_,x] : m_strip_y) ey += x.second;
    //info_out(ex);
    //info_out(ey);
    for (auto&& [_,x] : m_strip_x){
      //info_out(x.second);
      hisx->Fill(x.second/ex);
    }
    for (auto&& [_,x] : m_strip_y){
      //info_out(x.second);
      hisy->Fill(x.second/ey);
    }
  }

  void add_points(
      TH1F* hisx = nullptr
      ,TH1F* hisy = nullptr){
    float thr = 0.02;
    auto const& sample = [](std::size_t number
        ,std::size_t from, std::size_t to){
      std::set<std::size_t> cna;
      std::uniform_int_distribution<std::size_t> di(from,to);
      while(cna.size()!=number) cna.insert(di(s_reng));
      return cna; };
    if (hisx){
      if (m_strip_x.size()<8) return;
      float ex = 0.; for (auto&&[_,x] : m_strip_x) ex += x.second;
      std::map<uint16_t,std::pair<double,double>> x_strips;
      for(auto iter = std::next(std::begin(m_strip_x),2);
          iter != std::prev(std::end(m_strip_x),2); ++iter)
        x_strips.emplace(*iter);

      std::vector<float> data_x;
      std::vector<float> data_y;
      std::vector<float> weights;
      std::vector<float> sp_data_x;
      std::vector<float> sp_data_y;
      auto sample_idx = sample(2,0,x_strips.size()-1);
      std::size_t index=0;
      for (int i=0; i<x_strips.size(); ++i){
        auto iter = std::next(std::begin(x_strips),i);
        if (sample_idx.find(i) != sample_idx.end()
            && iter->second.second/ex>=thr
            ){
          sp_data_x.emplace_back(iter->first*s_strips_width);
          sp_data_y.emplace_back(iter->second.first*s_dfift_velocity);
        }
        else if(iter->second.second/ex>=thr){
          data_x.emplace_back(iter->first*s_strips_width);
          data_y.emplace_back(iter->second.first*s_dfift_velocity);
          weights.emplace_back(iter->second.second/ex);
        }
      }
      //auto kb_w = util::least_squart_line_fit(
      //    std::begin(data_x), std::end(data_x)
      //    ,std::begin(data_y), std::end(data_y)
      //    );
      auto kb_w = util::weight_least_squart_line_fit(
          data_x.data()
          ,data_y.data()
          ,weights.data()
          ,data_x.size()
          );
      //std::cout
      //  <<kb->first<<" "<<kb->second
      //  <<kb_w->first<<" "<<kb_w->second
      //  <<"\n";
      if (!kb_w) return;
      for (int i=0; i<sp_data_x.size(); ++i){
        float dis = service::distance_p2l(
            sp_data_x.at(i),sp_data_y.at(i)
            ,kb_w->first,kb_w->second
            );
        hisx->Fill(dis);
        //info_out(dis);
      }
    }

    if (hisy){
      if (m_strip_y.size()<8) return;
      float ey = 0.; for (auto&& [_,x] : m_strip_y) ey += x.second;
      std::map<uint16_t,std::pair<double,double>> y_strips;
      for(auto iter = std::next(std::begin(m_strip_y),2);
          iter != std::prev(std::end(m_strip_y),2); ++iter)
        y_strips.emplace(*iter);
      std::vector<float> data_x;
      std::vector<float> data_y;
      std::vector<float> weights;
      std::vector<float> sp_data_x;
      std::vector<float> sp_data_y;
      auto sample_idx = sample(2,0,y_strips.size()-1);
      std::size_t index=0;
      for (int i=0; i<y_strips.size(); ++i){
        auto iter = std::next(std::begin(y_strips),i);
        if (sample_idx.find(i) != sample_idx.end()
            && iter->second.second/ey>=thr
            ){
          sp_data_x.emplace_back(iter->first*s_strips_width);
          sp_data_y.emplace_back(iter->second.first*s_dfift_velocity);
        }
        else if(iter->second.second/ey>=thr){
          data_x.emplace_back(iter->first*s_strips_width);
          data_y.emplace_back(iter->second.first*s_dfift_velocity);
          weights.emplace_back(iter->second.second/ey);
        }
      }
      //auto kb_w = util::least_squart_line_fit(
      //    std::begin(data_x), std::end(data_x)
      //    ,std::begin(data_y), std::end(data_y)
      //    );
      auto kb_w = util::weight_least_squart_line_fit(
          data_x.data()
          ,data_y.data()
          ,weights.data()
          ,data_x.size()
          );
      //std::cout
      //  <<kb->first<<" "<<kb->second
      //  <<" "<<kb_w->first<<" "<<kb_w->second
      //  <<"\n";

      if (!kb_w) return;
      for (int i=0; i<sp_data_x.size(); ++i){
        float dis = service::distance_p2l(
            sp_data_x.at(i),sp_data_y.at(i)
            ,kb_w->first,kb_w->second
            );
        hisy->Fill(dis);
        //info_out(dis);
      }
    }
  }

  typedef std::function<bool(entry_strip const&)> filter_t;
  filter_t m_filter;
  void set_filter(filter_t&& f){ m_filter = f;}
    
};

//void handle_arrive_time(detector_entry_t const&, ){
//}


}
using std::cout; using std::endl; using std::string; using std::vector;
int main(int argc, char* argv[]){
  std::string fin_name = argv[1];
  namespace fs = std::filesystem;
  if (!fs::exists(fin_name) && !fs::is_regular_file(fin_name)){
    std::cerr<<"Invalid input file, invoke 'exit(-1)'.\n"; exit(-1); }
  auto* fin = TFile::Open(fin_name.c_str());
  auto* tree = dynamic_cast<TTree*>(fin->Get("CollectionTree"));
  auto entries = tree->GetEntries();
  TClonesArray* arr = new TClonesArray("entry_strip");
  tree->GetBranch("hit_strips")->SetAutoDelete(false);
  tree->SetBranchAddress("hit_strips",&arr);
  int event_id;
  tree->SetBranchAddress("event_id",std::addressof(event_id));
  std::size_t count = 0;

  TFile *fout = new TFile("result.root","recreate");
  util::t_object_factory_v0 t_object_factory{};
  t_object_factory
    .register_element<TH1F>("spectrum_L1",1500,0,15000)
    .register_element<TH1F>("spectrum_L0",1000,0,50000)
    .register_element<TH1F>("spectrum_L0_s0",4000,0,40000)
    .register_element<TH1F>("kx",2000,-100,100)
    .register_element<TH1F>("ky",2000,-100,100)
    .register_element<TH1F>("Chisquare_NDF_x",1000,0,400)
    .register_element<TH1F>("Chisquare_NDF_y",1000,0,400)
    .register_element<TH1F>("Chisquare_NDF_y",1000,0,400)
    .register_element<TH1F>("residualx_L0",1000,-5,5)
    .register_element<TH1F>("residualy_L0",1000,-5,5)
    .register_element<TH1F>("residualz_L0",200,-100,100)

    .register_element<TH2I>("HitMap_L0",320,0,320,320,0,320)
    .register_element<TH1F>("Spectrum_L0_Center",2000,0,40000)
    .register_element<TH1F>("Spectrum_L0_Center_fit",2000,0,40000)
    .register_element<TH1F>("Spectrum_L0_Side",2000,0,40000)
    .register_element<TH1F>("Spectrum_L0_Total",2000,0,40000)

    .register_element<TH1F>("strips_relative_edep_x",100,0,1)
    .register_element<TH1F>("strips_relative_edep_y",100,0,1)

    .register_element<TH1F>("track_L0_kXZ",180,-90,90)
    .register_element<TH1F>("track_L0_kYZ",180,-90,90)
    .register_element<TH1F>("residual_L0_XZ_15_17",200,-5,5)
    .register_element<TH1F>("residual_L0_XZ_20_22",200,-5,5)
    .register_element<TH1F>("residual_L0_XZ_26_28",200,-5,5)
    .register_element<TH1F>("residual_L0_XZ_30_32",200,-5,5)
    .register_element<TH1F>("residual_L0_XZ_40_42",200,-5,5)
    .register_element<TH1F>("residual_L0_XZ_50_52",200,-5,5)
    .register_element<TH1F>("residual_L0_XZ_60_62",200,-5,5)

    .register_element<TH1F>("residual_L0_YZ_15_17",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_18_20",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_20_22",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_22_24",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_24_26",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_26_28",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_28_30",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_30_32",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_32_34",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_34_36",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_36_38",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_38_40",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_40_42",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_50_52",200,-5,5)
    .register_element<TH1F>("residual_L0_YZ_60_62",200,-5,5)
    ;


  std::vector<entry_strip> hits_vec;
  auto const& fill_hits = [&](){
    hits_vec.clear();
    int ht = arr->GetEntriesFast();
    hits_vec.resize(ht);
    for (int j=0; j<ht; ++j) hits_vec[j] = *static_cast<entry_strip*>(arr->At(j));
  };

  double kx, ky, bx, by, ndf_kx, ndf_ky;
  std::vector<service::point_t> points_x;
  std::vector<service::point_t> points_y;

  std::size_t count0=0, count1=0, count2=0;
  std::size_t track_count_center = 0;
  TFolder* track_fl = new TFolder("tracks","tracks");
  fout->cd();

  // temporary
  //entries = entries>100 ? 10 : entries;

  auto const& hit_filter = [](entry_strip const& hit)->bool{
    if (hit.is_fit==0) return false;
    if (hit.p0<=550 || hit.p0>750) return false;
    if (hit.p2<315. || hit.p2>=455.) return false;
    if (hit.p3>30) return false;
    return true; };
  for (decltype(entries) i=0; i<entries; ++i){
    if (i%10000==0) std::cout<<"progress: "<<i<<"/"<<entries<<std::endl;
    arr->Clear();
    tree->GetEntry(i);
    int hits = arr->GetEntriesFast();
    if (hits<10 || hits>100) continue;
    fill_hits();
    user::detector_hits_t entry{};
    entry.set_filter(hit_filter);
    entry.set_hits(i,hits_vec);
    auto hit_pos = entry.hit_position();
    if (hit_pos){
      uint16_t posx = hit_pos->first;
      uint16_t posy = hit_pos->second;
      (*t_object_factory.get<TH2I>("HitMap_L0"))->Fill(posx,posy);
      (*t_object_factory.get<TH1F>("Spectrum_L0_Total"))->Fill(entry.m_edep);
      if (posx>285 && posx<320 && posy>110 && posy<180){
        (*t_object_factory.get<TH1F>("Spectrum_L0_Side"))->Fill(entry.m_edep);
      }
      if (posx>130 && posx<180 && posy>140 && posy<195){
        (*t_object_factory.get<TH1F>("Spectrum_L0_Center"))->Fill(entry.m_edep);
        (*t_object_factory.get<TH1F>("Spectrum_L0_Center_fit"))->Fill(entry.m_edep_fit);
        //if (track_count_center<200 && i>0){
        //  std::stringstream sstr(""); sstr<<"Canvas_"<<i;
        //  TCanvas* canvas = new TCanvas(sstr.str().c_str(),sstr.str().c_str(),1500,900);
        //  canvas->Divide(2,2);
        //  entry.draw_xz_yz( dynamic_cast<TPad*>(canvas->cd(1)) ,dynamic_cast<TPad*>(canvas->cd(2)));
        //  track_fl->Add(canvas);
        //  track_count_center++; }
        float dzx = entry.get_drift_distance_xz();
        if (dzx>8){
          float axz = entry.get_angle_xz(0.02);
          (*t_object_factory.get<TH1F>("track_L0_kXZ"))->Fill(axz);

          if (std::abs(axz)>=15 && std::abs(axz)<17){
            entry.add_points(*t_object_factory.get<TH1F>("residual_L0_XZ_15_17") ,nullptr);
          }
          if (std::abs(axz)>=22 && std::abs(axz)<24){
            entry.add_points(*t_object_factory.get<TH1F>("residual_L0_XZ_20_22") ,nullptr);
          }

          if (std::abs(axz)>=26 && std::abs(axz)<28){
            if (track_count_center<400){
              std::stringstream sstr(""); sstr<<"Event_"<<i;
              TCanvas* canvas = new TCanvas(sstr.str().c_str(),sstr.str().c_str(),1500,900);
              canvas->Divide(2,2);
              entry.draw_xz_yz( dynamic_cast<TPad*>(canvas->cd(1)) ,dynamic_cast<TPad*>(canvas->cd(2)));
              track_fl->Add(canvas);
              track_count_center++; }

            entry.add_points( *t_object_factory.get<TH1F>("residual_L0_XZ_26_28") ,nullptr);
          }
          if (std::abs(axz)>=32 && std::abs(axz)<34){
            entry.add_points( *t_object_factory.get<TH1F>("residual_L0_XZ_30_32") ,nullptr);
          }
          if (std::abs(axz)>=42 && std::abs(axz)<44){
            entry.add_points( *t_object_factory.get<TH1F>("residual_L0_XZ_40_42") ,nullptr);
          }
          if (std::abs(axz)>=52 && std::abs(axz)<54){
            entry.add_points( *t_object_factory.get<TH1F>("residual_L0_XZ_50_52") ,nullptr);
          }
          if (std::abs(axz)>=62 && std::abs(axz)<64){
            entry.add_points( *t_object_factory.get<TH1F>("residual_L0_XZ_60_62") ,nullptr);
          }
        }
        float dzy = entry.get_drift_distance_yz();
        if (dzy>6.){
          float ayz = entry.get_angle_yz(0.03);
          (*t_object_factory.get<TH1F>("track_L0_kYZ"))->Fill(ayz);
          if (std::abs(ayz)>=15 && std::abs(ayz)<17){
            entry.add_points(nullptr,*t_object_factory.get<TH1F>("residual_L0_YZ_15_17"));
          }
          if (std::abs(ayz)>=18 && std::abs(ayz)<20){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_18_20"));
          }
          if (std::abs(ayz)>=20 && std::abs(ayz)<22){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_20_22"));
          }
          if (std::abs(ayz)>=22 && std::abs(ayz)<24){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_22_24"));
          }
          if (std::abs(ayz)>=24 && std::abs(ayz)<26){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_24_26"));
          }
          if (std::abs(ayz)>=26 && std::abs(ayz)<28){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_26_28"));
          }
          if (std::abs(ayz)>=28 && std::abs(ayz)<30){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_28_30"));
          }
          if (std::abs(ayz)>=30 && std::abs(ayz)<32){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_30_32"));
          }
          if (std::abs(ayz)>=40 && std::abs(ayz)<42){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_40_42"));
          }
          if (std::abs(ayz)>=50 && std::abs(ayz)<52){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_50_52"));
          }
          if (std::abs(ayz)>=60 && std::abs(ayz)<62){
            entry.add_points( nullptr ,*t_object_factory.get<TH1F>("residual_L0_YZ_60_62"));
          }
        } 
      }
    }
  }
  fout->cd();
  track_fl->Write();
  t_object_factory
    .write_to(fout,"HitMap_.*")
    .write_to(fout,"Spectrum.*")
    .write_to(fout,"strips_relative_edep_.*")
    .write_to(fout,"track_L0_k.*")
    .write_to(fout,"residual_L0_.*")
    //.write_to(fout,"spectrum.*")
    //.write_to(fout,"Chisquare_NDF.*")
    //.write_to(fout,"kx")
    //.write_to(fout,"ky")
    //.write_to(fout,"residualx_L0")
    //.write_to(fout,"residualy_L0")
    //.write_to(fout,"residualz_L0")

    ;
  fout->Write();
  fout->Close();
  fin->Close();
  return 0;
}

//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: wy187110@mail.ustc.edu.cn     
//^-^ Time: 2024-10-31 16:34:44   Posi: Hefei
//^-^ File: util.h
//--------------------------------------------------------------------
#ifndef util_H
#define util_H 1 
#include <string>
#include <cstdint>
#include <utility>
#include <map>
#include <optional>
#include <type_traits>
#include <random>
#ifndef info_out
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#endif
std::string version();

namespace util{
template <class _tp, class _up>
struct id_transform_base{
  typedef _tp id_to_t;
  typedef _up id_from_t;

  id_transform_base() = default;
  ~id_transform_base() noexcept = default;

  virtual _tp tran(_up const&) const = 0;
};

struct det_id_t{
  int16_t m_layer_id=-1;
  int16_t m_dimension_id=-1;
  int16_t m_channel_id=-1;

  det_id_t() = default;
  ~det_id_t() noexcept = default;
};

typedef std::pair<uint32_t,uint32_t> fec_id_t;

struct id_trans : public id_transform_base<det_id_t, fec_id_t>{
  typedef id_transform_base<det_id_t,fec_id_t> base_t;
  using base_t::id_from_t;
  using base_t::id_to_t;

  id_trans(std::string const&);
  ~id_trans() noexcept = default;
  
  virtual id_to_t tran(id_from_t const&) const override;
  det_id_t m_det_id;
  std::map<std::pair<uint16_t,uint16_t>,std::pair<uint32_t,uint32_t>> m_fec2det_map;

};
template <class _iterx, class _itery>
std::optional<std::pair<double,double>> least_squart_line_fit(
    _iterx first, _iterx last, _itery yfirst, _itery ylast){
  auto cd = std::min(std::distance(first,last),std::distance(yfirst,ylast));
  if(cd<=1) return std::nullopt;
  auto const& ave = []<class _tp>(_tp f, _tp l)->double{
    return std::accumulate(f,l,0.)/std::distance(f,l); };
  double ave_x = ave(first,last);
  double ave_y = ave(yfirst,ylast);
  double sum_xy=0., sum_xx=0.;
  for (size_t i=0; i<(size_t)cd; ++i) sum_xy += *first++**yfirst++;
  std::advance(first,-cd);
  for (size_t i=0; i<(size_t)cd; ++i) sum_xx += *first**first++;
  std::pair<double,double> rt;
  double sub = sum_xx-cd*ave_x*ave_x;
  if (std::abs(sub)<1.e-3) return std::nullopt;
  rt.first = (sum_xy-cd*ave_x*ave_y)/(sum_xx-cd*ave_x*ave_x);
  rt.second = ave_y-rt.first*ave_x;
  return std::make_optional(rt);
}




}

class TGraph;
class TObject;
class TH1;
class TH1F;
class TH1D;
class TH1I;
class TH1S;
class TH2C;
class TH2D;
class TH2F;
class TH2I;
class TH2S;
class TFile;
class TH1C;
class TF1;
class TF2;
#include <optional>
#include <regex>
#include <unordered_map>
namespace util{
std::pair<bool,double> fit_wave(TGraph* grp, double* params
    ,double rate, double& arrive_time);
 

//template <class _tp>
//std::pair<bool,double> fit_wave(_tp first, _tp last, double* params = nullptr){
//}


struct t_object_factory_v0{
  typedef t_object_factory_v0 self_t;
  t_object_factory_v0() = default;
  ~t_object_factory_v0() noexcept = default;

  std::unordered_map<std::string,TObject*> m_elements;

  template <class _tp, class... _args>
  self_t& register_element(std::string const& name, _args&&... params){
    if (m_elements.find(name)!=m_elements.end()) return *this;
    if constexpr
      (std::__is_one_of<_tp,TH1,TH1C,TH1D,TH1F,TH1I,TH1S,TH2C,TH2D,TH2F,TH2I,TH2S>::value){
      auto* ptr= new _tp(name.c_str(),name.c_str(),std::forward<_args>(params)...);
      ptr->SetDirectory(nullptr);
      m_elements.emplace(name,static_cast<TObject*>(ptr));
    }else if constexpr(std::__is_one_of<_tp,TF1,TF2>::value){
      auto* ptr= new _tp(name.c_str(),std::forward<_args>(params)...);
      m_elements.emplace(name,static_cast<TObject*>(ptr));
    }else{
      m_elements[name] = dynamic_cast<TObject*>(new _tp(std::forward<_args>(params)...));
    }
    return *this;
  }
  //TODO
  bool unregister_element(std::string const& name);

  template <class _tp>
  std::optional<_tp*> get(std::string const& name){
    if (m_elements.find(name) == m_elements.end()) return std::nullopt;
    return dynamic_cast<_tp*>(m_elements.at(name)); }

  self_t& write_to(TFile* fout, std::string const& names);
};
}
#endif

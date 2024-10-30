#include "util.hpp"
#include <chrono>
namespace{
auto const& format00 = [](std::size_t v)->std::string{
  return v<10 ? std::string("0")+std::to_string(v) : std::to_string(v); };
}

namespace util{

namespace math{
double gaus(double* x,double* p){
  return p[0]*std::exp(-(x[0]-p[1])*(x[0]-p[1])/2/p[2]/p[2]); }
}

std::unordered_map<std::string,std::string> read_argv(std::string const& pg){
  std::ifstream fin("argv.ini");
  if (!fin){
    throw std::invalid_argument("No InI file");
    exit(0);
  }
  std::string sbuf;
  std::unordered_map<std::string,std::string> ret;
  std::stringstream pgname(""); pgname<<"["<<pg<<"]";
  while(!fin.eof()){
    std::getline(fin,sbuf);
    if (sbuf==pgname.str()){
      //std::cout<<"WY\n";
      while(!fin.eof()){
        auto back = fin.tellg();
        std::getline(fin,sbuf);
        if (auto iter = sbuf.find_first_of('#'); iter != std::string::npos){
          bool tag=true;
          for (std::size_t i=0; i<iter; ++i) if (!std::isspace(sbuf[i])){ tag=false; break;}
          if (tag) continue; }
        if (sbuf.find_first_of('[') != std::string::npos) { fin.seekg(back); break;}
        if (!sbuf.empty() && sbuf.find_first_of('=') != std::string::npos){
          std::string key = sbuf.substr(0,sbuf.find_first_of('='));
          std::string value =  sbuf.substr(sbuf.find_first_of('=')+1);
          trim_space(key); trim_space(value);
          if (!key.empty() && !value.empty()) ret[key]=value;
        }
      }
    }
  }
  fin.close();
  return ret;
}
void generate_frame_trigger(char*& positon,uint16_t b, uint16_t f, uint16_t t){
  //head_write(positon,e_command_address::k_TThreshold);
  //head_write(positon+4,0x0001);
  t &= 0x0FFF;
  head_write(positon,e_command_address::k_FEID);
  positon[4] = 0x40 + (b&0x000F);
  positon[5] = 0x50 + ((b>>4)&0x000F);
  positon[6] = 0x60 + ((b>>8)&0x000F);
  positon[7] = 0x70 + ((b>>12)&0x000F);
  positon[8] = 0x83;
  head_write(positon+9,e_command_address::k_ChannelID);
  positon[13] = 0x40 + (f&0x000F);
  positon[14] = 0x50 + ((f>>4)&0x000F);
  positon[15] = 0x60 + ((f>>8)&0x000F);
  positon[16] = 0x70 + ((f>>12)&0x000F);
  positon[17] = 0x83;
  head_write(positon+18,e_command_address::k_Threshold_Value);
  positon[22] = 0x40 + (t&0x000F);
  positon[23] = 0x50 + ((t>>4)&0x000F);
  positon[24] = 0x60 + ((t>>8)&0x000F);
  positon[25] = 0x70 + ((t>>12)&0x000F);
  positon[26] = 0x83;
  head_write(positon+27,e_command_address::k_Config);
  head_write(positon+31,e_command_address::k_Confirm,0x40);
  positon[35] = 0x83;
  positon+=36;
}

std::size_t generate_frame_baseline(char* positon, std::vector<uint16_t> const& bl){
  char* begin = positon;
  assert(bl.size()>=3);
  for (size_t i=0; i<bl.size(); ++i){
    head_write(positon,(uint16_t)e_command_address::k_BaseLine+2*i);
    head_write(positon+4,bl[i],0x40);
    positon[8] = 0x83; positon += 9;
  }
  return std::distance(begin,positon);
}
std::size_t generate_frame_irrfilter(char* positon, std::vector<uint16_t> const& bl){
  char* begin = positon;
  assert(bl.size()>=8);
  for (size_t i=0; i<bl.size(); ++i){
    head_write(positon,(uint16_t)e_command_address::k_IIRFilter+2*i);
    head_write(positon+4,bl[i],0x40);
    positon[8] = 0x83; positon += 9;
  }
  return std::distance(begin,positon);
}
std::size_t generate_frame_trig_delay_cycle(char* positon, uint16_t v){
  head_write(positon,e_command_address::k_TrigDelayCycle);
  head_write(positon+4,v,0x40);
  positon[8] = 0x83;
  return 9;
}
std::size_t generate_frame_hitwidth_cycle(char* positon, uint8_t v){
  head_write(positon,e_command_address::k_HitWidthCycle);
  head_write(positon+4,v,0x40);
  positon[8] = 0x83;
  return 9;
}
std::size_t generate_frame_trig_rise_step(char* positon, uint8_t v){
  head_write(positon,e_command_address::k_TrigRiseStep);
  head_write(positon+4,v,0x40);
  positon[8] = 0x83;
  return 9;
}
std::size_t generate_frame_nhit_channel(char* positon, uint8_t v){
  head_write(positon,e_command_address::k_NHitChannel);
  head_write(positon+4,v,0x40);
  positon[8] = 0x83;
  return 9; }

std::string terminal_color::gen() const{
  return  
    std::string{"\033"}
    + std::string{"["}
    + std::to_string(std::underlying_type<display_mode>::type(mode))
    + std::string{";"}
    + std::to_string(std::underlying_type<f_color>::type(fc))
    + std::string{";"}
    + std::to_string(std::underlying_type<b_color>::type(bc))
    + std::string{"m"}
    ;
}
std::string time_to_str(){
  using namespace std::chrono;
  system_clock::time_point tp_now = system_clock::now();
  system_clock::duration dsp = tp_now.time_since_epoch();
  time_t msp = duration_cast<microseconds>(dsp).count();
  time_t sse = msp/1000000;
  std::tm ct = *std::localtime(&sse);
  std::stringstream sstr;
  sstr<<1900 + ct.tm_year << "-"<<1+ ct.tm_mon << "-" << ct.tm_mday
    <<"_"<<format00(ct.tm_hour)<<"-"<<format00(ct.tm_min)<<"-"<<format00(ct.tm_sec)<<"_"<< msp%1000000;
  return sstr.str(); }


}

#include <numeric>
namespace util{
void cluster_comp(cluster& v){
  v.strips = v.range.second-v.range.first;
  v.sum_amp = std::accumulate(v.amp.begin(),v.amp.end(),0); }
}

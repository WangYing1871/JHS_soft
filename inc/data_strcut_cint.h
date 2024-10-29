#ifndef data_strcut_cint_H
#define data_strcut_cint_H 1 
#include "TObject.h"
#include <array>
#include <list>
#include <vector>
#include <cstdint>
struct entry_new : public TObject{
  uint32_t event_id;
  std::vector<uint8_t> fec_ids;
  std::vector<uint8_t> hit_channel_nos;
  float time_stamp;
  std::vector<uint16_t> global_ids;
  std::vector<std::vector<uint16_t>> adcs;

public:
  ClassDef(entry_new,1)
};

struct cluster : public TObject{
  std::pair<int,int> range;
  std::vector<uint16_t> amp;
  std::vector<int> amp_time;
public:
  //cluster() = default;
  //~cluster() noexcept = default;
public:

  ClassDef(cluster,1)
};


#endif

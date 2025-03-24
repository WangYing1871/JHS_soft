#include "topographicseries.h"
float const packingFactor = 11983.f;

TopographicSeries::TopographicSeries(){
  setDrawMode(QSurface3DSeries::DrawSurface);
  setFlatShadingEnabled(true);
  setBaseColor(Qt::white); }

TopographicSeries::~TopographicSeries() = default;


void TopographicSeries::setTopographyFile(QString const file, float width, float height){
  QImage image(file);
  uchar* bits = image.bits();
  int img_height = image.height();
  int img_width = image.width();
  int width_bits = img_width*4;
  float stepx = width/float(img_width);
  float stepz = height/float(img_height);

  auto* data_array = new QSurfaceDataArray;
  data_array->reserve(img_height);
  for (int i=0; i<img_height; ++i){
    int p = i*width_bits;
    float z = height-float(i)*stepz;
    auto* new_row = new QSurfaceDataRow;
    new_row->reserve(img_width);
    for (int j=0; j<img_width; ++j){
      uchar aa = bits[p+0];
      uchar rr = bits[p+1];
      uchar gg = bits[p+2];
      uint color = uint((gg<<16)+(rr<<8)+aa);
      float y = color/packingFactor;
      new_row->append(QSurfaceDataItem({float(j)*stepx,y,z}));
      p+=4;
    }
    data_array->append(new_row);
  }
  dataProxy()->resetArray(data_array);
}

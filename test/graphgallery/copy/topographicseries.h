#ifndef topographicseries_H
#define topographicseries_H 1 
#include <QtDataVisualization/qsurface3dseries.h>
class TopographicSeries : public QSurface3DSeries{
  Q_OBJECT

public:
  TopographicSeries();
  ~TopographicSeries();
  void setTopographyFile(QString const, float, float);

  float sampleCountX() const {return m_sampleCountX;}
  float sampleCountZ() const {return m_sampleCountZ;}


private:
  float m_sampleCountX = 0.f;
  float m_sampleCountZ = 0.f;
};



#endif

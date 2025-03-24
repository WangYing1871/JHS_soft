#ifndef highlightseries_H
#define highlightseries_H 1 
#include <QtDataVisualization/qsurface3dseries.h>
#include "topographicseries.h"

class HighlightSeries : public QSurface3DSeries{
  Q_OBJECT

public:
  explicit HighlightSeries();
  ~HighlightSeries();

  void setTopographicSeries(TopographicSeries*);
  inline void setMinHeight(float height) {m_minHeight = height;}

public Q_SLOTS:
  void handlePositionChange(QPoint const&);
  void handleGradientChange(float);

private:
  int m_width = 100;
  int m_height = 100;
  int m_srcWidth = 0;
  int m_srcHeight = 0;
  QPoint m_position = {};
  TopographicSeries *m_topographicSeries = nullptr;
  float m_minHeight = 0.f;
};
#endif

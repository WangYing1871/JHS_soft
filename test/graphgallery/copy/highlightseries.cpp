#include "highlightseries.h"
const float darkRedPos = 1.f;
const float redPos = 0.8f;
const float yellowPos = 0.6f;
const float greenPos = 0.4f;
const float darkGreenPos = 0.2f;

HighlightSeries::HighlightSeries(){
  setDrawMode(QSurface3DSeries::DrawSurface);
  setFlatShadingEnabled(true);
  setVisible(false); }

HighlightSeries::~HighlightSeries(){ }

void HighlightSeries::setTopographicSeries(TopographicSeries* series){
  m_topographicSeries = series;
  m_srcWidth = m_topographicSeries->dataProxy()->array()->at(0)->size();
  m_srcHeight = m_topographicSeries->dataProxy()->array()->size();
  QObject::connect(m_topographicSeries
      ,&QSurface3DSeries::selectedPointChanged
      ,this
      ,&HighlightSeries::handlePositionChange
      );
}

void HighlightSeries::handlePositionChange(QPoint const& position){
  m_position = position;
  if (position==invalidSelectionPosition()){setVisible(false); return;}
  int hw = m_width/2;
  int hh = m_height/2;
  int startx = position.y()-hw<0 ? 0 : position.y()-hw;
  int endx = position.y()+hw>m_srcWidth-1 ? m_srcWidth-1 : position.y()+hw;
  int startz = position.x()-hh < 0 ? 0 : position.x()-hh;
  int endz = position.x()+hh > (m_srcHeight-1) ? m_srcHeight-1 : position.x()+hh;
  auto* src_proxy = m_topographicSeries->dataProxy();
  QSurfaceDataArray const& src_array = *src_proxy->array();
  auto* data_array = new QSurfaceDataArray;
  data_array->reserve(endz-startz);
  for (int i=startz; i<endz; ++i){
    auto* new_row = new QSurfaceDataRow;
    new_row->reserve(endx-startx);
    auto* src_row = src_array.at(i);
    for (int j=startx; j<endx; ++j){
      auto pos = src_row->at(j).position();
      pos.setY(pos.y()+0.1f);
      new_row->append(QSurfaceDataItem(pos));
    }
    data_array->append(new_row);
  }
  dataProxy()->resetArray(data_array);
  setVisible(true);
}

void HighlightSeries::handleGradientChange(float value){
  float ratio = m_minHeight/value;
  QLinearGradient gr;
  gr.setColorAt(0.f,Qt::black);
  gr.setColorAt(darkRedPos*ratio,Qt::darkGreen);
  gr.setColorAt(greenPos * ratio, Qt::green);
  gr.setColorAt(yellowPos * ratio, Qt::yellow);
  gr.setColorAt(redPos * ratio, Qt::red);
  gr.setColorAt(darkRedPos * ratio, Qt::darkRed);
  setBaseGradient(gr);
  setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

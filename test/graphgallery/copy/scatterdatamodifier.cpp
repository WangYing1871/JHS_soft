#include <QtDataVisualization/qscatterdataproxy.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qscatter3dseries.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtCore/qmath.h>
#include <QtCore/qrandom.h>
#include <QtWidgets/qcombobox.h>
#include "scatterdatamodifier.h"
#include "axesinputhandler.h"

using namespace Qt::StringLiterals;

const int numberOfItems = 10000;
const float curveDivider = 7.5f;
const int lowerNumberOfItems = 900;
const float lowerCurveDivider = 0.75f;

ScatterDataModifier::ScatterDataModifier(Q3DScatter*
    scatter, QObject* parent):
  QObject(parent)
  ,m_graph(scatter)
  ,m_itemCount(lowerNumberOfItems)
  ,m_curveDivider(lowerCurveDivider)
  ,m_inputHandler(new AxesInputHandler(scatter)){
  m_graph->activeTheme()->setType(Q3DTheme::ThemeStoneMoss);
  m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftHigh);
  m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);

  auto* proxy = new QScatterDataProxy;
  auto* series = new QScatter3DSeries(proxy);

  series->setItemLabelFormat(u"@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"_s);
  series->setMeshSmooth(m_smooth);
  m_graph->addSeries(series);
  m_graph->setActiveInputHandler(m_inputHandler);
  m_inputHandler->setAxes(m_graph->axisX(),m_graph->axisZ(),m_graph->axisY());

  addData();
}

void ScatterDataModifier::addData(){
  m_graph->axisX()->setTitle("X");
  m_graph->axisY()->setTitle("Y");
  m_graph->axisZ()->setTitle("Z");

  auto* data_array = new QScatterDataArray;
  data_array->reserve(m_itemCount);
#ifdef RANDOM_SCATTER
  for (int i=0; i<m_itemCount; ++i)
    data_array->append(QScatterDataItem(randVector()));
#else
  const float limit = qSqrt(m_itemCount)/2.f;
  for (int i= -limit; i<limit; ++i){
    for (int j=-limit; j<limit; ++j){
      float const x = float(i)+0.5f;
      float const y = std::cos(qDegreesToRadians(float(i*j)/m_curveDivider));
      float const z = float(j)+0.5f;
      data_array->append(QScatterDataItem({x,y,z}));
    }
  }
#endif
  m_graph->seriesList().at(0)->dataProxy()->resetArray(data_array);
}

void ScatterDataModifier::changeStyle(int style){
  auto* cbb = qobject_cast<QComboBox*>(sender());
  if (cbb){
    m_style = cbb->itemData(style).value<QAbstract3DSeries::Mesh>();
    if (!m_graph->seriesList().isEmpty())
      m_graph->seriesList().at(0)->setMesh(m_style);
  }
}

void ScatterDataModifier::setSmoothDots(int smooth){
  m_smooth = bool(smooth);
  auto* series = m_graph->seriesList().at(0);
  series->setMeshSmooth(m_smooth);
}

void ScatterDataModifier::changeTheme(int theme){
  auto* current_theme = m_graph->activeTheme();
  current_theme->setType(Q3DTheme::Theme(theme));
  emit backgroundEnabledChanged(current_theme->isBackgroundEnabled());
  emit gridEnabledChanged(current_theme->isGridEnabled());
}

void ScatterDataModifier::changePresetCamera(){
  static int preset = Q3DCamera::CameraPresetFrontLow;
  m_graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);
  if (++preset > Q3DCamera::CameraPresetDirectlyBelow)
    preset = Q3DCamera::CameraPresetFrontLow;
}

void ScatterDataModifier::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality sq){
  int quality = int(sq); emit shadowQualityChanged(quality); }

void ScatterDataModifier::changeShadowQuality(int quality){
  QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
  m_graph->setShadowQuality(sq); }

void ScatterDataModifier::setGridEnabled(int enabled){
  m_graph->activeTheme()->setGridEnabled(enabled == Qt::Checked); }

void ScatterDataModifier::toggleItemCount(){
  if (m_itemCount==numberOfItems){
    m_itemCount = lowerNumberOfItems; m_curveDivider = lowerCurveDivider; }
  else{ m_itemCount = numberOfItems; m_curveDivider = curveDivider; }
  m_graph->seriesList().at(0)->dataProxy()->resetArray(0);
  addData(); }

void ScatterDataModifier::toggleRanges(){
  if (!m_autoAdjust){
    m_graph->axisX()->setAutoAdjustRange(true);
    m_graph->axisZ()->setAutoAdjustRange(true);
    m_inputHandler->setDragSpeedModifier(1.5f);
    m_autoAdjust = true; }
  else{
    m_graph->axisX()->setRange(-10.0f, 10.0f);
    m_graph->axisZ()->setRange(-10.0f, 10.0f);
    m_inputHandler->setDragSpeedModifier(15.0f);
    m_autoAdjust = false; }
}

QVector3D ScatterDataModifier::randVector(){
  auto* generator = QRandomGenerator::global();
  const auto x = float(generator->bounded(100)) / 2.0f - float(generator->bounded(100)) / 2.0f;
  const auto y = float(generator->bounded(100)) / 100.0f - float(generator->bounded(100)) / 100.0f;
  const auto z = float(generator->bounded(100)) / 2.0f - float(generator->bounded(100)) / 2.0f;
  return {x, y, z}; }

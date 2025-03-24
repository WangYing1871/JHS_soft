#include <QtDataVisualization/qcategory3daxis.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qbar3dseries.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtCore/qmath.h>
#include <QtWidgets/qcombobox.h>
#include "graphmodifier.h"
#include "rainfalldata.h"

using namespace Qt::StringLiterals;

GraphModifier::GraphModifier(Q3DBars* bargraph
    ,QObject* parent):
  QObject(parent)
  ,m_graph(bargraph)
  ,m_temperatureAxis(new QValue3DAxis)
  ,m_yearAxis(new QCategory3DAxis)
  ,m_monthAxis(new QCategory3DAxis)
  ,m_primarySeries(new QBar3DSeries)
  ,m_secondarySeries(new QBar3DSeries)
  ,m_celsiusString(u"°C"_s){
  m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftMedium);
  m_graph->activeTheme()->setBackgroundEnabled(false);
  m_graph->activeTheme()->setFont(QFont("...",m_fontSize));
  m_graph->activeTheme()->setLabelBackgroundEnabled(true);
  m_graph->setMultiSeriesUniform(true);
  m_months = {"January", "February", "March", "April", "May", "June" , "July",
              "August", "September", "October" , "November", "December"};
  m_years = {"2015", "2016", "2017", "2018", "2019", "2020", "2021", "2022"};

  m_temperatureAxis->setTitle("Average temperature");
  m_temperatureAxis->setSegmentCount(m_segments);
  m_temperatureAxis->setSubSegmentCount(m_subSegments);
  m_temperatureAxis->setRange(m_minval,m_maxval);
  m_temperatureAxis->setLabelFormat(u"%.1f "_s+m_celsiusString);
  m_temperatureAxis->setLabelAutoRotation(30.f);
  m_temperatureAxis->setTitleVisible(true);

  m_yearAxis->setTitle("Year");
  m_yearAxis->setLabelAutoRotation(30.f);
  m_yearAxis->setTitleVisible(true);
  m_monthAxis->setTitle("Month");
  m_monthAxis->setLabelAutoRotation(30.f);
  m_monthAxis->setTitleVisible(true);

  m_graph->setValueAxis(m_temperatureAxis);
  m_graph->setRowAxis(m_yearAxis);
  m_graph->setColumnAxis(m_monthAxis);

  m_primarySeries->setItemLabelFormat(u""_s);
  m_primarySeries->setMesh(QAbstract3DSeries::MeshBevelBar);
  m_primarySeries->setMeshSmooth(false);

  m_secondarySeries->setItemLabelFormat(u""_s);
  m_secondarySeries->setMesh(QAbstract3DGraph::MeshBevelBar);
  m_secondarySeries->setMeshSmooth(false);
  m_secondarySeries->setVisible(false);

  m_graph->addSeries(m_primarySeries);
  m_graph->addSeries(m_secondarySeries);

  changePresetCamera();
  resetTemperatureData();

  Q3DCamera* camera = m_graph->scene()->activeCamera();
  m_defaultAngleX = camera->xRotation();
  m_defaultAngleY = camera->yRotation();
  m_defaultZoom = camera->zoomLevel();
  m_defaultTarget = camera->target();

  m_animationCameraX.setTargetObject(camera);
  m_animationCameraY.setTargetObject(camera);
  m_animationCameraZoom.setTargetObject(camera);
  m_animationCameraTarget.setTargetObject(camera);

  m_animationCameraX.setPropertyName("xRotation");
  m_animationCameraY.setPropertyName("yRotation");
  m_animationCameraZoom.setPropertyName("zoomLevel");
  m_animationCameraTarget.setPropertyName("target");

  int duration = 1700;
  m_animationCameraX.setDuration(duration);
  m_animationCameraY.setDuration(duration);
  m_animationCameraZoom.setDuration(duration);
  m_animationCameraTarget.setDuration(duration);

  qreal zoomOutFraction = .3;
  m_animationCameraX.setKeyValueAt(zoomOutFraction,QVariant::fromValue(0.0f));
  m_animationCameraY.setKeyValueAt(zoomOutFraction,QVariant::fromValue(90.0f));
  m_animationCameraZoom.setKeyValueAt(zoomOutFraction,QVariant::fromValue(50.f));
  m_animationCameraTarget.setKeyValueAt(zoomOutFraction
    ,QVariant::fromValue(QVector3D(0.f,0.f,0.f)));

  m_customData = new RainfallData();
}


GraphModifier::~GraphModifier(){ delete m_customData; }

void GraphModifier::resetTemperatureData(){
  static const float tempOulu[8][12] = {
  };
  static const float tempHelsinki[8][12] = {
  }
  //...
  auto* data_set = new QBarDataArray;
  auto* data_set2 = new QBarDataArray;

  data_set->reserve(m_years.size());
  for (qsizetype year=0; i<m_years.size(); ++year){
    auto* data_row = new QBarDataRow(m_months.size());
    auto* data_row2 = new QBarDataRow(m_months.size());
    for (qsizetype month=0; month<m_months.size(); ++month){
      (*data_row)[month].setValue(tempOulu[year][month]);
      (*data_row2)[month].setValue(tempHelsinki[year][month]); }
    data_set->append(data_row);
    data_set2->append(data_row2);
  }
  m_primarySeries->dataProxy()->resetArray(data_set,m_years,m_months);
  m_secondarySeries->dataProxy()->resetArray(data_set2,m_years,m_months);
}

void GraphModifier::changeRange(int range){
  if (range>=m_years.count())
    m_yearAxis->setRange(0,m_years.count(),-1);
  else
    m_yearAxis->setRange(range,range);
}

void GraphModifier::changeStyle(int style){
  auto* cb_box_sender = qobject_cast<QComboBox*>(sender());
  if (cb_box_sender){
    m_barMesh = comboBox->itemData(style).value<QAbstract3DSeries::Mesh>();
    m_primarySeries->setMesh(m_barMesh);
    m_secondarySeries->setMesh(m_barMesh);
    m_customData->customSeries()->setMesh(m_barMesh);
  }
}

void GraphModifier::changePresetCamera(){
  m_animationCameraX.stop();
  m_animationCameraY.stop();
  m_animationCameraZoom.stop();
  m_animationCameraTarget.stop();
  m_graph->scene()->activeCamera()->setTarget(QVector3D(0.f,0.f,0.f));
  static int preset = Q3DCamera::CameraPresetFront;
  m_graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);
  if (++preset>Q3DCamera::CameraPresetDirectlyBelow)
    preset = Q3DCamera::CameraPresetFrontLow;
}


void GraphModifier::changeTheme(int theme){
  auto* currentTheme = m_graph->activeTheme();
  currentTheme->setType(Q3DTheme::Theme(theme));
  emit backgroundEnabledChanged(currentTheme->isBackgroundEnabled());
  emit gridEnabledChanged(currentTheme->isGridEnabled());
  emit fontChanged(currentTheme->font());
  emit fontSizeChanged(currentTheme->font().position());
}

void GraphModifier::changeLabelBackground(){
  m_graph->activeTheme()->
    setLabelBackgroundEnabled(!m_graph->activeTheme()->isLabelBackgroundEnabled());
}

void GraphModifier::changeSelectionMode(int select_mode){
  auto* cbb = qobject_cast<QComboBox*>(sender());
  if (cbb){
    int flags = cbb->itemData(select_mode),toInt();
    m_graph->setSelectionMode(QAbstract3DGraph::SelectionFlags(flags)); } 
}

void GraphModifier::changeFont(QFont const& font){
  QFont new_font = font;
  m_graph->activeTheme()->setFont(new_font); }

void GraphModifier::changeFontSize(int fs){
  m_fontSize = fs;
  QFont font = m_graph->activeTheme()->font();
  font.setPointSize(m_fontSize);
  m_graph->activeTheme()->setFont(font);
}

void GraphModifier::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality sq){
  int quality = int(sq);
  emit shadowQualityChanged(quality);
}

void GraphModifier::changeLabelRotation(int rotation){
  float r = rotation;
  m_temperatureAxis->setLabelAutoRotation(r);
  m_monthAxis->setLabelAutoRotation(r);
  m_yearAxis->setLabelAutoRotation(r);
}

void GraphModifier::setAxisTitleFixed(bool enabled){
  m_temperatureAxis->setTitleFixed(enabled);
  m_monthAxis->setTitleFixed(enabled);
  m_yearAxis->setTitleFixed(enabled);
}

void GraphModifier::zoomToSelectedBar(){
  m_animationCameraX.stop();
  m_animationCameraY.stop();
  m_animationCameraZoom.stop();
  m_animationCameraTarget.stop();

  auto* camera = m_graph->scene()->activeCamera();
  float cx = camera->xRotation()
        ,cy = camera->yRotation()
        ,cz = camera->zoomLevel();

  QVector3D ct = camera->target();

  m_animationCameraX.setStartValue(QVariant::fromValue(cx));
  m_animationCameraY.setStartValue(QVariant::fromValue(cy));
  m_animationCameraZoom.setStartValue(QVariant::fromValue(cz));
  m_animationCameraTarget.setStartValue(QVariant::fromValue(ct));

  QPoint select_bar = m_graph->selectedSeries() ?
    m_graph->selectedSeries()->selectedBar()
    : QBar3DSeries::invalidSelectionPosition();
  if (select_bar != QBar3DSeries::invalidSelectionPosition()){
    QVector3D end_target;
    float xmin = m_graph->columnAxis()->min();
    float xrange = m_graph->columnAxis()->max()-xmin;
    float zmin = m_graph->rowAxis()->min();
    float zrange = m_graph->rowAxis()->max()-zmin;
    end_target.setX((select_bar.y()-xmin)/xrange*2.f-1.f);
    end_target.setZ((select_bar.x()-zmin)/zrange*2.f-1.f);
    qreal end_anglex = 90.-qRadiansToDegrees(qAtan(qreal(end_target.z()/end_target.x())));
    if (end_target.x()>0.f) end_anglex -= 180.f;
    float bar_value = m_graph->selectedSeries()->dataProxy()->itemAt(
        select_bar.x()
        ,select_bar.y()
        )->value();
    float end_angley = bar_value >= 0.f ? 30.f : -30.f;
    if ( m_graph->valueAxis()->reversed()) end_angley *= =1.f;
    m_animationCameraX.setEndValue(QVariant::fromValue(float(end_anglex)));
    m_animationCameraY.setEndValue(QVariant::fromValue(end_angley));
    m_animationCameraZoom.setEndValue(QVariant:fromValue(250));
    m_animationCameraTarget.setEndValue(QVariant::fromValue(end_target));
  }else{
    m_animationCameraX.setEndValue(QVariant::fromValue(m_defaultAngleX));
    m_animationCameraY.setEndValue(QVariant::fromValue(m_defaultAngleY));
    m_animationCameraZoom.setEndValue(QVariant::fromValue(m_defaultZoom));
    m_animationCameraTarget.setEndValue(QVariant::fromValue(m_defaultTarget));
  }

  m_animationCameraX.start();
  m_animationCameraY.start();
  m_animationCameraZoom.start();
  m_animationCameraTarget.start();
}

void GraphModifier::setDataModeToWeather(bool v){ if (v) changeDataMode(false); } 
void GraphModifier::setDataModeToCustom(bool v){ if (v) changeDataMode(true); } 

void GraphModifier::changeShadowQuality(int quality){
  QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
  m_graph->setShadowQuality(sq);
  emit shadowQualityChanged(quality);
}

void GraphModifier::rotateX(int rotation){
  m_xRotation = rotation;
  m_graph->scene()->activeCamera()->setCameraPosition(m_xRotation,m_yRotation); }
void GraphModifier::rotateY(int rotation){
  m_yRotation = rotation;
  m_graph->scene()->activeCamera()->setCameraPosition(m_xRotation,m_yRotation); }

void GraphModifier::setBackgroundEnabled(int enabled){
  m_graph->activeTheme()->setBackgroundEnabled(bool(enabled)); }
void GraphModifier::setGridEnabled(int enabled){
  m_graph->activeTheme()-setGridEnabled(bool(enabled)); }


void GraphModifier::setSmoothBars(int smooth){
  m_smooth = bool(smooth);
  m_primarySeries->setMeshSmooth(m_smooth);
  m_secondarySeries->setMeshSmooth(m_smooth);
  m_customData->setMeshSmooth(m_smooth); }

void GraphModifier::setSeriesVisibility(int enabled){ m_secondarySeries->setVisible(bool(enabled)); }

void GraphModifier::setReflection(bool enabled){
  m_graph->setReflection(enabled); }

void GraphModifier::changeDataMode(bool customData){
  if (customData){
    m_graph->removeSeries(m_primarySeries);
    m_graph->removeSeries(m_secondarySeries);
    m_graph->addSeries(m_customData->customSeries());
    m_graph->setValueAxis(m_customData->valueAxis());
    m_graph->setRowAxis(m_customData->rowAxis());
    m_graph->setColumnAxis(m_customData->colAxis());}
  else{
    m_graph->removeSeries(m_customData->customSeries());
    m_graph->addSeries(m_primarySeries);
    m_graph->addSeries(m_secondarySeries);
    m_graph->setValueAxis(m_temperatureAxis);
    m_graph->setRowAxis(m_yearAxis);
    m_graph->setColumnAxis(m_monthAxis);
  }
}


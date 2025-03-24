#ifndef graphmodifier_H
#define graphmodifier_H 1 
#include <QtDataVisualization/q3dbars.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/qabstract3dseries.h>
#include <QtCore/qpropertyanimation.h>

class RainfallData;

class GraphModifier : public QObject{
  Q_OBJECT
 
public:
  explicit GraphModifier(Q3DBars* bargraph, QObject*);
  ~GraphModifier();

  void resetTemperatureData();
  void changePresetCamera();
  void changeLabelBackground();
  void changeFont(QFont const&);
  void changeFontSize(int);
  void rotateX(int);
  void rotateY(int);
  void setBackgroundEnabled(int);
  void setGridEnabled(int);
  void setSeriesVisibility(int);
  void setReverseValueAxis(int);
  void setReflection(bool);
  void changeDataMode(bool);

public Q_SLOTS:
  void changeRange(int range);
  void changeStyle(int);
  void changeSelectionMode(int);
  void changeTheme(int);
  void changeShadowQuality(int);
  void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality);
  void changeLabelRotation(int);
  void setAxisTitleVisibility(bool);
  void setAxisTitleFixed(bool);
  void zoomToSelectedBar();
  void setDataModeToWeather(bool);
  void setDataModeToCustom(bool);

Q_SIGNALS:
  void shadowQualityChanged(int);
  void backgroundEnabledChanged(bool);
  void gridEnabledChanged(bool);
  void fontChanged(QFont const&);
  void fontSizeChanged(int);

private:
  Q3DBars *m_graph = nullptr;
  float m_xRotation = 0.f;
  float m_yRotation = 0.f;
  int m_fontSize = 30;
  int m_segments = 4;
  int m_subSegments = 3;
  float m_minval = -20.f;
  float m_maxval = 20.f;
  QStringList m_months = {};
  QStringList m_years = {};
  QValue3DAxis *m_temperatureAxis = nullptr;
  QCategory3DAxis *m_yearAxis = nullptr;
  QCategory3DAxis *m_monthAxis = nullptr;
  QBar3DSeries *m_primarySeries = nullptr;
  QBar3DSeries *m_secondarySeries = nullptr;
  QAbstract3DSeries::Mesh m_barMesh = QAbstract3DSeries::MeshBevelBar;
  bool m_smooth = false;
  QPropertyAnimation m_animationCameraX = {};
  QPropertyAnimation m_animationCameraY = {};
  QPropertyAnimation m_animationCameraZoom = {};
  QPropertyAnimation m_animationCameraTarget = {};
  float m_defaultAngleX = 0.f;
  float m_defaultAngleY = 0.f;
  float m_defaultZoom = 0.f;
  QVector3D m_defaultTarget = {};
  const QString m_celsiusString;
  RainfallData *m_customData = nullptr;
};
#endif

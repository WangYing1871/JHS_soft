#ifndef custominputhandler_H
#define custominputhandler_H 1 
#include <QtDataVisualization/q3dinputhandler.h>
#include <QtDataVisualization/qabstract3dgraph.h>
#include <QtDataVisualization/qvalue3daxis.h>
class HighlightSeries;

class CustomInputHandler : public Q3DInputHandler{
  Q_OBJECT

  enum InputState{
    StateNormal = 0
    ,StateDraggingX
    ,StateDraggingZ
    ,StateDraggingY
  };

public:
  explicit CustomInputHandler(QAbstract3DGraph* graph, QObject* parent=0);
  inline void setLimits(float min, float max, float min_range){
    m_areaMinValue = min;
    m_areaMaxValue = max;
    m_axisXMinValue = m_areaMinValue;
    m_axisXMaxValue = m_areaMaxValue;
    m_axisZMinValue = m_areaMinValue;
    m_axisZMaxValue = m_areaMaxValue;
    m_axisXMinRange = min_range;
    m_axisZMinRange = min_range; }

  inline void setAxes(QValue3DAxis* a
      ,QValue3DAxis* b ,QValue3DAxis* c){ m_axisX = a; m_axisY = b; m_axisZ = c; }
  inline void setAspectRatio(float ratio) {m_aspectRatio = ratio;}
  inline void setHighlightSeries(HighlightSeries* series){ m_highlight = series; }
  inline void setDragSpeedModifier(float modifier) {m_speedModifier = modifier;}

protected:
  virtual void mousePressEvent(QMouseEvent*,QPoint const&);
  virtual void mouseMoveEvent(QMouseEvent*,QPoint const&);
  virtual void mouseReleaseEvent(QMouseEvent*,QPoint const&);
  virtual void wheelEvent(QWheelEvent*);

private:
  void handleElementSelected(QAbstract3DGraph::ElementType);
  void handleAxisDragging();
  void checkConstraints();
  
private:
  HighlightSeries *m_highlight = nullptr;
  bool m_mousePressed = false;
  InputState m_state = StateNormal;
  QValue3DAxis *m_axisX = nullptr;
  QValue3DAxis *m_axisY = nullptr;
  QValue3DAxis *m_axisZ = nullptr;
  float m_speedModifier = 20.f;
  float m_aspectRatio = 0.f;
  float m_axisXMinValue = 0.f;
  float m_axisXMaxValue = 0.f;
  float m_axisXMinRange = 0.f;
  float m_axisZMinValue = 0.f;
  float m_axisZMaxValue = 0.f;
  float m_axisZMinRange = 0.f;
  float m_areaMinValue = 0.f;
  float m_areaMaxValue = 0.f;
};

#endif

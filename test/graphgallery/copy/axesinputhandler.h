#ifndef axesinputhandler_H
#define axesinputhandler_H 1 
#include <QtDataVisualization/q3dinputhandler.h>
#include <QtDataVisualization/qabstract3dgraph.h>
#include <QtDataVisualization/qvalue3daxis.h>

class AxesInputHandler : public Q3DInputHandler{
  Q_OBJECT

  enum InputState{
    StateNormal = 0
    ,StateDraggingX
    ,StateDraggingZ
    ,StateDraggingY
  };

public:
  explicit AxesInputHandler(QAbstract3DGraph* graph, QObject* parent=0);

  inline void setAxes(QValue3DAxis* axisX
      ,QValue3DAxis* axisZ
      ,QValue3DAxis* axisY){ m_axisX = axisX; m_axisZ = axisZ; m_axisY = axisY; }

  inline void setDragSpeedModifier(float modifier) {m_speedModifier = modifier;}

  virtual void mousePressEvent(QMouseEvent* event, QPoint const&);
  virtual void mouseMoveEvent(QMouseEvent* event, QPoint const&);
  virtual void mouseReleaseEvent(QMouseEvent* event, QPoint const&);

private:
  void handleElementSelected(QAbstract3DGraph::ElementType type);
  void handleAxisDragging();

private:
  bool m_mousePressed = false;
  InputState m_state = StateNormal;
  QValue3DAxis* m_axisX = nullptr
                ,* m_axisZ = nullptr
                ,* m_axisY = nullptr
                ;
  float m_speedModifier = 15.f;
};

#endif 

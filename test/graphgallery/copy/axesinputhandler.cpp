#include <cmath>
#include <QtCore/qmath.h>
#include "axesinputhandler.h"

AxesInputHandler::AxesInputHandler(QAbstract3DGraph* graph
  ,QObject* parent){
  QObject::connect(graph,&QAbstract3DGraph::selectedElementChanged
      ,this,&AxesInputHandler::handleElementSelected); }

void AxesInputHandler::mousePressEvent(QMouseEvent* event
    ,QPoint const& mp){
  Q3DInputHandler::mousePressEvent(event,mp);
  if (Qt::LeftButton == event->button())
    m_mousePressed = true;
}

void AxesInputHandler::mouseMoveEvent(QMouseEvent* event, QPoint const& mp){
  if (m_mousePressed && m_state != StateNormal){
    setPreviousInputPos(inputPosition());
    setInputPosition(mp);
    handleAxisDragging();
  }else
    Q3DInputHandler::mousePressEvent(event,mp);
}

void AxesInputHandler::mouseReleaseEvent(QMouseEvent* event, QPoint const& mpos){
  Q3DInputHandler::mousePressEvent(event,mpos);
  m_mousePressed = false;
  m_state = StateNormal;
}

void AxesInputHandler::handleElementSelected(
    QAbstract3DGraph::ElementType type){
  switch(type){
    case QAbstract3DGraph::ElementAxisXLabel: m_state = StateDraggingX; break;
    case QAbstract3DGraph::ElementAxisYLabel: m_state = StateDraggingY; break;
    case QAbstract3DGraph::ElementAxisZLabel: m_state = StateDraggingZ; break;
    default: m_state = StateNormal; break;
  }
}

void AxesInputHandler::handleAxisDragging(){
  float distance = 0.f;
  float xr = scene()->activeCamera()->xRotation();
  float yr = scene()->activeCamera()->yRotation();

  float x_mutlix = std::cos(qDegreesToRadians(xr));
  float x_mutliy = std::sin(qDegreesToRadians(xr));
  float z_mutlix = std::sin(qDegreesToRadians(xr));
  float z_mutliy = std::cos(qDegreesToRadians(xr));

  QPoint move = inputPosition()-previousInputPos();
  float y_move = yr<0 ? -move.y() : move.y();
  switch(m_state){
    case StateDraggingX:
      distance = (move.x()*x_mutlix-y_move*x_mutliy)/m_speedModifier;
      m_axisX->setRange(m_axisX->min()-distance,m_axisX->max()-distance);
      break;
    case StateDraggingZ:
      distance = (move.x()*z_mutlix+y_move*z_mutliy)/m_speedModifier;
      m_axisZ->setRange(m_axisZ->min()-distance,m_axisZ->max()+distance);
      break;
    case StateDraggingY:
      distance = move.y()/m_speedModifier;
      m_axisY->setRange(m_axisY->min()+distance,m_axisY->max()+distance);
      break;
    default: break;
  }
}

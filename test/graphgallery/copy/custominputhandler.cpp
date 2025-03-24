#include <cmath>
#include <QtDataVisualization/q3dcamera.h>
#include <QtCore/qmath.h>

#include "custominputhandler.h"
#include "highlightseries.h"

CustomInputHandler::CustomInputHandler(QAbstract3DGraph* graph
    ,QObject* parent):
  Q3DInputHandler(parent){
  QObject::connect(graph,&QAbstract3DGraph::selectedElementChanged
      ,this,&CustomInputHandler::handleElementSelected);
}

void CustomInputHandler::mousePressEvent(QMouseEvent* event
    ,QPoint const& mouse_pos){
  if (Qt::LeftButton == event->button()){
    m_highlight->setVisible(false);
    m_mousePressed = true; }
  Q3DInputHandler::mousePressEvent(event,mouse_pos); }

void CustomInputHandler::wheelEvent(QWheelEvent* event){
  float delta = float(event->angleDelta().y());
  m_axisXMinValue += delta;
  m_axisXMaxValue -= delta;
  m_axisZMinValue += delta;
  m_axisZMaxValue -= delta;

  checkConstraints();
  float y = (m_axisXMaxValue-m_axisXMinValue)*m_aspectRatio;
  m_axisX->setRange(m_axisXMinValue,m_axisXMaxValue);
  m_axisY->setRange(100.f,y);
  m_axisZ->setRange(m_axisZMinValue,m_axisZMaxValue);
}

void CustomInputHandler::mouseMoveEvent(QMouseEvent* event, QPoint const& mpos){
  if (m_mousePressed && m_state != StateNormal){
    setPreviousInputPos(inputPosition());
    setInputPosition(mpos);
    handleAxisDragging(); }
  else Q3DInputHandler::mouseMoveEvent(event,mpos);
}


void CustomInputHandler::mouseReleaseEvent(QMouseEvent* event,QPoint const& mpos){
  Q3DInputHandler::mousePressEvent(event,mpos); m_mousePressed = false; m_state = StateNormal; }

void CustomInputHandler::handleElementSelected(QAbstract3DGraph::ElementType type){
  switch(type){
    case QAbstract3DGraph::ElementAxisXLabel: m_state = StateDraggingX; break;
    case QAbstract3DGraph::ElementAxisZLabel: m_state = StateDraggingZ; break;
    default: m_state = StateNormal; break;
  }
}

void CustomInputHandler::handleAxisDragging(){
  float distance = 0.f;
  float x_r = scene()->activeCamera()->xRotation();
  float x_mutlix = std::cos(qDegreesToRadians(x_r));
  float x_mutliy = std::sin(qDegreesToRadians(x_r));
  float z_mutlix = std::sin(qDegreesToRadians(x_r));
  float z_mutliy = std::cos(qDegreesToRadians(x_r));
  QPoint move = inputPosition()-previousInputPos();
  switch(m_state){
    case StateDraggingX:
      distance = (move.x()*x_mutlix-move.y()*x_mutliy)*m_speedModifier;
      m_axisXMinValue -= distance;
      m_axisXMaxValue -= distance;
      if (m_axisXMinValue<m_areaMinValue){
        float dist=m_axisXMaxValue-m_axisXMinValue;
        m_axisXMinValue = m_areaMaxValue;
        m_axisXMaxValue = m_axisXMinValue+dist; }
      if (m_axisXMaxValue>m_areaMaxValue){
        float dist=m_axisXMaxValue-m_axisXMinValue;
        m_axisXMaxValue = m_areaMaxValue;
        m_axisXMinValue = m_axisXMaxValue-dist; }
      m_axisX->setRange(m_axisXMinValue,m_axisXMaxValue);
      break;
    case StateDraggingZ:
      distance = (move.x()*z_mutlix+move.y()*z_mutliy)*m_speedModifier;
      m_axisZMinValue += distance;
      m_axisZMaxValue += distance;
      if (m_axisZMinValue<m_areaMinValue){
        float dist=m_axisXMaxValue-m_axisXMinValue;
        m_axisZMinValue = m_areaMaxValue;
        m_axisZMaxValue = m_axisZMinValue+dist; }
      if (m_axisZMaxValue>m_areaMaxValue){
        float dist=m_axisZMaxValue-m_axisZMinValue;
        m_axisXMaxValue = m_areaMaxValue;
        m_axisXMinValue = m_axisZMaxValue-dist; }
      m_axisZ->setRange(m_axisZMinValue,m_axisZMaxValue);
      break;
    default:
      break;
  }
}  

void CustomInputHandler::checkConstraints(){
  if (m_axisXMinValue<m_areaMinValue) m_axisXMinValue = m_areaMinValue;
  if (m_axisXMaxValue>m_areaMaxValue) m_axisXMaxValue = m_areaMaxValue;
  if ((m_axisXMaxValue-m_axisXMinValue)<m_axisXMinRange){
    float adjust =(m_axisXMinRange-(m_axisXMaxValue-m_axisXMinValue))/2.f;
    m_axisXMinValue -= adjust; m_axisXMaxValue += adjust; }

  if (m_axisZMinValue<m_areaMinValue) m_axisZMinValue = m_areaMinValue;
  if (m_axisZMaxValue>m_areaMaxValue) m_axisZMaxValue = m_areaMaxValue;
  if ((m_axisZMaxValue-m_axisZMinValue)<m_axisZMinRange){
    float adjust =(m_axisZMinRange-(m_axisZMaxValue-m_axisZMinValue))/2.f;
    m_axisZMinValue -= adjust; m_axisZMaxValue += adjust; }
}

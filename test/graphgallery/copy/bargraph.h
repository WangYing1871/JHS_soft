#ifndef bargraph_H
#define bargraph_H 1 
#include <QtDataVisualization/q3dbars.h>
#include <QtCore/qobject.h>

class BarGraph : public QObject{
  Q_OBJECT

public:
  BarGraph();
  ~BarGraph();
  bool initialize(QSize const&, QSize const&);
  QWidget* barsWidget() {return m_barsWidget;}
  
private:
  Q3DBars* m_barsGraph = nullptr;
  QWidget* m_container = nullptr
    ,* m_barsWidget = nullptr;
};
#endif

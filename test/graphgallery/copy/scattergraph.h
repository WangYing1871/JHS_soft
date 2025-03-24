#ifndef scattergraph_H
#define scattergraph_H 1 
#include <QtCore/qobject.h>
#include <QtDataVisualization/q3dscatter.h>
class ScatterGraph : public QObject{
  Q_OBJECT

public:
  ScatterGraph();
  ~ScatterGraph();

  bool initialize(QSize const& minimumGraphSize, QSize const&);
  QWidget* scatterWidget() {return m_scatterWidget;}

private:
  Q3DScatter* m_scatterGraph = nullptr;
  QWidget* m_container = nullptr;
  QWidget* m_scatterWidget = nullptr;
};
#endif

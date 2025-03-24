#ifndef rainfalldata_H
#define rainfalldata_H 1 
#include <QtDataVisualization/qcategory3daxis.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include "variantbardataproxy.h"

class RainfallData : public QObject{
  Q_OBJECT

public:
  explicit RainfallData();
  ~RainfallData();

  void addDataSet();

  QBar3DSeries* customSeries() {return m_series;}
  QCategory3DAxis* rowAxis() {return m_rowAxis;}
  QCategory3DAxis* colAxis() {return m_colAxis;}

private:
  void updateYearsList(int start, int end);
  int m_columnCount;
  int m_rowCount;
  QStringList m_years;
  QStringList m_numericMonths;
  VariantBarDataProxy *m_proxy;
  VariantBarDataMapping *m_mapping;
  VariantDataSet *m_dataSet;
  QBar3DSeries *m_series;
  QValue3DAxis *m_valueAxis;
  QCategory3DAxis *m_rowAxis;
  QCategory3DAxis *m_colAxis;
};

#endif

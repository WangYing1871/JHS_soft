#ifndef variantdataset_H
#define variantdataset_H 1 
#include <QtCore/qvariantlist.h>
typedef QVariantList VariantDataItem;
typedef QList<QVariantList> VariantDataItemList;

class VariantDataSet : public QObject{
  Q_OBJECT

public:
  explicit VariantDataSet();
  ~VariantDataSet();

  void clear();

  int addItem(VariantDataItem*);
  int addItems(VariantDataItemList*);
  VariantDataItemList const& itemList() const;

Q_SIGNALS:
  void itemsAdded(int index, int count);
  void dataCleared();

private:
  VariantDataItemList m_variantData;
  Q_DISABLE_COPY(VariantDataSet)
};

#endif

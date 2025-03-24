#ifndef variantbardatamapping_H
#define variantbardatamapping_H 1 
#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>
class VariantBarDataMapping : public QObject{
  Q_OBJECT
  Q_PROPERTY(int rowIndex READ rowIndex WRITE setRowIndex NOTIFY rowIndexChanged)
  Q_PROPERTY(int columnIndex READ columnIndex WRITE setColumnIndex NOTIFY columnIndexChanged)
  Q_PROPERTY(int valueIndex READ valueIndex WRITE setValueIndex NOTIFY valueIndexChanged)
  Q_PROPERTY(QStringList rowCategories READ rowCategories WRITE setRowCategories NOTIFY rowCategoriesChanged)
  Q_PROPERTY(QStringList columnCategories READ columnCategories WRITE setColumnCategories NOTIFY columnCategoriesChanged)

public:
  explicit VariantBarDataMapping(int,int,int);

};
#endif


#ifndef SceneGraphWidget_H
#define SceneGraphWidget_H

#include <QSet>

#include "DockWidget.h"

class QModelIndex;
class QTreeView;
class SceneGraphViewModel;

class SceneGraphWidget : public DockWidget
{
  Q_OBJECT

public:
  SceneGraphWidget(const SceneObject& object, QWidget* parent);

  QAction* toggleViewAction() const;

private slots:
  void itemActivatedSlot(const QModelIndex&);
  void itemCollapsedSlot(const QModelIndex &index);
  void itemExpandedSlot(const QModelIndex &index);
  void closeTreeSlot();
  void restoreLayoutSlot();
  void writeLayoutSlot();
  void updateObject();

private:
  QTreeView* treeView;
  SceneGraphViewModel* viewModel;
  QSet<QString> expandedItems;

  QString getFullName(const QModelIndex &index) const;
};

#endif // SceneGraphWidget_H

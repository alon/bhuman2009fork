
#include <QTreeView>
#include <QHeaderView>
#include <QSettings>
#include <QAction>

#include "SceneGraphWidget.h"
#include "Document.h"

class SceneGraphViewModel : public QAbstractItemModel
{
public:
  SceneGraphViewModel(QTreeView* treeView) : QAbstractItemModel(treeView),
    rootIcon(QIcon(":/icons/bricks.png")),
    objectIcon(QIcon(":/icons/brick.png")), sensorIcon(QIcon(":/icons/transmit_go.png")), 
    actuatorIcon(QIcon(":/icons/arrow_rotate_clockwise.png")), categoryIcon(QIcon(":/icons/folder.png")) {}
  ~SceneGraphViewModel() {}

  void clear()
  {
    if(root.childs.count() > 0)
    {
      beginRemoveRows(QModelIndex(), 0, root.childs.count() - 1);
      root.remove(0, root.childs.count());
      endRemoveRows();
    }
  }

  void update(const std::vector<ObjectDescription> &objs, const QSet<QString>& expandedItems, QList<QModelIndex>& itemsToExpand)
  {
    if(objs.size() == 0)
    {
      clear();
      return;
    }

    int currentDepth = 0;
    Item* currentParent = &root;
    currentParent->currentIndex = 0;
    QModelIndex currentModelIndex;

    bool initialBuild = false;
    if(root.count() == 0)
    {
      initialBuild = true;
      beginInsertRows(currentModelIndex, 0, 0);
    }

    for(std::vector<ObjectDescription>::const_iterator i = objs.begin(), end = objs.end(); i != end; ++i)
    {
      if(i->depth < currentDepth) //if we need to go upwards
      {
        do //go upwards till we reach the level we want to
        {
          // remove all not passed items
          int childCount = currentParent->count();
          if(currentParent->currentIndex < childCount)
          {
            Q_ASSERT(!initialBuild);
            beginRemoveRows(currentModelIndex, currentParent->currentIndex, childCount - 1);
            currentParent->remove(currentParent->currentIndex, childCount - currentParent->currentIndex);
            endRemoveRows();
            Q_ASSERT(currentParent->currentIndex >= currentParent->childs.count());
          }

          // now really go upwards
          currentParent = currentParent->parent;
          Q_ASSERT(currentParent);
          currentDepth--;
          currentModelIndex = parent(currentModelIndex);
        } while(i->depth < currentDepth); 
      }
      if(i->depth > currentDepth) //if we need to go downwards
      {
        Q_ASSERT(currentParent->currentIndex > 0);
        Q_ASSERT(currentParent->currentIndex - 1 < currentParent->childs.count());
        int row = currentParent->currentIndex - 1;
        currentParent = currentParent->childs[row];
        currentDepth++;
        Q_ASSERT(i->depth == currentDepth);
        currentParent->currentIndex = 0;
        currentModelIndex = index(row, 0, currentModelIndex);
      }

      int childCount = currentParent->count();
      if(currentParent->currentIndex < childCount &&
        currentParent->childs[currentParent->currentIndex]->name == i->name)
      { // item already exists, pass it
        currentParent->currentIndex++;
      }
      else
      { // try to find the item in "yonger" childrens
        int j;
        for(j = currentParent->currentIndex + 1; j < childCount; j++)
          if(currentParent->childs[j]->name == i->name)
            break;

        if(j < childCount)
        { // if found remove all items above
          Q_ASSERT(!initialBuild);
          beginRemoveRows(currentModelIndex, currentParent->currentIndex, j - 1);
          currentParent->remove(currentParent->currentIndex, j - currentParent->currentIndex);
          endRemoveRows();

          // and item already exists, pass it
          currentParent->currentIndex++;
        }
        else
        {
          // insert new item
          if(!initialBuild)
            beginInsertRows(currentModelIndex, currentParent->currentIndex, currentParent->currentIndex);
          currentParent->insert(currentParent->currentIndex, i->name, i->type);
          if(!initialBuild)
            endInsertRows();
          if(expandedItems.contains(QString(i->fullName.c_str())))
            itemsToExpand.append(index(currentParent->currentIndex, 0, currentModelIndex));
          currentParent->currentIndex++; // and pass it
        }
      }
    }

    if(initialBuild)
    {
      Q_ASSERT(root.count() == 1);
      endInsertRows();
    }
  }

  class Item
  {
  public:

    inline const Item* at(int index) const    
    {
      return childs.at(index);
    }

    inline int count() const
    {
      return childs.count();
    }

    const std::string& getName() const
    {
      return name;
    }

    const Item* getParent() const
    {
      return parent;
    }

    ObjectType getType() const
    {
      return type;
    }

  private:
    std::string name;
    ObjectType type;
    QVector<Item*> childs;
    Item* parent;
    int currentIndex; /**< A walkthrough index used in update() */ 

    Item() : parent(0) {}
    Item(const std::string& name, ObjectType type, Item* parent) : 
      name(name), type(type), parent(parent) {}

    ~Item()
    {
      for(int i = childs.count() - 1; i >= 0; --i)
        delete childs[i];
    }

    inline void insert(int index, const std::string& name, ObjectType type)
    {
      childs.insert(index, new Item(name, type, this));
    }

    inline void remove(int index, int count)
    {
      for(int i = index, end = index + count; i < end; ++i)
        delete childs[i];
      childs.remove(index, count);
    }

    inline int row() const
    {
      return parent->childs.indexOf((Item*)this);
    }

    friend class SceneGraphViewModel;
  };

  const Item* getRoot() const
  {
    return &root;
  }

private:
  QTreeView* treeView;
  Item root;
  const QVariant rootIcon;
  const QVariant objectIcon;
  const QVariant sensorIcon;
  const QVariant actuatorIcon;
  const QVariant categoryIcon;

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
  {
    if(!hasIndex(row, column, parent))
      return QModelIndex();

    const Item *parentItem;
    if(parent.isValid())
      parentItem = (const Item*)parent.internalPointer();
    else
      parentItem = &root;
    
    return createIndex(row, column, (void*)parentItem->at(row));
  }

  QModelIndex parent(const QModelIndex& index) const
  {
    if(!index.isValid())
      return QModelIndex();
    
    const Item* childItem = (const Item*)index.internalPointer();
    const Item* parentItem = childItem->parent;
    if(parentItem == &root)
       return QModelIndex();

    return createIndex(parentItem->row(), 0, (void*)parentItem);
  }

  int rowCount(const QModelIndex& index) const
  {
    if(index.column() > 0)
       return 0;

    const Item* item;
    if(index.isValid())
      item = (const Item*)index.internalPointer();
    else
      item = &root;

    return item->count();
  }

  int columnCount(const QModelIndex& index) const
  {
    return 1;
  }

  QVariant data(const QModelIndex& index, int role) const
  {
    if(!index.isValid())
       return QVariant();
    if(role == Qt::DisplayRole)
    {
      const Item *item = (const Item*)index.internalPointer();
      return QVariant(QString(item->name.c_str()));
    }
    else if(role == Qt::DecorationRole)
    {
      
      const Item *item = (const Item*)index.internalPointer();
      if(item->parent == &root)
        return rootIcon;
      switch(item->type)
      {
      case OBJECT_TYPE_OBJECT:
        return objectIcon;
        break;
      case OBJECT_TYPE_ACTUATORPORT:
        return actuatorIcon;
        break;
      case OBJECT_TYPE_SENSORPORT:
        return sensorIcon;
        break;
      default:
        return categoryIcon;
        break;
      }
      
    }
    
    return QVariant();
  }

  Qt::ItemFlags flags(const QModelIndex& index) const
  {
    if(!index.isValid())
      return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }
};

class SceneGraphView : public QTreeView
{
public:
  SceneGraphView(QWidget* parent) : QTreeView(parent) {}
private:
  QSize sizeHint () const { return QSize(200, 400); }
};

SceneGraphWidget::SceneGraphWidget(const SceneObject& object, QWidget* parent) : DockWidget(object, parent)
{
  setWindowTitle(tr("Scene Graph"));
  treeView = new SceneGraphView(this);
  setWidget(treeView);
  setFocusProxy(treeView);
  viewModel = new SceneGraphViewModel(treeView);
  treeView->setModel(viewModel);
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
  treeView->setExpandsOnDoubleClick(false);
#endif
  treeView->header()->hide();

  connect(treeView, SIGNAL(activated(const QModelIndex&)), this, SLOT(itemActivatedSlot(const QModelIndex&)));
  connect(treeView, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(itemCollapsedSlot(const QModelIndex&)));
  connect(treeView, SIGNAL(expanded(const QModelIndex&)), this, SLOT(itemExpandedSlot(const QModelIndex&)));

  connect(Document::document, SIGNAL(updatedSceneGraph()), this, SLOT(updateObject()));
  connect(Document::document, SIGNAL(closeSignal()), SLOT(closeTreeSlot()));
  connect(Document::document, SIGNAL(restoreLayoutSignal()), SLOT(restoreLayoutSlot()));
  connect(Document::document, SIGNAL(writeLayoutSignal()), SLOT(writeLayoutSlot()));
}

void SceneGraphWidget::updateObject()
{
  const std::vector<ObjectDescription> &objs(Document::document->getObjectTree());
  QList<QModelIndex> itemsToExpand;
  viewModel->update(objs, expandedItems, itemsToExpand);

  for(QList<QModelIndex>::iterator i = itemsToExpand.begin(), end = itemsToExpand.end(); i != end; ++i)
    treeView->setExpanded(*i, true);
}

void SceneGraphWidget::closeTreeSlot()
{
  viewModel->clear();
  expandedItems.clear();
}

void SceneGraphWidget::itemActivatedSlot(const QModelIndex& itemIndex)
{
  const SceneGraphViewModel::Item* item = (const SceneGraphViewModel::Item*)itemIndex.internalPointer();
  if(item->getType() != OBJECT_TYPE_NONE)
  {
    SceneObject obj(getFullName(itemIndex), SceneObject::Type(item->getType()));
    Document::document->openObject(obj);
  }
  else
    treeView->setExpanded(itemIndex, !treeView->isExpanded(itemIndex));
}

void SceneGraphWidget::itemCollapsedSlot(const QModelIndex &index)
{
  expandedItems.remove(getFullName(index));
}

void SceneGraphWidget::itemExpandedSlot(const QModelIndex& index)
{
  expandedItems.insert(getFullName(index));
}

QString SceneGraphWidget::getFullName(const QModelIndex &index) const
{
  const SceneGraphViewModel::Item* root = viewModel->getRoot();
  const SceneGraphViewModel::Item* item = (const SceneGraphViewModel::Item*)index.internalPointer();
  std::string fullName(item->getName());
  for(;;)
  {
    item = item->getParent();
    if(item == root)
      break;
    fullName = item->getName() + '.' + fullName;
  }
  return QString(fullName.c_str());
}

void SceneGraphWidget::restoreLayoutSlot()
{
  QSettings* settings = Document::document->getLayoutSettings();
  settings->beginGroup("Tree");

  const QByteArray& buffer(settings->value("ExpandedItems").toByteArray());
  QDataStream in(buffer);
  in.setVersion(QDataStream::Qt_4_3);
  in >> expandedItems;

  settings->endGroup();
}

void SceneGraphWidget::writeLayoutSlot()
{
  QSettings* settings = Document::document->getLayoutSettings();
  settings->beginGroup("Tree");

  QByteArray buffer;
  QDataStream out(&buffer, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_3);
  out << expandedItems;
  settings->setValue("ExpandedItems", buffer);

  settings->endGroup();
}

QAction *SceneGraphWidget::toggleViewAction() const
{
  QAction* action = DockWidget::toggleViewAction();
  action->setIcon(QIcon(":/icons/chart_organisation_grey.png"));
  return action;
}

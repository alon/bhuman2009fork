
#ifndef EditorWidget_H
#define EditorWidget_H

#include "DockWidget.h"

class EditorView;

class EditorWidget : public DockWidget
{
  Q_OBJECT

public:
  EditorWidget(const SceneObject& object, QWidget* parent);

  QMenu* createPopupMenu();

signals:
  void modificationChanged(bool changed);

private slots:
  void visibilityChanged(bool visible);
  void updateWindowTitle(bool changed);
  void aboutToSave();
  void saved();
  void copyAvailable(bool available);
  void redoAvailable(bool available);
  void undoAvailable(bool available);
  void cut();
  void copy();
  void deleteText();

private:
  EditorView* textEdit;
  bool loaded;

  QAction* undoAct;
  QAction* redoAct;
  QAction* cutAct;
  QAction* copyAct;
  QAction* pasteAct;
  QAction* deleteAct;

  bool canCopy;
  bool canUndo;
  bool canRedo;

  friend class EditorView;
};

#endif // EditorWidget_H

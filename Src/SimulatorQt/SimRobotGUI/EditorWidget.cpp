
#include <QTextEdit>
#include <QMenu>

#include "EditorWidget.h"

class EditorView : public QTextEdit
{
public:
  EditorView(QWidget* parent) : QTextEdit(parent) {}
private:
  QSize sizeHint () const { return QSize(600, 400); }
  void focusInEvent(QFocusEvent* event)
  {
    QTextEdit::focusInEvent(event);
    EditorWidget* editorWidget = (EditorWidget*)parent();
    if(editorWidget->pasteAct)
      editorWidget->pasteAct->setEnabled(canPaste());
  }
};

EditorWidget::EditorWidget(const SceneObject& object, QWidget* parent) : DockWidget(object, parent), loaded(false),
  undoAct(0), redoAct(0), cutAct(0), copyAct(0), pasteAct(0), deleteAct(0),
  canCopy(false), canUndo(false), canRedo(false) 
{
  setWindowTitle(tr("Editor"));
  textEdit = new EditorView(this);

#ifdef _WIN32
  QFont font("Courier New", 10);
#else
  QFont font("Bitstream Vera Sans Mono", 9);
#endif
  textEdit->setFont(font);
  textEdit->setLineWrapMode(QTextEdit::NoWrap);

  setWidget(textEdit);
  setFocusProxy(textEdit);

  QTextDocument* textDocument = textEdit->document();
  connect(textDocument, SIGNAL(modificationChanged(bool)), this, SLOT(updateWindowTitle(bool)));
  connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)));
  connect(Document::document, SIGNAL(aboutToSave()), this, SLOT(aboutToSave()));
  connect(Document::document, SIGNAL(saved()), this, SLOT(saved()));

  connect(textEdit, SIGNAL(copyAvailable(bool)), this, SLOT(copyAvailable(bool)));
  connect(textEdit, SIGNAL(undoAvailable(bool)), this, SLOT(undoAvailable(bool)));
  connect(textEdit, SIGNAL(redoAvailable(bool)), this, SLOT(redoAvailable(bool)));
}

void EditorWidget::visibilityChanged(bool visible)
{
  if(visible && !loaded)
  {
    textEdit->setPlainText(Document::document->getFileContent());
    loaded = true;
    textEdit->document()->setModified(false);
  }
}

void EditorWidget::updateWindowTitle(bool changed)
{
  if(!loaded)
    return;

  if(changed)
    setWindowTitle(tr("Editor*"));
  else
    setWindowTitle(tr("Editor"));
  emit modificationChanged(changed);
}

void EditorWidget::aboutToSave()
{
  if(loaded && textEdit->document()->isModified())
    Document::document->setFileContent(textEdit->toPlainText());
}


void EditorWidget::saved()
{
  if(loaded)
    textEdit->document()->setModified(false);
}

QMenu* EditorWidget::createPopupMenu()
{
  QMenu* menu = new QMenu(tr("&Edit"));

  if(!undoAct)
  {
    undoAct = new QAction(QIcon(":/icons/arrow_undo.png"), tr("&Undo"), this);
    undoAct->setShortcut(QKeySequence(QKeySequence::Undo));
    undoAct->setStatusTip(tr("Undo the last action"));
    undoAct->setEnabled(canUndo);
    connect(undoAct, SIGNAL(triggered()), textEdit, SLOT(undo()));
  }
  menu->addAction(undoAct);

  if(!redoAct)
  {
    redoAct = new QAction(QIcon(":/icons/arrow_redo.png"), tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence(QKeySequence::Redo));
    redoAct->setStatusTip(tr("Redo the previously undone action"));
    redoAct->setEnabled(canRedo);
    connect(redoAct, SIGNAL(triggered()), textEdit, SLOT(redo()));
  }
  menu->addAction(redoAct);
  menu->addSeparator();

  if(!cutAct)
  {
    cutAct = new QAction(QIcon(":/icons/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(QKeySequence(QKeySequence::Cut));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    cutAct->setEnabled(canCopy);
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));
  }
  menu->addAction(cutAct);

  if(!copyAct)
  {
    copyAct = new QAction(QIcon(":/icons/page_copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(QKeySequence(QKeySequence::Copy));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    copyAct->setEnabled(canCopy);
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));
  }
  menu->addAction(copyAct);

  if(!pasteAct)
  {
    pasteAct = new QAction(QIcon(":/icons/page_paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(QKeySequence(QKeySequence::Paste));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));
  }
  pasteAct->setEnabled(textEdit->canPaste());
  menu->addAction(pasteAct);

  if(!deleteAct)
  {
    deleteAct = new QAction(tr("&Delete"), this);
    deleteAct->setShortcut(QKeySequence(QKeySequence::Delete));
    deleteAct->setStatusTip(tr("Delete the currently selected content"));
    deleteAct->setEnabled(canCopy);
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteText()));
  }
  menu->addAction(deleteAct);
  menu->addSeparator();

  QAction* action = menu->addAction(tr("Select &All"));
  action->setShortcut(QKeySequence(QKeySequence::SelectAll));
  action->setStatusTip(tr("Select the whole document"));
  connect(action, SIGNAL(triggered()), textEdit, SLOT(selectAll()));

  return menu;
}

void EditorWidget::copyAvailable(bool available)
{
  canCopy = available;
  if(copyAct)
    copyAct->setEnabled(available);
  if(cutAct)
    cutAct->setEnabled(available);
  if(deleteAct)
    deleteAct->setEnabled(available);
}

void EditorWidget::redoAvailable(bool available)
{
  canRedo = available;
  if(redoAct)
    redoAct->setEnabled(available);
}

void EditorWidget::undoAvailable(bool available)
{
  canUndo = available;
  if(undoAct)
    undoAct->setEnabled(available);
}

void EditorWidget::cut()
{
  textEdit->cut();
  if(pasteAct)
    pasteAct->setEnabled(textEdit->canPaste());
}

void EditorWidget::copy()
{
  textEdit->copy();
  if(pasteAct)
    pasteAct->setEnabled(textEdit->canPaste());
}

void EditorWidget::deleteText()
{
  textEdit->insertPlainText(QString());
}


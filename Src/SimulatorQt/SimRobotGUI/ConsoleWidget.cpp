
#include <QApplication>
#include <QScrollBar>
#include <QTextEdit>
#include <QKeyEvent>
#include <QAction>

#include "ConsoleWidget.h"
#include "Document.h"

class ConsoleTextEdit : public QTextEdit
{
public:
  ConsoleTextEdit(QWidget* parent) : QTextEdit(parent) {}

private:
  void keyPressEvent(QKeyEvent* event)
  {
    switch(event->key())
    {
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
      event->accept();
      {
        QTextCursor cursor = textCursor();
        int begin = cursor.position();
        int end = cursor.anchor();
        if(end < begin)
        {
          int tmp = end;
          end = begin;
          begin = tmp;
        }
        cursor.setPosition(end);
        cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
        QString line = cursor.selectedText();
        int lineLength = begin - cursor.position();
        Document::document->completeConsoleCommand(line, event->key() == Qt::Key_Tab, begin == end);
        cursor.insertText(line);
        cursor.setPosition(begin);
        cursor.setPosition(begin + (line.length() - lineLength), QTextCursor::KeepAnchor);      
        setTextCursor(cursor);
      }
      break;

    case Qt::Key_Return:
    case Qt::Key_Enter:
      if(event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier |Qt::MetaModifier))
      {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::EndOfBlock);
        cursor.insertBlock();
        setTextCursor(cursor);
      } 
      else
      {
        event->accept();
        {
          QTextCursor cursor = textCursor();
          cursor.movePosition(QTextCursor::StartOfBlock);
          cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
          QString line = cursor.selectedText();
          cursor.movePosition(QTextCursor::EndOfLine);
          if(cursor.atEnd())
            cursor.insertText("\n");
          cursor.movePosition(QTextCursor::NextBlock);
          setTextCursor(cursor);
          
          Document::document->handleConsoleCommand(line);      
        }
      }
      break;

    default:
      QTextEdit::keyPressEvent(event);
      break;
    }
  }

  QSize sizeHint () const { return QSize(400, 200); }
};

ConsoleWidget::ConsoleWidget(const SceneObject& object, QWidget *parent) : DockWidget(object, parent)
{
  setWindowTitle(tr("Console"));
  setFeatures(features() | DockWidgetVerticalTitleBar);

  textEdit = new ConsoleTextEdit(this);
  setWidget(textEdit);
  setFocusProxy(textEdit);

  connect(Document::document, SIGNAL(clearConsoleSignal()), this, SLOT(clear()));
  connect(Document::document, SIGNAL(printSignal(const QString&)), this, SLOT(print(const QString&)));
}


void ConsoleWidget::clear()
{
  textEdit->setPlainText("");
}

void ConsoleWidget::print(const QString& text)
{
  QScrollBar* scrollBar = textEdit->verticalScrollBar();
  bool scroll = scrollBar->value() == scrollBar->maximum();
  QTextCursor cursor = textEdit->textCursor();
  cursor.movePosition(QTextCursor::StartOfBlock);
  /*
  cursor.movePosition(QTextCursor::NextBlock);
  if(cursor.atEnd() && !cursor.atStart())
  {
    cursor.insertText("\n");
    cursor.movePosition(QTextCursor::NextBlock);
  }*/
  cursor.insertText(text);
  if(scroll)
    scrollBar->setValue(scrollBar->maximum());
  cursor.movePosition(QTextCursor::End);
}

QAction *ConsoleWidget::toggleViewAction() const
{
  QAction* action = DockWidget::toggleViewAction();
  action->setIcon(QIcon(":/icons/textfield.png"));
  return action;
}

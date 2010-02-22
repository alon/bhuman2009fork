
#include <QLabel>
#include <QSizeGrip>
#include <QAction>

#include <Simulation/Simulation.h>
#include <Platform/OffscreenRenderer.h>

#include "StatusBar.h"
#include "Document.h"

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent),
  lastGotSimulation(false), lastRenderingMethod(int(OffscreenRenderer::UNKNOWN)), toggleViewAct(0), visible(true)
{
  collisionsLabel = new QLabel(this);
  collisionsLabel->setMinimumWidth(50);
  collisionsLabel->setEnabled(false);
  frameRateLabel = new QLabel(this);
  frameRateLabel->setMinimumWidth(50);
  frameRateLabel->setEnabled(false);
  stepsLabel = new QLabel(this);
  stepsLabel->setMinimumWidth(50);
  stepsLabel->setEnabled(false);
  rendererLabel = new QLabel(this);
  rendererLabel->setMinimumWidth(50);

  setSizeGripEnabled(false);

  addPermanentWidget(collisionsLabel);
  addPermanentWidget(frameRateLabel);
  addPermanentWidget(stepsLabel);
  addPermanentWidget(rendererLabel);

  addPermanentWidget(new QSizeGrip(this));

  showMessage("Ready");

  connect(Document::document, SIGNAL(updateStatusBarSignal()), SLOT(updateSlot()));
}

void StatusBar::hideEvent(QHideEvent* event)
{
  QStatusBar::hideEvent(event);

  if(toggleViewAct)
    toggleViewAct->setChecked(false);
}

void StatusBar::showEvent(QShowEvent * event)
{
  QStatusBar::showEvent(event);

  if(toggleViewAct)
    toggleViewAct->setChecked(true);
}

void StatusBar::updateSlot()
{
  if(!isVisible())
    return;

  const QString& newMessage(Document::document->getStatusMessage());
  const QString& curMessage(currentMessage());
  if(curMessage.isEmpty() || curMessage == lastStatusMessage)
    showMessage(newMessage);
  lastStatusMessage = newMessage;

  Simulation* simulation = Document::document->getSimulation();
  bool gotSimulation = simulation ? true : false;
  if(gotSimulation != lastGotSimulation)
  {
    lastGotSimulation = gotSimulation;
    collisionsLabel->setEnabled(gotSimulation);
    frameRateLabel->setEnabled(gotSimulation);
    stepsLabel->setEnabled(gotSimulation);
  }
  if(simulation)
  {
    char buf[20];
    int val;

    sprintf(buf, "%03d", simulation->getNumberOfCollisions());
    collisionsLabel->setText(buf);

    val = simulation->getFrameRate();
    if(val > 99999)
      val = 99999;
    sprintf(buf, "%d", val);
    frameRateLabel->setText(buf);

    sprintf(buf, "%06d", simulation->getSimulationStep());
    stepsLabel->setText(buf);

    if(lastRenderingMethod == OffscreenRenderer::UNKNOWN)
    {
      const OffscreenRenderer* renderer(simulation->getOffscreenRenderer());
      if(renderer)
      {
        int renderingMethod = simulation->getOffscreenRenderer()->getRenderingMethod();
        if(renderingMethod != lastRenderingMethod)
        {
          lastRenderingMethod = renderingMethod;
          static const char* renderingMethods[] = {
            "", "pbuf", "fbo", "win"
          };
          rendererLabel->setText(tr(renderingMethods[(renderingMethod < 0 || renderingMethod >= int(sizeof(renderingMethods)/sizeof(*renderingMethods))) ? 0 : renderingMethod]));
        }
      }
      else
      {
        rendererLabel->setText(tr("err"));
      }
    }
  }
  else
  {    
    collisionsLabel->setText("000");
    frameRateLabel->setText("00000");
    stepsLabel->setText("000000");
  }
}

QAction* StatusBar::toggleViewAction()
{
  if(toggleViewAct)
    return toggleViewAct;
  toggleViewAct = new QAction(tr("&Status Bar"), this);
  toggleViewAct->setCheckable(true);
  toggleViewAct->setChecked(isVisible());
  connect(toggleViewAct, SIGNAL(triggered(bool)), this, SLOT(setVisible(bool)));
  return toggleViewAct;
}

bool StatusBar::wasVisible() const
{
  return visible;
}

void StatusBar::setVisible(bool vis)
{
  visible = vis;
  QStatusBar::setVisible(vis);
}

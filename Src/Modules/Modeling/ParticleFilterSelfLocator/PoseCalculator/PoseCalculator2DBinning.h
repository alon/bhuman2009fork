/**
* @file PoseCalculator2DBinning.h
*
* A class for computing a pose from a given sample set.
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef _PoseCalculator2DBinning_H_
#define _PoseCalculator2DBinning_H_

#include "PoseCalculator.h"


/**
* @class PoseCalculator2DBinning
* A class for computing a pose from a given sample set.
*/
template <class Sample, class SampleContainer, int numberOfGridCells>
class PoseCalculator2DBinning: public PoseCalculator<Sample, SampleContainer>
{
private:
  /**
  * A cell in the grid that is used to cluster the samples.
  */
  class Cell
  {
  public:
    Cell* next; /**< The next cell in use. */
    int count; /**< The number of samples in this square. */
    Sample* first; /**< The first sample in this square. */
  };

  Cell poseSpace[numberOfGridCells][numberOfGridCells]; /**< A grid of cells for clustering samples. */
  const FieldDimensions& theFieldDimensions;
  Vector2<int> winningSquareCoordiates;

public:
  /** Default constructor. */
  PoseCalculator2DBinning(SampleContainer& samples, const FieldDimensions& theFieldDimensions):
      PoseCalculator<Sample, SampleContainer>(samples), theFieldDimensions(theFieldDimensions)
  {
    memset(poseSpace, 0, sizeof(poseSpace));
  }

  void calcPose(RobotPose& robotPose)
  {
    const int width = int(theFieldDimensions.x.getSize()),
      height = int(theFieldDimensions.y.getSize());
    int xSquareMin = numberOfGridCells,
      xSquareMax = 0,
      ySquareMin = numberOfGridCells,
      ySquareMax = 0;
    Cell* firstCell = 0;

    // attach the samples to the corresponding cells of the poseSpace
    for(int i = 0; i < this->samples.size(); i++)
    {
      Sample& sample = this->samples.at(i);

      if(sample.weighting)
      {
        //  x,y: Index on the translation axes. similar to the rotation, the 0 position
        //       is at 0.5*numberOfGridCells.  	
        int x = (sample.translation.x * numberOfGridCells + (numberOfGridCells >> 1)*width) / width;
        int y = (sample.translation.y * numberOfGridCells + (numberOfGridCells >> 1)*height)/ height;

        // translation axes are non-cyclic, so we set [x|y] < 0 to 0 and 
        // [x|y] >= numberOfGridCells to numberOfGridCells-1
        if(x < 0) 
          x = 0;
        else if(x >= numberOfGridCells) 
          x = numberOfGridCells - 1;
        if(x < xSquareMin)
          xSquareMin = x;
        if(x > xSquareMax)
          xSquareMax = x;

        if(y < 0) 
          y = 0;
        else if(y >= numberOfGridCells) 
          y = numberOfGridCells - 1;
        if(y < ySquareMin)
          ySquareMin = y;
        if(y > ySquareMax)
          ySquareMax = y;

        // fill the queue of samples for this cell by 
        // inserting before first sample
        Cell& cell = poseSpace[y][x];
        if(!cell.count)
        {
          cell.next = firstCell;
          firstCell = &cell;
          sample.next = 0;
        }
        else
          sample.next = cell.first;
        cell.first = &sample;
        ++cell.count;
      }
    } 

    // increase size of bounding box if it is empty (x)
    if(xSquareMin == xSquareMax)
    {
      if(xSquareMax == numberOfGridCells - 1)
      {
        --xSquareMin;
      }
      else
      {
        ++xSquareMax;
      }
    }
    // increase size of bounding box if it is empty (y)
    if(ySquareMin == ySquareMax)
    {
      if(ySquareMax == numberOfGridCells - 1)
      {
        --ySquareMin;
      }
      else
      {
        ++ySquareMax;
      }
    }
    // determine the 2x2 sub-square that contains most samples

    // the position and sample-count of the winning sub-square
    int yMax(0),
      xMax(0),
      countMax(0);

    // iterate with "x" and "y" over the translation axes
    for(int y = ySquareMin; y < ySquareMax; ++y)
      for(int x = xSquareMin; x < xSquareMax; ++x)
      {
        // get the number of all samples in the sub-square
        // around the current cell [y][x]
        const int count = poseSpace[y][x].count +
          poseSpace[y][x+1].count +
          poseSpace[y+1][x].count +
          poseSpace[y+1][x+1].count;
        // if we've got a new winning square, we have to remember
        if(count > countMax)
        {
          countMax = count;
          yMax = y;
          xMax = x;
        }
      }

      if(countMax)
        calcPoseFromSubSquare(xMax, yMax, robotPose);
      else 
        robotPose.validity = 0;

      // delete the cells used.
      while(firstCell)
      {
        firstCell->count = 0;
        firstCell = firstCell->next;
      }
  }

  void draw()
  {
    const int fieldLength = int(theFieldDimensions.x.getSize()),
      fieldWidth = int(theFieldDimensions.y.getSize()),
      squareLength = fieldLength / numberOfGridCells,
      squareWidth = fieldWidth / numberOfGridCells;
    // Draw all squares:
    for(int x = -fieldLength/2; x < fieldLength/2; x += squareLength)
    {
      for(int y = -fieldWidth/2; y < fieldWidth/2; y += squareWidth)
      {
        QUADRANGLE("module:SelfLocator:poseCalculator", x, y, x+squareLength, y, x+squareLength, y+squareWidth, 
          x, y+squareWidth, 10, Drawings::ps_solid, ColorRGBA(255,0,0));
      }
    }
    // Draw winner square:
    const int xWinner = squareLength*winningSquareCoordiates.x - fieldLength/2;
    const int yWinner = squareWidth*winningSquareCoordiates.y - fieldWidth/2;
    QUADRANGLE("module:SelfLocator:poseCalculator", xWinner, yWinner, xWinner+2*squareLength, yWinner,
      xWinner+2*squareLength, yWinner+2*squareWidth, xWinner, yWinner+2*squareWidth, 50, Drawings::ps_dash, ColorRGBA(255,0,0));
    QUADRANGLE("module:SelfLocator:poseCalculator", xWinner, yWinner, xWinner+squareLength, yWinner,
      xWinner+squareLength, yWinner+squareWidth, xWinner, yWinner+squareWidth, 50, Drawings::ps_dash, ColorRGBA(0,0,100));
  }

private:
  void calcPoseFromSubSquare(int xMax, int yMax, RobotPose& robotPose) 
  {
    winningSquareCoordiates.x = xMax;
    winningSquareCoordiates.y = yMax;

    // determine the average pose of all samples in the winner sub-square
    int xSum = 0,
      ySum = 0,
      cosSum = 0,
      sinSum = 0,
      sampleCount = 0;

    // iterate over the translation axes...
    for(int y = 0; y < 2; ++y)
      for(int x = 0; x < 2; ++x)
      {
        const Cell& cell = poseSpace[yMax + y][xMax + x];
        if(cell.count)
          // iterate over the list of samples inside the cell
          // cell.first is 0 if the queue is empty
          // else p->next is 0 at the end of the queue	
          for(Sample* sample = cell.first; sample; sample = sample->next)
          {
            // sum the x and y positions of the samples 
            // sum the sine and cosine of the rotations
            xSum += sample->translation.x;
            ySum += sample->translation.y;
            cosSum += sample->rotation.x;
            sinSum += sample->rotation.y;
            ++sampleCount;
          }
      }

      // now calculate the pose as the averages of x,y,r
      if(this->samples.size() > 0)
      {
        const double avgX = static_cast<double>(xSum) / sampleCount;
        const double avgY = static_cast<double>(ySum) / sampleCount;
        const double avgRot = atan2(static_cast<double>(sinSum), static_cast<double>(cosSum));
        robotPose = Pose2D(avgRot, avgX, avgY);
        robotPose.validity = (static_cast<double>(sampleCount) / this->samples.size());
      } 
  }
};


#endif

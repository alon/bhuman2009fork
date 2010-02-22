/**
* @file LineSensorModel.h
*
* Sensor model for updating samples by perceived points on lines
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#ifndef _LineSensorModel_h_
#define _LineSensorModel_h_

#include "FieldModel.h"

/**
* @class LineSensorModel
*/
class LineSensorModel: public SensorModel
{
private:
  const std::vector<const LinePercept::Line*>& lines; /**< Reference to the lines. */
  std::vector<short> selectedIndices; /**< The indices of the points in the points percept that were selected for the sensor update. */
  std::vector< std::pair<short,double> > selectedIndicesPositions; /**< Precomputed information (distance+angle) about selected points */
  GaussianTable gaussian; /**< A table of Gaussians for assessing bearing measurements to points. */
  const FieldModel& fieldModel; /**< The model of proximity to features on the field. */

public:
  /** Constructor. */
  LineSensorModel(const SelfLocatorParameter& selfLocatorParameter,
    const std::vector<const LinePercept::Line*>& lines, const FrameInfo& frameInfo, 
    const FieldDimensions& fieldDimensions, const CameraMatrix& cameraMatrix, 
    const PerceptValidityChecker& perceptValidityChecker,
    const FieldModel& fieldModel) :
    SensorModel(selfLocatorParameter, frameInfo, fieldDimensions, cameraMatrix, 
              perceptValidityChecker, Observation::POINT), 
    lines(lines), fieldModel(fieldModel)
  {}

  /** Function for computing weightings for a sample set.
  * @param samples The samples (not changed by this function
  * @param selectedIndices The indices of the selected observations.
  * @param weightings List of weightings. -1 means: no update
  * @return An overall result of the computation
  */
  SensorModelResult computeWeightings(const SampleSet<SelfLocatorSample>& samples,
    const std::vector<int>& selectedIndices, std::vector<double>& weightings)
  {
    // Precompute some values
    Pose2D origin(theCameraMatrix.rotation.getZAngle(), theCameraMatrix.translation.x, theCameraMatrix.translation.y);
    int xOrigin = int(origin.translation.x);
    int yOrigin = int(origin.translation.y);
    int rotCos  = int(floor(cos(origin.rotation) * (1 << 10) + 0.5));
    int rotSin  = int(floor(sin(origin.rotation) * (1 << 10) + 0.5));
    gaussian.computeTable(1000, 200, theSelfLocatorParameter.standardDeviationFieldLines, theSelfLocatorParameter.standardDeviationFieldLines, 1);
    selectedIndicesPositions.resize(selectedIndices.size());
    for(int j = 0; j < (int) selectedIndices.size(); ++j)
    {
      const Vector2<int>& pp = selectedIndices[j] & 1 ? lines[selectedIndices[j] >> 1]->first 
                                                      : lines[selectedIndices[j] >> 1]->last;
      const double angle = pp.angle();
      const short distance = static_cast<short>(pp.abs());
      selectedIndicesPositions[j] = std::make_pair(distance, angle);
    }
    SensorModelResult result = FULL_SENSOR_UPDATE;
    for(int i = 0; i < samples.size(); ++i)
    {
NEXT_SAMPLE:
      const SelfLocatorSample& s(samples.at(i));
      const Vector2<int> origin(int(s.translation.x), int(s.translation.y)),
                         camRotation((rotCos * s.rotation.x - rotSin * s.rotation.y) >> 10,
                                     (rotCos * s.rotation.y + rotSin * s.rotation.x) >> 10);

      weightings[i] = 1.0;
      for(int j = 0; j < (int) selectedIndices.size(); ++j)
      {
        if(thePerceptValidityChecker.pointIsProbablyInGoalNet(s.toPose(), 
          selectedIndicesPositions[j].first, selectedIndicesPositions[j].second))
        {
          weightings[i] = -1;
          result = PARTIAL_SENSOR_UPDATE;
          ++i;
          if(i<samples.size())
            goto NEXT_SAMPLE;
          else 
            goto FINISHED;
        }
        const Vector2<int>& pp = selectedIndices[j] & 1 ? lines[selectedIndices[j] >> 1]->first 
                                                      : lines[selectedIndices[j] >> 1]->last,
                           pObs(origin.x + ((pp.x * s.rotation.x - pp.y * s.rotation.y) >> 10),
                                origin.y + ((pp.x * s.rotation.y + pp.y * s.rotation.x) >> 10)),
                         & other = selectedIndices[j] & 1 ? lines[selectedIndices[j] >> 1]->last 
                                                          : lines[selectedIndices[j] >> 1]->first,
                           pOther(origin.x + ((other.x * s.rotation.x - other.y * s.rotation.y) >> 10),
                                  origin.y + ((other.x * s.rotation.y + other.y * s.rotation.x) >> 10)),
                           pModel = fieldModel.getClosestLinePoint(pObs, pOther, sqr(pp - other)),
                           diffOnField = pModel - pObs,
                           diffFromCamera((diffOnField.x * camRotation.x + diffOnField.y * camRotation.y) >> 10,
                                          (diffOnField.y * camRotation.x - diffOnField.x * camRotation.y) >> 10),
                           ppWithoutOffset(pp.x - xOrigin, pp.y - yOrigin),
                           ppFromCamera((ppWithoutOffset.x * rotCos + ppWithoutOffset.y * rotSin) >> 10,
                                        (ppWithoutOffset.y * rotCos - ppWithoutOffset.x * rotSin) >> 10);
        weightings[i] *= gaussian.value(
          std::abs(1024 * diffFromCamera.x / (std::abs(ppFromCamera.x) + 450)),
          theSelfLocatorParameter.standardDeviationFieldLines);
        weightings[i] *= gaussian.value(
          ppFromCamera.x != 0 ? std::abs(1024 * diffFromCamera.y / ppFromCamera.x) : 0,
          theSelfLocatorParameter.standardDeviationFieldLines);
      }
    }
FINISHED:
    return result;
  }
};


#endif

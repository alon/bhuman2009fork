/**
* @file CornersSensorModel.h
*
* Sensor model for updating samples by perceived corners
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef _CornersSensorModel_h_
#define _CornersSensorModel_h_

#include "FieldModel.h"

/**
* @class CornersSensorModel
*/
class CornersSensorModel: public SensorModel
{
private:
  class SelectedIndex
  {
  public:
    short distance; /**< Distance to corner in mm. */
    double bearing; /**< Bearing to corner in radians. */
    double direction; /**< Relative direction of corner in radians. */

    SelectedIndex() {}
    SelectedIndex(short distance, double bearing, double direction) : 
      distance(distance), bearing(bearing), direction(direction) {}
  };

  const LinePercept& theLinePercept; /**< Reference to points percept */
  std::vector<short> selectedIndices;    /**< The indices of the points in the points percept that were selected for the sensor update. */
  std::vector<SelectedIndex> selectedIndicesPositions; /**< Precomputed information (distance+angle) about selected points */
  GaussianTable gaussian; /**< A table of Gaussians for assessing bearing measurements to corners. */
  const FieldModel& fieldModel; /**< The model of proximity to features on the field. */

public:
  /** Constructor. */
  CornersSensorModel(const SelfLocatorParameter& selfLocatorParameter,
    const LinePercept& linePercept, const FrameInfo& frameInfo, 
    const FieldDimensions& fieldDimensions, const CameraMatrix& cameraMatrix, 
    const PerceptValidityChecker& perceptValidityChecker,
    const FieldModel& fieldModel):
  SensorModel(selfLocatorParameter, frameInfo, fieldDimensions, cameraMatrix, 
              perceptValidityChecker, Observation::CORNER), 
    theLinePercept(linePercept), fieldModel(fieldModel)
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
    gaussian.computeTable(1000, 200, theSelfLocatorParameter.standardDeviationCorners, theSelfLocatorParameter.standardDeviationCorners, 1);
    selectedIndicesPositions.resize(selectedIndices.size());
    for(int j = 0; j < (int) selectedIndices.size(); ++j)
    {
      const short distance = static_cast<short>(theLinePercept.intersections[selectedIndices[j]].pos.abs());
      const double bearing = theLinePercept.intersections[selectedIndices[j]].pos.angle();
      const double direction = theLinePercept.intersections[selectedIndices[j]].type == LinePercept::Intersection::X
                               ? 0
                               : theLinePercept.intersections[selectedIndices[j]].type == LinePercept::Intersection::T
                                 ? theLinePercept.intersections[selectedIndices[j]].dir1.angle()
                                 : (theLinePercept.intersections[selectedIndices[j]].dir1 +
                                    theLinePercept.intersections[selectedIndices[j]].dir2).angle() - pi_4;
      selectedIndicesPositions[j] = SelectedIndex(distance, bearing, direction);
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
          selectedIndicesPositions[j].distance, selectedIndicesPositions[j].bearing))
        {
          weightings[i] = -1;
          result = PARTIAL_SENSOR_UPDATE;
          ++i;
          if(i<samples.size())
            goto NEXT_SAMPLE;
          else 
            goto FINISHED;
        }
        const LinePercept::Intersection& pp = theLinePercept.intersections[selectedIndices[j]];
        const Vector2<int> pObs(origin.x + ((pp.pos.x * s.rotation.x - pp.pos.y * s.rotation.y) >> 10),
                                origin.y + ((pp.pos.x * s.rotation.y + pp.pos.y * s.rotation.x) >> 10)),
                           pModel = fieldModel.getClosestCorner(pObs, pp.type, s.angle + selectedIndicesPositions[j].direction),
                           diffOnField = pModel - pObs,
                           diffFromCamera((diffOnField.x * camRotation.x + diffOnField.y * camRotation.y) >> 10,
                                          (diffOnField.y * camRotation.x - diffOnField.x * camRotation.y) >> 10),
                           ppWithoutOffset(pp.pos.x - xOrigin, pp.pos.y - yOrigin),
                           ppFromCamera((ppWithoutOffset.x * rotCos + ppWithoutOffset.y * rotSin) >> 10,
                                        (ppWithoutOffset.y * rotCos - ppWithoutOffset.x * rotSin) >> 10);
        weightings[i] *= gaussian.value(
          std::abs(1024 * diffFromCamera.x / (std::abs(ppFromCamera.x) + 450)),
          theSelfLocatorParameter.standardDeviationCorners);
        weightings[i] *= gaussian.value(
          ppFromCamera.x != 0 ? std::abs(1024 * diffFromCamera.y / ppFromCamera.x) : 0,
          theSelfLocatorParameter.standardDeviationCorners);
      }
    }
FINISHED:
    return result;
  }
};


#endif

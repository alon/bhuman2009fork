/**
* @file BoardInfo.h
* The file declares a class that represents information about the connection to
* all boards of the robot.
* @author <a href="mailto:Thomas.Röfer@dfki.de">Thomas Röfer</a>
*/ 

#ifndef __BoardInfo_h_
#define __BoardInfo_h_

#include "Tools/Streams/Streamable.h"


/**
* @class BoardInfo
* A class that represents information about the connection to
* all boards of the robot.
*/
class BoardInfo : public Streamable
{
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read (if in != 0).
  * @param out The stream to which the object is written (if out != 0).
  */
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM_ARRAY(ack);
    STREAM_ARRAY(nack);
    STREAM_ARRAY(error);
    STREAM_REGISTER_FINISH();
  }

public:
  enum Board
  {
    chestBoard,
    battery,
    usBoard,
    inertialSensor,
    headBoard,
    earLeds,
    faceBoard,
    leftShoulderBoard,
    leftArmBoard,
    rightShoulderBoard,
    rightArmBoard,
    leftHipBoard,
    leftThighBoard,
    leftShinBoard,
    leftFootBoard,
    rightHipBoard,
    rightThighBoard,
    rightShinBoard,
    rightFootBoard,
    numOfBoards
  };

  int ack[numOfBoards], /**< The number of times a package has been successfully received for each board. */
      nack[numOfBoards], /**< The number of times an error occured during package reception for each board. */
      error[numOfBoards]; /**< The current error number for each board. */

  /** Default constructor */
  BoardInfo() 
  {
    for(int i = 0; i < numOfBoards; ++i)
      ack[i] = nack[i] = error[i] = 0;
  }

  /**
  * The function returns the name of a board.
  * @param board The board the name of which will be returned.
  * @return The name.
  */
  static const char* getBoardName(Board board)
  {
    switch(board)
    {
    case chestBoard: return "chestBoard";
    case battery: return "battery";
    case usBoard: return "usBoard";
    case inertialSensor: return "inertialSensor";
    case headBoard: return "headBoard";
    case earLeds: return "earLeds";
    case faceBoard: return "faceBoard";
    case leftShoulderBoard: return "leftShoulderBoard";
    case leftArmBoard: return "leftArmBoard";
    case rightShoulderBoard: return "rightShoulderBoard";
    case rightArmBoard: return "rightArmBoard";
    case leftHipBoard: return "leftHipBoard";
    case leftThighBoard: return "leftThighBoard";
    case leftShinBoard: return "leftShinBoard";
    case leftFootBoard: return "leftFootBoard";
    case rightHipBoard: return "rightHipBoard";
    case rightThighBoard: return "rightThighBoard";
    case rightShinBoard: return "rightShinBoard";
    case rightFootBoard: return "rightFootBoard";
    default: return "unknown";
    }
  }
};


#endif //__BoardInfo_h_

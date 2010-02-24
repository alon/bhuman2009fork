/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef ALVIDEO_H
# define ALVIDEO_H
# include "altypes.h"
# include "alvisiondefinitions.h"
# include "alimage.h"
# include <iostream>
# include <fstream>
# include <vector>
//# include "altools.h"
//#include "alvalue.h"
//#include "altimeval.h"


//#define ALVISION_VIDEO_DEBUG(...){ printf(__VA_ARGS__); }
#define ALVISION_VIDEO_DEBUG(...)

//#define ALVISION_VIDEO_IN_AND_OUT(...) { printf(__VA_ARGS__); }
#define ALVISION_VIDEO_IN_AND_OUT(...)


/******************************************************
 video format is as follow
 
 
 
 
 
 
 
 
 *******************************************************/


namespace AL
{
  //structures for video raw stream recording
 /*
  * streamHeader
  * @brief single stream header
  */
  struct streamHeader
  {
    unsigned int width : 32;
    unsigned int height: 32;
    unsigned int colorSpace : 32;
    unsigned int pixelDepth : 32;
  };
    
 /*
  * videoHeader
  * @brief multiple stream header global header with version field
  */   
  struct videoHeader
  {
    unsigned int magicNumber : 32;  // magic number
    unsigned int userSpaceSize : 32;    
    unsigned int numberOfFrames : 32;
    unsigned int numberOfStreams : 32;
    //the different streams headers will be writen here
    //and finally the free space for storing user data
  };
    
    
    
  struct streamProperties
  {
    unsigned int width;
    unsigned int height;
    unsigned int resolution;
    unsigned int colorSpace;
    unsigned int nbLayers;
    unsigned int pixelDepth;
    unsigned int sizePerImage;
  };   
    
    
  class ALVideo
  {
    public :

    private:
      volatile bool fIsOpenForReading;
      volatile bool fIsOpenForWriting;

      std::ifstream     fVideoStreamIn;
      std::ofstream     fVideoStreamOut;
      std::string       fFilePath;
      
      unsigned int fMagicNumber;
      unsigned int fUserSpaceSize;
      unsigned int fNumberOfFrames;
      unsigned int fNumberOfStreams;
      unsigned int fSizeOfHeader;
      unsigned int fSizePerMultiplexedFrame;
      std::vector<struct streamProperties> fStreamPropertiesVector;

      unsigned int fCurrentStreamNumber;
      unsigned int fCurrentFrameNumber;

    public:
      ALVideo();
      ~ALVideo();
      
      bool recordVideo(const std::string pFilePath, const unsigned int pUserSpaceSize, const std::vector<streamHeader>& pStreamHeaderVector);
      bool readVideo(const std::string pFilePath);
      void closeVideo();
      bool goToFrame(const unsigned int pFrameNumber, const unsigned int pStreamNumber);
      bool getFrame(ALImage & pImage, const unsigned int pFrameNumber, const unsigned int pStreamNumber, const bool pCheckFormat = true);
      bool getNextFrame(ALImage & pImage, const bool pCheckFormat = true);
      bool getPrecedingFrame(ALImage & pImage, const bool pCheckFormat = true);
      inline bool write(char* ptrImageData, const int pSizeData, const long long pTimeStamp = 0);
      //bool setFrame(ALImage & pImage);
      
      inline const unsigned char* getMagicNumber() const       { return (unsigned char*)&fMagicNumber; };
      inline const unsigned int getNumberOfFrames() const      { return fNumberOfFrames; };
      inline const unsigned int getNumberOfStreams() const     { return fNumberOfStreams; };
      inline const unsigned int getCurrentFrameNumber() const  { return fCurrentFrameNumber; };
      inline const unsigned int getCurrentStreamNumber() const { return fCurrentStreamNumber; };
      inline const bool isOpenForReading() const               { return fIsOpenForReading; };
      inline const bool isOpenForWriting() const               { return fIsOpenForWriting; }; 
      
      inline const int getStreamWidth(const unsigned int pStreamNumber) const       { return (pStreamNumber<fNumberOfStreams) ? fStreamPropertiesVector[pStreamNumber].width        : -1; };
      inline const int getStreamHeight(const unsigned int pStreamNumber)const       { return (pStreamNumber<fNumberOfStreams) ? fStreamPropertiesVector[pStreamNumber].height       : -1; };
      inline const int getStreamResolution(const unsigned int pStreamNumber) const  { return (pStreamNumber<fNumberOfStreams) ? fStreamPropertiesVector[pStreamNumber].resolution   : -1; };
      inline const int getStreamColorSpace(const unsigned int pStreamNumber) const  { return (pStreamNumber<fNumberOfStreams) ? fStreamPropertiesVector[pStreamNumber].colorSpace   : -1; };
      inline const int getStreamNbLayers(const unsigned int pStreamNumber) const    { return (pStreamNumber<fNumberOfStreams) ? fStreamPropertiesVector[pStreamNumber].nbLayers     : -1; };
      inline const int getStreamPixelDepth(const unsigned int pStreamNumber) const  { return (pStreamNumber<fNumberOfStreams) ? fStreamPropertiesVector[pStreamNumber].pixelDepth   : -1; };
      inline const int getStreamSizePerImage(const unsigned int pStreamNumber)const { return (pStreamNumber<fNumberOfStreams) ? fStreamPropertiesVector[pStreamNumber].sizePerImage : -1; };
      
  };


  bool ALVideo::write(char* ptrImageData, const int pSizeData, const long long pTimeStamp) 
  {
    ALVISION_VIDEO_IN_AND_OUT("ALVideo::write - in\n");

    if(fIsOpenForWriting)
    {  
      fVideoStreamOut.write((char*)&pTimeStamp, 8);	
      fVideoStreamOut.write(ptrImageData+8, pSizeData-8);
  	  fNumberOfFrames++;
  	
      ALVISION_VIDEO_IN_AND_OUT("ALVideo::write - out\n");  	
  	  return true;
    }
    else
      return false;
  }
  
}

#endif // ALVIDEO_H

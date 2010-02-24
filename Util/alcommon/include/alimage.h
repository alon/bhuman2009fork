/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef ALVISIONIMAGE_H
# define ALVISIONIMAGE_H
# include "altypes.h"
# include "alvisiondefinitions.h"
# include "altools.h"
#include "alvalue.h"
#include "altimeval.h"

#define ALVisionImage ALImage

namespace AL
{
  class ALImage
  {
    // TODO default param depth = 8 and allocation of int8, int16, int32 []
    public :
      /**
      *fWitdh
      *@brief width of the image
      */
      int fWidth;
      
      /**
      *fHeight
      *@brief height of the image
      */
      int fHeight;
  
      /**
      *fWitdh
      *@brief number of layers of the image
      */
      int fNbLayers;
  
      /**
      *fColorSpace
      *@briefcolor space of the image
      */
      int fColorSpace;
          
      /**
      *fUnixTimeStamp
      *@brief Time in microsecond when the image was captured
      */
      long long fTimeStamp;

	private:
      /**
      *fData 
      *@brief pointer to the image data
      */
      unsigned char* fData;
      
      /**
      *fAllocatedSize
      *@brief Size of memory allocation
      */
      int fAllocatedSize;
  
      /**
      *fMaxResolution
      *@brief Maximum resolution accepted in the reserved memory
      */
      int fMaxResolution;
      
      /**
      *fNumberOfLayerMax
      *@brief Maximum number of layers accepted in the reserved memory
      */
      int fNumberOfLayerMax;
      
      /**
      *fIsExternalData
      *@brief If true, image points to an external data buffer. If false, data buffer was allocated while creating instance. 
      */ 
      bool fIsExternalData;


    public:
      /**
      *ALImage
      *@brief constructor
      *@param[0] : [int] width of the image
      *@param[1] : [int] height of the image
      *@param[2] : [int] color space of the image
      *@param[3] : [bool] is data buffer external, or is it allocated with the image
      */
      ALImage( int pWidth, int pHeight, int pColorSpace, bool pIsExternalData = false );
  
      /**
      *ALImage
      *@brief constructor
      *@param[0] : [int] resolution of the image
      *@param[1] : [int] color space of the image
      *@param[2] : [bool] is data buffer external, or is it allocated with the image
      */
      ALImage( int pResolution, int pColorSpace, bool pIsExternalData = false );
  
      /**
      *~ALImage 
      *@brief destructor
      */
      ~ALImage();
    
      /**
      *toALValue
      *@brief return an ALValue containing image structure
      *@result[0] : [int] with of the image
      *@result[1] : [int] height of the image
      *@result[2] : [int] number of layers of the image
      *@result[3] : [int] colorspace of the image
      *@result[4] : [int] time stamp in second
      *@result[5] : [int] time stamp in microsecond (and under second)
      *@result[6] : [int] data of the image
      */
      ALValue toALValue();
      
      /**
      *setSize //DEPRECATED
      *@brief set the Size of the image without changing the allocation size.
      *@param[0] : [int] resolution of the image
      */
      bool setSize(int pResolution) { return setResolution(pResolution); }
      /**
      *setResolution
      *@brief set the Size of the image without changing the allocation size.
      *@param[0] : [int] resolution of the image
      */
      bool setResolution(int pResolution);
  
      /**
      *setColorSpace
      *@brief set the ColorSpace of the image without changing the allocation size.
      *@param[0] : [int] ColorSpace of the image
      */
      bool setColorSpace(int pColorSpace);
  
      /**
      *getFrame //DEPRECATED
      *@brief return the reference to the image data
      *@result reference to the image data
      */
      inline const unsigned char* getFrame() const { AL_ASSERT( fData != 0); return fData; }
	  /**
      *getData 
      *@brief return the reference to the image data
      *@result reference to the image data
      */
      inline const unsigned char* getData() const { AL_ASSERT( fData != 0); return fData; } 
             
      // for the camera
      /**
      *getFrame //DEPRECATED
      *@brief return the pointer to the image data
      *@result pointer to the image data
      */
      inline unsigned char* getFrame() { AL_ASSERT( fData != 0); return fData; }
      /**
      *getData
      *@brief return the pointer to the image data
      *@result pointer to the image data
      */
      inline unsigned char* getData() { AL_ASSERT( fData != 0); return fData; }
            
      /**
      *setFrame //DEPRECATED
      *@brief set the image data pointer to point to the specified buffer
      *@param[0] : [uInt8*] pointer to the image data buffer
      */
	  inline void setFrame(unsigned char* pData) { fData = pData; }
      /**
      *setData
      *@brief set the image data pointer to point to the specified buffer
      *@param[0] : [uInt8*] pointer to the image data buffer
      */
	  inline void setData(unsigned char* pData) { fData = pData; }
	  
	  /**
      *setTimeStamp
      *@brief set the image timestamp
      *@param[0] : [struct timeval] time in a struct timeval format
      */      
      inline void setTimeStamp(const struct timeval pTimeStamp) 
      {
      	fTimeStamp = ( (pTimeStamp.tv_usec < 0) || (pTimeStamp.tv_sec < 0) ) ? -1 : ( (((long long)pTimeStamp.tv_sec)*1000000) + (long long)((int)pTimeStamp.tv_usec) );  // even if tv_usec is on 64 bits, it never needs more than 32 bits 
      }
      
      /**
      *setTimeStamp
      *@brief set the image timestamp
      *@param[0] : [long long] time in a long long format
      */
      inline void setTimeStamp(long long pTimeStamp) { fTimeStamp = pTimeStamp; }

    
      /**
      *getSize
      *@return the size of the allocated memory for the image (the size of fData buffer)
      */
      inline unsigned int getSize() const { return fWidth*fHeight*fNbLayers; }
  
      /*
      * Accessor
      */
      inline int getWidth( void )  const { return fWidth; }
      inline int getHeight( void ) const { return fHeight; }
      inline int getResolution( void ) const { return (fWidth==640) ? kVGA : (fWidth==320)?kQVGA:kQQVGA; }
      inline int getColorSpace( void ) const { return fColorSpace; }
      inline int getNbLayers( void ) const { return fNbLayers; }
      inline long long getTimeStamp( void ) const { return fTimeStamp; }
      inline int getMaxResolution( void ) const { return fMaxResolution; }
      inline int getNbOfLayersMax( void ) const { return fNumberOfLayerMax; }
      inline bool areDataExternal( void ) const { return fIsExternalData; }            
  
      /*
      * For debug purpose: print the object in an human format
      */
      std::string toString( void ) const;
      
      
    /*  inline ALImage& operator=(const ALImage& source)
      {
      	fWidth          = source.fWidth;
        fHeight         = source.fHeight;
        fNbLayers       = source.fNbLayers;
        fColorSpace     = source.fColorSpace;
        fTimeStamp      = source.fTimeStamp;
        fAllocatedSize  = source.fAllocatedSize;
        maxResolution       = source.maxResolution;
        fIsExternalData = false;
        memcpy(fData, source.fData, fAllocatedSize);
      }*/    
      
    private:
      inline bool reallocateDataSize(const int resolution, const int nbLayers);

  };
}

// take a pixel in YUV and compute its RGB Value. RGB value are returned directly in params
void computeRgbFromYuv( unsigned char * pYR, unsigned char * pUG, unsigned char * pVB );


#endif // ALVISIONIMAGE_H

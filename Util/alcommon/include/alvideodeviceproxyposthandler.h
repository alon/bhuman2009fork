/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALVIDEODEVICEPROXYPOSTHANDLER_H_
# define ALVIDEODEVICEPROXYPOSTHANDLER_H_
# include "alproxy.h"
namespace AL
{
  class ALVideoDeviceProxy;

  /**
   * ALVideoDeviceProxyPostHandler gives acces to all bound methods of the module, using a thread around each call.
   * Long running tasks can be stopped by calling stop(id) with the id returned by the post. 
   */
  class ALVideoDeviceProxyPostHandler
    {
       friend class ALVideoDeviceProxy;

       protected:
          AL::ALProxy* fParent;

       private:
          void setParent(AL::ALProxy* pParent) { fParent = pParent;}

       public:

       ALVideoDeviceProxyPostHandler() : fParent(NULL)
       {}

       /**
        * \brief dataChanged : Called by ALMemory when subscribed data has been modified.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int dataChanged ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "dataChanged", dataName, value, message );
       }

       /**
        * \brief exit : Exit and unregister module, with no params and no return value.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int exit (  )
       {
         return fParent->pCall( "exit" );
       }

       /**
        * \brief getAngPosFromImgPos : Returns angles relative to camera axis given a normalized position in the image.
        * \param pImgPos normalized position in the image [0.0 - 1.0]
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getAngPosFromImgPos ( std::vector<float>  pImgPos )
       {
         return fParent->pCall( "getAngPosFromImgPos", pImgPos );
       }

       /**
        * \brief getAngSizeFromImgSize : Returns angles relative to camera axis given a normalized position in the image.
        * \param pImgSize normalized position in the image [0.0 - 1.0]
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getAngSizeFromImgSize ( std::vector<float>  pImgSize )
       {
         return fParent->pCall( "getAngSizeFromImgSize", pImgSize );
       }

       /**
        * \brief getBrokerName : return the broker name
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getBrokerName (  )
       {
         return fParent->pCall( "getBrokerName" );
       }

       /**
        * \brief getDirectRawImageLocal : Returns a pointer to a locked ALImage, with data array pointing directly to raw data. No format conversion and no copy of the raw buffer.
  When image is not necessary anymore, a call to releaseDirectRawImage() is requested.
  Warning: 1. when using this mode for several G.V.M., if they need raw data for more than 25ms check that you have strictly less G.V.M. modules in this mode than the amount of kernel buffers!! 2. Release all kernel buffers before any action requesting a modification in camera running mode (e.g. resolution, switch between cameras). 
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getDirectRawImageLocal ( std::string pId )
       {
         return fParent->pCall( "getDirectRawImageLocal", pId );
       }

       /**
        * \brief getDirectRawImageRemote : Fills pFrameOut with data coming directly from raw buffer (no format conversion).
  No need to call releaseDirectRawImage() because it is done automatically at the end of data transmission.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getDirectRawImageRemote ( std::string pId )
       {
         return fParent->pCall( "getDirectRawImageRemote", pId );
       }

       /**
        * \brief getGVMColorSpace : Get the color space of the requested G.V.M.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getGVMColorSpace ( std::string pId )
       {
         return fParent->pCall( "getGVMColorSpace", pId );
       }

       /**
        * \brief getGVMFrameRate : Get the frame rate of the requested G.V.M.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getGVMFrameRate ( std::string pId )
       {
         return fParent->pCall( "getGVMFrameRate", pId );
       }

       /**
        * \brief getGVMResolution : Get the resolution of the requested G.V.M.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getGVMResolution ( std::string pId )
       {
         return fParent->pCall( "getGVMResolution", pId );
       }

       /**
        * \brief getImageLocal : Applies transformations to the last image from video source and returns a pointer to a locked ALImage.
  When image is not necessary anymore, a call to releaseImage() is requested.
  If the G.V.M. didn't release preceding image, returns NULL.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getImageLocal ( std::string pId )
       {
         return fParent->pCall( "getImageLocal", pId );
       }

       /**
        * \brief getImageRemote : Applies transformations to the last image from video source and fills pFrameOut.
  No need to call releaseImage() because it is done automatically at the end of data transmission.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getImageRemote ( std::string pId )
       {
         return fParent->pCall( "getImageRemote", pId );
       }

       /**
        * \brief getImgInfoFromAngInfo : Returns normalized image info from angles info in radians (as returned by vision extractors).
        * \param pAngles camera angle values in radians.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getImgInfoFromAngInfo ( std::vector<float>  pAngles )
       {
         return fParent->pCall( "getImgInfoFromAngInfo", pAngles );
       }

       /**
        * \brief getImgInfoFromAngInfoWithRes : Returns image info from angles info in radians (as returned by vision extractors). Express image info in the requested resolution.
        * \param pAngInfo camera angle values in radians.
        * \param arg2 arg
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getImgInfoFromAngInfoWithRes ( std::vector<float>  pAngInfo, int arg2 )
       {
         return fParent->pCall( "getImgInfoFromAngInfoWithRes", pAngInfo, arg2 );
       }

       /**
        * \brief getImgPosFromAngPos : Returns normalized image position from camera angles in radians.
        * \param pAngPos camera angle values in radians.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getImgPosFromAngPos ( std::vector<float>  pAngPos )
       {
         return fParent->pCall( "getImgPosFromAngPos", pAngPos );
       }

       /**
        * \brief getImgSizeFromAngSize : Returns normalized image position from camera angles in radians.
        * \param pAngSize camera angle values in radians.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getImgSizeFromAngSize ( std::vector<float>  pAngSize )
       {
         return fParent->pCall( "getImgSizeFromAngSize", pAngSize );
       }

       /**
        * \brief getMethodHelp : Retrieve a method's description
        * \param methodName the name of the method
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getMethodHelp ( std::string methodName )
       {
         return fParent->pCall( "getMethodHelp", methodName );
       }

       /**
        * \brief getMethodList : Retrieve the module's method list.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getMethodList (  )
       {
         return fParent->pCall( "getMethodList" );
       }

       /**
        * \brief getParam : Get the value of a video source specific parameter.
        * \param pParam Parameter's reference among kCameraBrightnessID,
 kCameraContrastID,
 kCameraSaturationID,
 kCameraHueID,
 kCameraRedChromaID,
 kCameraBlueChromaID,
 kCameraGainID,
 kCameraHFlipID,
 kCameraVFlipID,
 kCameraLensXID,
 kCameraLensYID,
 kCameraAutoExpositionID,
 kCameraAecAlgorithmID [0:average-based; 1:histogram-based],
 kCameraExposureCorrectionID [-6; 6] (correction by n/3 IL, switch automatically in average-based AEC algorithm if n!=0 and back to histogram-based for n=0),
 kCameraExposureID,
 kCameraAutoWhiteBalanceID,
 kCameraAutoGainID,
 kCameraSelectID [0:top camera; 1:bottom camera],
 kCameraResolutionID,
 kCameraFrameRateID,
 kCameraBufferSizeID,
 kCameraFastSwitchID (need both camera to run same format/ parameter value has no signification),
 kCameraSharpnessID (0-1: no sharpness - 2: medium sharpness - 3-5: high sharpness) 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getParam ( int pParam )
       {
         return fParent->pCall( "getParam", pParam );
       }

       /**
        * \brief getVIMColorSpace : Get the color space of the video source image.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getVIMColorSpace (  )
       {
         return fParent->pCall( "getVIMColorSpace" );
       }

       /**
        * \brief getVIMFrameRate : Get the frame rate of the video source image.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getVIMFrameRate (  )
       {
         return fParent->pCall( "getVIMFrameRate" );
       }

       /**
        * \brief getVIMResolution : Get the resolution of the video source image.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getVIMResolution (  )
       {
         return fParent->pCall( "getVIMResolution" );
       }

       /**
        * \brief innerTest : Run an internal test to this module.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int innerTest (  )
       {
         return fParent->pCall( "innerTest" );
       }

       /**
        * \brief isFrameGrabberOff : Ask if the framegrabber is off.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int isFrameGrabberOff (  )
       {
         return fParent->pCall( "isFrameGrabberOff" );
       }

       /**
        * \brief isRunning : return true if method is currently running
        * \param ID ID of the method to wait
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int isRunning ( int ID )
       {
         return fParent->pCall( "isRunning", ID );
       }

       /**
        * \brief moduleHelp : Retrieve the module's description
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int moduleHelp (  )
       {
         return fParent->pCall( "moduleHelp" );
       }

       /**
        * \brief ping : Just a ping, with no params and returning always true
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int ping (  )
       {
         return fParent->pCall( "ping" );
       }

       /**
        * \brief recordVideo : BETA - background record of an .arv raw format video from the images processed by a G.V.M.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \param pPath path/name of the video to be recorded
        * \param pTotalNumber number of images to be recorded. 0xFFFFFFFF for "unlimited"
        * \param pPeriod one image recorded every pPeriod images
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int recordVideo ( std::string pId, std::string pPath, int pTotalNumber, int pPeriod )
       {
         return fParent->pCall( "recordVideo", pId, pPath, pTotalNumber, pPeriod );
       }

       /**
        * \brief releaseDirectRawImage : Release image buffer locked by getDirectRawImageLocal().
  If G.V.M. had no locked image buffer, does nothing.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int releaseDirectRawImage ( std::string pId )
       {
         return fParent->pCall( "releaseDirectRawImage", pId );
       }

       /**
        * \brief releaseImage : Release image buffer locked by getImageLocal().
  If G.V.M. had no locked image buffer, does nothing.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int releaseImage ( std::string pId )
       {
         return fParent->pCall( "releaseImage", pId );
       }

       /**
        * \brief resolutionToSizes : return the width and the height of a resolution
        * \param resolution  { 0 = kQQVGA, 1 = kQVGA, 2 = kVGA } 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int resolutionToSizes ( int resolution )
       {
         return fParent->pCall( "resolutionToSizes", resolution );
       }

       /**
        * \brief setColorSpace : Set the colorspace of the output image.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \param pColorSpace  { 0 = kYuv, 9 = kYUV422, 10 = kYUV, 11 = kRGB, 12 = kHSY, 13 = kBGR } 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setColorSpace ( std::string pId, int pColorSpace )
       {
         return fParent->pCall( "setColorSpace", pId, pColorSpace );
       }

       /**
        * \brief setFrameRate : Set the required frame rate.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \param frameRate images per seconds { 5, 10, 15, 30 } 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setFrameRate ( std::string pId, int frameRate )
       {
         return fParent->pCall( "setFrameRate", pId, frameRate );
       }

       /**
        * \brief setParam : Sets the value of a specific parameter for the video source.
        * \param pParam Parameter's reference among kCameraBrightnessID,
 kCameraContrastID,
 kCameraSaturationID,
 kCameraHueID,
 kCameraRedChromaID,
 kCameraBlueChromaID,
 kCameraGainID,
 kCameraHFlipID,
 kCameraVFlipID,
 kCameraLensXID,
 kCameraLensYID,
 kCameraAutoExpositionID,
 kCameraAecAlgorithmID [0:average-based; 1:histogram-based],
 kCameraExposureCorrectionID [-6; 6] (correction by n/3 IL, switch automatically in average-based AEC algorithm if n!=0 and back to histogram-based for n=0),
 kCameraExposureID,
 kCameraAutoWhiteBalanceID,
 kCameraAutoGainID,
 kCameraSelectID [0:top camera; 1:bottom camera],
 kCameraResolutionID,
 kCameraFrameRateID,
 kCameraBufferSizeID,
 kCameraFastSwitchID (need both camera to run same format/ parameter value has no signification),
 kCameraSharpnessID (0-1: no sharpness - 2: medium sharpness - 3-5: high sharpness) 
        * \param pNewValue Parameter's new value.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setParam ( int pParam, int pNewValue )
       {
         return fParent->pCall( "setParam", pParam, pNewValue );
       }

       /**
        * \brief setParamDefault : Sets a specific parameter for the video source at its default value.
        * \param pParam Parameter's reference. 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setParamDefault ( int pParam )
       {
         return fParent->pCall( "setParamDefault", pParam );
       }

       /**
        * \brief setResolution : Set the size of the output image.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \param pSize  { 0 = kQQVGA, 1 = kQVGA, 2 = kVGA } 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setResolution ( std::string pId, int pSize )
       {
         return fParent->pCall( "setResolution", pId, pSize );
       }

       /**
        * \brief sizesToResolution : return the resolution from sizes
        * \param width width of the image
        * \param height height of the image
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int sizesToResolution ( int width, int height )
       {
         return fParent->pCall( "sizesToResolution", width, height );
       }

       /**
        * \brief startFrameGrabber : Open and initialize video source device.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int startFrameGrabber (  )
       {
         return fParent->pCall( "startFrameGrabber" );
       }

       /**
        * \brief stopFrameGrabber : Close video source device.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int stopFrameGrabber (  )
       {
         return fParent->pCall( "stopFrameGrabber" );
       }

       /**
        * \brief stopVideo : BETA - stop writing the video sequence
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int stopVideo ( std::string pId )
       {
         return fParent->pCall( "stopVideo", pId );
       }

       /**
        * \brief subscribe : Register to the Video Input Module(V.I.M.). When a General Video Module(G.V.M.) registers to the V.I.M., a buffer of the requested image format is added to the buffers list.
  Returns the name under which the G.V.M. is registered to the V.I.M. (usefull when two G.V.M. try to register using the same name
        * \param pGVMname Name of the registering G.V.M.
        * \param pResolution Resolution requested. { 0 = kQQVGA, 1 = kQVGA, 2 = kVGA } 
        * \param pColorSpace Colorspace requested. { 0 = kYuv, 9 = kYUV422, 10 = kYUV, 11 = kRGB, 12 = kHSY, 13 = kBGR } 
        * \param pFps Fps (frames per second) requested. { 5, 10, 15, 30 } 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int subscribe ( std::string pGVMname, int pResolution, int pColorSpace, int pFps )
       {
         return fParent->pCall( "subscribe", pGVMname, pResolution, pColorSpace, pFps );
       }

       /**
        * \brief unsubscribe : Used to unregister a G.V.M. from the V.I.M.
        * \param pId Name under which the G.V.M. is known from the V.I.M.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int unsubscribe ( std::string pId )
       {
         return fParent->pCall( "unsubscribe", pId );
       }

       /**
        * \brief version : Returns the revision of the module.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int version (  )
       {
         return fParent->pCall( "version" );
       }

       /**
        * \brief wait : wait end of execution method if method was called with pCall
        * \param ID ID of the method to wait
        * \param timeout timeout of the wait in ms. 0 if no timeout
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int wait ( int ID, int timeout )
       {
         return fParent->pCall( "wait", ID, timeout );
       }

       /**
        * \brief waitMethod : wait end of execution method if method was called with pCall
        * \param name name of the method to wait
        * \param timeout timeout of the wait. 0 if no timeout
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int waitMethod ( std::string name, int timeout )
       {
         return fParent->pCall( "waitMethod", name, timeout );
       }

    };
};

#endif

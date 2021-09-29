// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef _CAP_PYLON_HPP_
#define _CAP_PYLON_HPP_

#ifdef HAVE_PYLON

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#pragma GCC diagnostic pop

namespace cv
{

class VideoCapture_Pylon : public IVideoCapture
{
public:
    VideoCapture_Pylon(int index);
    virtual ~VideoCapture_Pylon();

    virtual double getProperty(int propIdx) const CV_OVERRIDE;
    virtual bool setProperty(int propIdx, double propVal) CV_OVERRIDE;

    virtual bool grabFrame() CV_OVERRIDE;
    virtual bool retrieveFrame(int outputType, OutputArray frame) CV_OVERRIDE;
    virtual int getCaptureDomain() CV_OVERRIDE;
    virtual bool isOpened() const CV_OVERRIDE;
    
    // TODO: make this configurable via setProperty
    const Pylon::EGrabStrategy grabStrategy = Pylon::GrabStrategy_LatestImageOnly;
protected:
    Pylon::PylonAutoInitTerm autoInitTerm;
    Pylon::CBaslerUniversalInstantCamera * camera;
    Pylon::CGrabResultPtr ptrGrabResult;

    // TODO: make this configurable via setProperty
    const int grab_timeout_ms = 1000;
};

}

#endif
#endif

// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include "precomp.hpp"

#ifdef HAVE_PYLON
#include "cap_pylon.hpp"

#include <iostream>

namespace cv
{

VideoCapture_Pylon::VideoCapture_Pylon(int idx)
{
    if (idx != 0) {
        throw std::invalid_argument("Can only open the first Pylon device. Enumeration not implemented.");
    }
    camera = new Pylon::CBaslerUniversalInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
    camera->Open();
    camera->StartGrabbing(grabStrategy);
}

VideoCapture_Pylon::~VideoCapture_Pylon() {
    delete camera;
}

double VideoCapture_Pylon::getProperty(int propIdx) const
{
    double propValue = 0.0;
    switch(propIdx)
    {
    case CAP_PROP_FRAME_WIDTH:
        //propValue = static_cast<double>(profile.width());
        break;
    case CAP_PROP_FRAME_HEIGHT:
        //propValue = static_cast<double>(profile.height());
        break;
    case CAP_PROP_FPS:
        //propValue = static_cast<double>(profile.fps());
        break;
    }
    return propValue;
}

bool VideoCapture_Pylon::setProperty(int propIdx, double value)
{
    bool isSet = false;
    switch(propIdx)
    {
    case CAP_PROP_SETTINGS:
        /* Hidden feature accessible by type-munging:
         * @code
         * const char * pfsptr = "…";
         * double value;
         * std::memcpy(&value, &pfsptr, sizeof(value)); // HERETIC INSANITY!
         * videoCapture.set(cv::CAP_PROP_SETTINGS, value); 
         * @endcode
         */
        char * pfsptr = nullptr;
        static_assert(sizeof(pfsptr) == sizeof(value), "char * has not the same size as double");
        std::memcpy(&pfsptr, &value, sizeof(value)); // please, for the love of god, never do this
        camera->StopGrabbing();
        Pylon::CFeaturePersistence::LoadFromString(pfsptr, &camera->GetNodeMap(), false);
        camera->StartGrabbing(grabStrategy);
        isSet = true;
        break;
    }
    return isSet;
}

bool VideoCapture_Pylon::grabFrame()
{
    if (camera->IsGrabbing()) {
        camera->RetrieveResult(grab_timeout_ms, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded()) {
            return true;
        } else {
            //throw std::runtime_error(std::string(ptrGrabResult->GetErrorDescription()));
            return false;
        }
    } else {
        //throw std::runtime_error("Camera is not grabbing.");
        return false;
    }
}
bool VideoCapture_Pylon::retrieveFrame(int, cv::OutputArray frame)
{
    if (ptrGrabResult->GetPixelType() != Pylon::EPixelType::PixelType_RGB8packed) {
        //throw std::runtime_error("Current pixel type not supported.");
        return false;
    }
    cv::Mat grabResult(
        ptrGrabResult->GetHeight(),
        ptrGrabResult->GetWidth(),
        CV_8UC3,
        ptrGrabResult->GetBuffer()
    );
    cvtColor(grabResult, frame, COLOR_RGB2BGR);
    return true;
}
int VideoCapture_Pylon::getCaptureDomain()
{
    return CAP_PYLON;
}

bool VideoCapture_Pylon::isOpened() const
{
    return camera->IsOpen();
}

Ptr<IVideoCapture> create_Pylon_capture(int index)
{
    return makePtr<VideoCapture_Pylon>(index);
}

}

#endif

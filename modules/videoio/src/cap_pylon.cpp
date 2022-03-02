// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include "precomp.hpp"

#ifdef HAVE_PYLON
#include "cap_pylon.hpp"

#include <iostream>

namespace cv
{

VideoCapture_Pylon::VideoCapture_Pylon(const std::string & settings)
{
    camera = new Pylon::CBaslerUniversalInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
    camera->Open();
    Pylon::CFeaturePersistence::LoadFromString(settings.c_str(), &camera->GetNodeMap(), false);
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
        propValue = static_cast<double>(camera->Width.GetValueOrDefault(0));
        break;
    case CAP_PROP_FRAME_HEIGHT:
        propValue = static_cast<double>(camera->Height.GetValueOrDefault(0));
        break;
    case CAP_PROP_FPS:
        if (GenApi::IsAvailable(camera->ResultingFrameRate)) {
            propValue = static_cast<double>(camera->ResultingFrameRate());
        }
        if (GenApi::IsAvailable(camera->ResultingFrameRateAbs)) {
            propValue = static_cast<double>(camera->ResultingFrameRateAbs());
        }
        break;
    }
    return propValue;
}

bool VideoCapture_Pylon::grabFrame()
{
    if (camera->IsGrabbing()) {
        camera->RetrieveResult(grab_timeout_ms, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded()) {
            return true;
        } else {
            CV_LOG_WARNING(NULL, "VIDEOIO(PYLON): grabFrame() " << ptrGrabResult->GetErrorDescription());
            return true;
        }
    } else {
        CV_LOG_ERROR(NULL, "VIDEOIO(PYLON): grabFrame() called while camera is not grabbing.");
        return false;
    }
}
bool VideoCapture_Pylon::retrieveFrame(int, cv::OutputArray frame)
{
    if (!ptrGrabResult->GrabSucceeded()) {
        return false;
    }
    if (ptrGrabResult->GetPixelType() != Pylon::EPixelType::PixelType_RGB8packed) {
        CV_LOG_ERROR(NULL, "VIDEOIO(PYLON): Only PixelType_RGB8packed is supported.");
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

Ptr<IVideoCapture> create_Pylon_capture(const std::string & settings)
{
    try {
        return makePtr<VideoCapture_Pylon>(settings);
    } catch (const GenICam::RuntimeException & e) {
        CV_LOG_WARNING(NULL, "VIDEOIO(PYLON): " << e.what());
        return nullptr;
    }
}

}

#endif

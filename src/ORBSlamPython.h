#ifndef ORBSLAMPYTHON_H
#define ORBSLAMPYTHON_H

#include <memory>
#include <Python.h>
#include <boost/python.hpp>
#include <ORB_SLAM2/System.h>
#include <ORB_SLAM2/Tracking.h>
#include <ORB_SLAM2/Sim3Solver.h>

class ORBSlamPython
{
public:
    ORBSlamPython(std::string vocabFile, std::string settingsFile,
        ORB_SLAM2::System::eSensor sensorMode = ORB_SLAM2::System::eSensor::RGBD);
    ORBSlamPython(const char* vocabFile, const char* settingsFile,
        ORB_SLAM2::System::eSensor sensorMode = ORB_SLAM2::System::eSensor::RGBD);
    ~ORBSlamPython();
    
    bool initialize();
    bool isRunning();
    cv::Mat loadAndProcessMono(std::string imageFile, double timestamp);
    cv::Mat processMono(cv::Mat image, double timestamp);
    bool loadAndProcessStereo(std::string leftImageFile, std::string rightImageFile, double timestamp);
    bool processStereo(cv::Mat leftImage, cv::Mat rightImage, double timestamp);
    bool loadAndProcessRGBD(std::string imageFile, std::string depthImageFile, double timestamp);
    bool processRGBD(cv::Mat image, cv::Mat depthImage, double timestamp);
    void reset();
    void shutdown();
    ORB_SLAM2::Tracking::eTrackingState getTrackingState() const;
    unsigned int getNumFeatures() const;
    unsigned int getNumMatches() const;
    boost::python::list getKeyframePoints() const;
    boost::python::list getTrajectoryPoints() const;
    boost::python::list getAllMapPoints() const;
    void correctLoop(long unsigned int loopMnId,
                     long unsigned int curMnId,
                     boost::python::dict corrections,
                     boost::python::dict loopConnections) const;
    boost::python::list computeSim3(boost::python::list p3d1,
                                    boost::python::list p3d2) const;
    ORB_SLAM2::KeyFrame* GetKeyFrameById(long unsigned int query) const;
    void saveMap(std::string path);
    void loadMap(std::string path);
    void activateLocalizationMode();
    void deactivateLocalizationMode();
    bool saveSettings(boost::python::dict settings) const;
    boost::python::dict loadSettings() const;
    void setMode(ORB_SLAM2::System::eSensor mode);
    void setRGBMode(bool rgb);
    void setUseViewer(bool useViewer);
    
    static bool saveSettingsFile(boost::python::dict settings, std::string settingsFilename);
    static boost::python::dict loadSettingsFile(std::string settingsFilename);
    
private:
    std::string vocabluaryFile;
    std::string settingsFile;
    ORB_SLAM2::System::eSensor sensorMode;
    std::shared_ptr<ORB_SLAM2::System> system;
    bool bUseViewer;
    bool bUseRGB;
};



#endif // ORBSLAMPYTHON_H

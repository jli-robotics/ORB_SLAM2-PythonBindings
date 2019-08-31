#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API
#include <opencv2/core/core.hpp>
#include <pyboostcvconverter/pyboostcvconverter.hpp>
#include <ORB_SLAM2/KeyFrame.h>
#include <ORB_SLAM2/Converter.h>
#include <ORB_SLAM2/Tracking.h>
#include <ORB_SLAM2/LoopClosing.h>
#include <ORB_SLAM2/MapPoint.h>
#include <ORB_SLAM2/Osmap.h>
#include "ORBSlamPython.h"

#if (PY_VERSION_HEX >= 0x03000000)
static void* init_ar() {
#else
static void init_ar() {
#endif
    Py_Initialize();

    import_array();
    return NUMPY_IMPORT_ARRAY_RETVAL;
}

BOOST_PYTHON_MODULE(orbslam2)
{
    init_ar();

    boost::python::to_python_converter<cv::Mat, pbcvt::matToNDArrayBoostConverter>();
    pbcvt::matFromNDArrayBoostConverter();

    boost::python::enum_<ORB_SLAM2::Tracking::eTrackingState>("TrackingState")
        .value("SYSTEM_NOT_READY", ORB_SLAM2::Tracking::eTrackingState::SYSTEM_NOT_READY)
        .value("NO_IMAGES_YET", ORB_SLAM2::Tracking::eTrackingState::NO_IMAGES_YET)
        .value("NOT_INITIALIZED", ORB_SLAM2::Tracking::eTrackingState::NOT_INITIALIZED)
        .value("OK", ORB_SLAM2::Tracking::eTrackingState::OK)
        .value("LOST", ORB_SLAM2::Tracking::eTrackingState::LOST);
    
    boost::python::enum_<ORB_SLAM2::System::eSensor>("Sensor")
        .value("MONOCULAR", ORB_SLAM2::System::eSensor::MONOCULAR)
        .value("STEREO", ORB_SLAM2::System::eSensor::STEREO)
        .value("RGBD", ORB_SLAM2::System::eSensor::RGBD);

    boost::python::class_<ORBSlamPython, boost::noncopyable>("System", boost::python::init<const char*, const char*, boost::python::optional<ORB_SLAM2::System::eSensor>>())
        .def(boost::python::init<std::string, std::string, boost::python::optional<ORB_SLAM2::System::eSensor>>())
        .def("initialize", &ORBSlamPython::initialize)
        .def("load_and_process_mono", &ORBSlamPython::loadAndProcessMono)
        .def("process_image_mono", &ORBSlamPython::processMono)
        .def("load_and_process_stereo", &ORBSlamPython::loadAndProcessStereo)
        .def("process_image_stereo", &ORBSlamPython::processStereo)
        .def("load_and_process_rgbd", &ORBSlamPython::loadAndProcessRGBD)
        .def("process_rgbd", &ORBSlamPython::processRGBD)
        .def("process_image_rgbd", &ORBSlamPython::processRGBD)
        .def("shutdown", &ORBSlamPython::shutdown)
        .def("is_running", &ORBSlamPython::isRunning)
        .def("reset", &ORBSlamPython::reset)
        .def("set_mode", &ORBSlamPython::setMode)
        .def("set_use_viewer", &ORBSlamPython::setUseViewer)
        .def("set_not_erase", &ORBSlamPython::setNotErase)
        .def("get_keyframe_points", &ORBSlamPython::getKeyframePoints)
        .def("get_trajectory_points", &ORBSlamPython::getTrajectoryPoints)
        .def("get_all_map_points", &ORBSlamPython::getAllMapPoints)
        .def("get_tracking_state", &ORBSlamPython::getTrackingState)
        .def("get_num_features", &ORBSlamPython::getNumFeatures)
        .def("get_num_matched_features", &ORBSlamPython::getNumMatches)
        .def("correct_loop", &ORBSlamPython::correctLoop)
        .def("compute_sim3", &ORBSlamPython::computeSim3)
        .def("save_map", &ORBSlamPython::saveMap)
        .def("load_map", &ORBSlamPython::loadMap)
        .def("activate_localization_mode",  &ORBSlamPython::activateLocalizationMode)
        .def("deactivate_localization_mode",  &ORBSlamPython::deactivateLocalizationMode)
        .def("save_settings", &ORBSlamPython::saveSettings)
        .def("load_settings", &ORBSlamPython::loadSettings)
        .def("save_settings_file", &ORBSlamPython::saveSettingsFile)
        .staticmethod("save_settings_file")
        .def("load_settings_file", &ORBSlamPython::loadSettingsFile)
        .staticmethod("load_settings_file");
}

ORBSlamPython::ORBSlamPython(std::string vocabFile, std::string settingsFile, ORB_SLAM2::System::eSensor sensorMode)
    : vocabluaryFile(vocabFile),
    settingsFile(settingsFile),
    sensorMode(sensorMode),
    system(nullptr),
    bUseViewer(false),
    bUseRGB(true)
{
    
}

ORBSlamPython::ORBSlamPython(const char* vocabFile, const char* settingsFile, ORB_SLAM2::System::eSensor sensorMode)
    : vocabluaryFile(vocabFile),
    settingsFile(settingsFile),
    sensorMode(sensorMode),
    system(nullptr),
    bUseViewer(false),
    bUseRGB(true)
{

}

ORBSlamPython::~ORBSlamPython()
{
}

bool ORBSlamPython::initialize()
{
    system = std::make_shared<ORB_SLAM2::System>(vocabluaryFile, settingsFile, sensorMode, bUseViewer);
    return true;
}

bool ORBSlamPython::isRunning()
{
    return system != nullptr;
}

void ORBSlamPython::reset()
{
    if (system)
    {
        system->Reset();
    }
}

cv::Mat ORBSlamPython::loadAndProcessMono(std::string imageFile, double timestamp)
{
    /*if (!system)
    {
        return false;
    }*/
    cv::Mat im = cv::imread(imageFile, cv::IMREAD_COLOR);
    if (bUseRGB)
    {
        cv::cvtColor(im, im, cv::COLOR_BGR2RGB);
    }
    return this->processMono(im, timestamp);
}

cv::Mat ORBSlamPython::processMono(cv::Mat image, double timestamp)
{
    /*if (!system)
    {
        return false;
    }
    if (image.data)
    {
        cv::Mat pose = system->TrackMonocular(image, timestamp);
        return !pose.empty();
    }
    else
    {
        return false;
    }*/
    
    cv::Mat pose = system->TrackMonocular(image, timestamp);
    return pose;
}

bool ORBSlamPython::loadAndProcessStereo(std::string leftImageFile, std::string rightImageFile, double timestamp)
{
    if (!system)
    {
        return false;
    }
    cv::Mat leftImage = cv::imread(leftImageFile, cv::IMREAD_COLOR);
    cv::Mat rightImage = cv::imread(rightImageFile, cv::IMREAD_COLOR);
    if (bUseRGB) {
        cv::cvtColor(leftImage, leftImage, cv::COLOR_BGR2RGB);
        cv::cvtColor(rightImage, rightImage, cv::COLOR_BGR2RGB);
    }
    return this->processStereo(leftImage, rightImage, timestamp);
}

bool ORBSlamPython::processStereo(cv::Mat leftImage, cv::Mat rightImage, double timestamp)
{
    if (!system)
    {
        return false;
    }
    if (leftImage.data && rightImage.data) {
        cv::Mat pose = system->TrackStereo(leftImage, rightImage, timestamp);
        return !pose.empty();
    }
    else
    {
        return false;
    }
}

bool ORBSlamPython::loadAndProcessRGBD(std::string imageFile, std::string depthImageFile, double timestamp)
{
    if (!system)
    {
        return false;
    }
    cv::Mat im = cv::imread(imageFile, cv::IMREAD_COLOR);
    if (bUseRGB)
    {
        cv::cvtColor(im, im, cv::COLOR_BGR2RGB);
    }
    cv::Mat imDepth = cv::imread(depthImageFile, cv::IMREAD_UNCHANGED);
    return this->processRGBD(im, imDepth, timestamp);
}

bool ORBSlamPython::processRGBD(cv::Mat image, cv::Mat depthImage, double timestamp)
{
    if (!system)
    {
        return false;
    }
    if (image.data && depthImage.data)
    {
        cv::Mat pose = system->TrackRGBD(image, depthImage, timestamp);
        return !pose.empty();
    }
    else
    {
        return false;
    }
}

void ORBSlamPython::shutdown()
{
    if (system)
    {
        system->Shutdown();
        system.reset();
    }
}

ORB_SLAM2::Tracking::eTrackingState ORBSlamPython::getTrackingState() const
{
    if (system)
    {
        return static_cast<ORB_SLAM2::Tracking::eTrackingState>(system->GetTrackingState());
    }
    return ORB_SLAM2::Tracking::eTrackingState::SYSTEM_NOT_READY;
}

unsigned int ORBSlamPython::getNumFeatures() const
{
    if (system)
    {
        return system->GetTracker()->mCurrentFrame.mvKeys.size();
    }
    return 0;
}

unsigned int ORBSlamPython::getNumMatches() const
{
    if (system)
    {
        // This code is based on the display code in FrameDrawer.cc, with a little extra safety logic to check the length of the vectors.
        ORB_SLAM2::Tracking* pTracker = system->GetTracker();
        unsigned int matches = 0;
        unsigned int num = pTracker->mCurrentFrame.mvKeys.size();
        if (pTracker->mCurrentFrame.mvpMapPoints.size() < num)
        {
            num = pTracker->mCurrentFrame.mvpMapPoints.size();
        }
        if (pTracker->mCurrentFrame.mvbOutlier.size() < num)
        {
            num = pTracker->mCurrentFrame.mvbOutlier.size();
        }
        for(unsigned int i = 0; i < num; ++i)
        {
            ORB_SLAM2::MapPoint* pMP = pTracker->mCurrentFrame.mvpMapPoints[i];
            if(pMP && !pTracker->mCurrentFrame.mvbOutlier[i] && pMP->Observations() > 0)
            {
                ++matches;
            }
        }
        return matches;
    }
    return 0;
}

boost::python::list ORBSlamPython::getKeyframePoints() const
{
    if (!system)
    {
        return boost::python::list();
    }

    // This is copied from the ORB_SLAM2 System.SaveKeyFrameTrajectoryTUM function, with some changes to output a python tuple.
    vector<ORB_SLAM2::KeyFrame*> vpKFs = system->GetKeyFrames();
    std::sort(vpKFs.begin(), vpKFs.end(), ORB_SLAM2::KeyFrame::lId);

    // Transform all keyframes so that the first keyframe is at the origin.
    // After a loop closure the first keyframe might not be at the origin.
    //cv::Mat Two = vpKFs[0]->GetPoseInverse();

    boost::python::list trajectory;

    for(size_t i=0; i<vpKFs.size(); i++)
    {
        ORB_SLAM2::KeyFrame* pKF = vpKFs[i];

        // pKF->SetPose(pKF->GetPose()*Two);

        if(pKF->isBad())
            continue;

        cv::Mat R = pKF->GetRotation().t();
        cv::Mat t = pKF->GetCameraCenter();
        trajectory.append(boost::python::make_tuple(
                              pKF->mTimeStamp,
                              pKF->mnId,
                              R.at<float>(0,0),
                              R.at<float>(0,1),
                              R.at<float>(0,2),
                              t.at<float>(0),
                              R.at<float>(1,0),
                              R.at<float>(1,1),
                              R.at<float>(1,2),
                              t.at<float>(1),
                              R.at<float>(2,0),
                              R.at<float>(2,1),
                              R.at<float>(2,2),
                              t.at<float>(2)
                              ));
    }

    return trajectory;
}

ORB_SLAM2::KeyFrame* ORBSlamPython::GetKeyFrameById(long unsigned int query) const {
    vector<ORB_SLAM2::KeyFrame*> vpKFs = system->GetKeyFrames();
    for(size_t i=0; i<vpKFs.size(); i++)
    {
        ORB_SLAM2::KeyFrame* pKF = vpKFs[i];
        if (pKF->mnId == query) {
            return pKF;
        }
    }
    return NULL;
}

void ORBSlamPython::setNotErase(long unsigned int query) const {
    ORB_SLAM2::KeyFrame* pKF = ORBSlamPython::GetKeyFrameById(query);
    pKF->SetKeep();
}

boost::python::list ORBSlamPython::computeSim3(boost::python::list p3d1,
                                               boost::python::list p3d2) const
{
    ORB_SLAM2::Sim3Solver* pSolver = new ORB_SLAM2::Sim3Solver(false);
    boost::python::list result;
    cv::Mat A(3,3,CV_32F);
    cv::Mat B(3,3,CV_32F);
    
    boost::python::extract<double> _A11(p3d1[0]);
    double A11 = _A11;
    boost::python::extract<double> _A12(p3d1[1]);
    double A12 = _A12;
    boost::python::extract<double> _A13(p3d1[2]);
    double A13 = _A13;
    
    boost::python::extract<double> _A21(p3d1[3]);
    double A21 = _A21;
    boost::python::extract<double> _A22(p3d1[4]);
    double A22 = _A22;
    boost::python::extract<double> _A23(p3d1[5]);
    double A23 = _A23;
    
    boost::python::extract<double> _A31(p3d1[6]);
    double A31 = _A31;
    boost::python::extract<double> _A32(p3d1[7]);
    double A32 = _A32;
    boost::python::extract<double> _A33(p3d1[8]);
    double A33 = _A33;
    
    A = (cv::Mat_<float>(3,3) << A11, A12, A13,
                                 A21, A22, A23,
                                 A31, A32, A33);
   
    
    boost::python::extract<double> _B11(p3d2[0]);
    double B11 = _B11;
    boost::python::extract<double> _B12(p3d2[1]);
    double B12 = _B12;
    boost::python::extract<double> _B13(p3d2[2]);
    double B13 = _B13;
    
    boost::python::extract<double> _B21(p3d2[3]);
    double B21 = _B21;
    boost::python::extract<double> _B22(p3d2[4]);
    double B22 = _B22;
    boost::python::extract<double> _B23(p3d2[5]);
    double B23 = _B23;
    
    boost::python::extract<double> _B31(p3d2[6]);
    double B31 = _B31;
    boost::python::extract<double> _B32(p3d2[7]);
    double B32 = _B32;
    boost::python::extract<double> _B33(p3d2[8]);
    double B33 = _B33;
    
    B = (cv::Mat_<float>(3,3) << B11, B12, B13,
                                 B21, B22, B23,
                                 B31, B32, B33);
    
    pSolver->ComputeSim3(A, B);
    
    /*cout << "OSP: I head A" << endl;
    cout << A << endl;
    
    cout << "OSP: I head B" << endl;
    cout << B << endl;
    
    cout << "OSP:" << pSolver->mR12i.at<float>(0,0) << endl;
    cout << "OSP:" << pSolver->mR12i.at<float>(0,1) << endl;
    cout << "OSP:" << pSolver->mR12i.at<float>(0,2) << endl;*/
    
    result.append(pSolver->mR12i.at<float>(0,0));
    result.append(pSolver->mR12i.at<float>(0,1));
    result.append(pSolver->mR12i.at<float>(0,2));
    result.append(pSolver->mR12i.at<float>(1,0));
    result.append(pSolver->mR12i.at<float>(1,1));
    result.append(pSolver->mR12i.at<float>(1,2));
    result.append(pSolver->mR12i.at<float>(2,0));
    result.append(pSolver->mR12i.at<float>(2,1));
    result.append(pSolver->mR12i.at<float>(2,2));
    
    result.append(pSolver->mt12i.at<float>(0,0));
    result.append(pSolver->mt12i.at<float>(0,1));
    result.append(pSolver->mt12i.at<float>(0,2));
    
    result.append(pSolver->ms12i);
    
    return result;
}

void ORBSlamPython::correctLoop(long unsigned int loopMnId,
                                long unsigned int curMnId,
                                boost::python::dict corrections,
                                boost::python::dict loopConnections
                                ) const
{   
    std::cout << "ORBSlamPython: Starting loop correction" << std::endl;
    ORB_SLAM2::LoopClosing::KeyFrameAndPose CorrectedSim3, NonCorrectedSim3;
    map<ORB_SLAM2::KeyFrame*, set<ORB_SLAM2::KeyFrame*> > LoopConnections;
    
    boost::python::list keys = corrections.keys();
    for (int index = 0; index < boost::python::len(keys); ++index)
    {
        boost::python::extract<long unsigned int> extractedKey(keys[index]);
        if (!extractedKey.check())
        {
            std::cout << "Problem with key in corrections dictionary" << std::endl;
            continue;
        }
        long unsigned int mnId = extractedKey;
        ORB_SLAM2::KeyFrame* pKF = ORBSlamPython::GetKeyFrameById(mnId);
        if (pKF == NULL) {
            std::cout << "ORBSlamPython: Got mnId of non-existant keyframe: " << mnId << std::endl;
            continue;
        }
        boost::python::list s3v = boost::python::extract<boost::python::list>(corrections[mnId]);

        // Deal with sim3 related matters
        //boost::python::list val(extractedValue);
        Eigen::Matrix<double,3,3> R;
        
        boost::python::extract<double> r1(s3v[0]);
        boost::python::extract<double> r2(s3v[1]);
        boost::python::extract<double> r3(s3v[2]);
        boost::python::extract<double> r4(s3v[3]);
        boost::python::extract<double> r5(s3v[4]);
        boost::python::extract<double> r6(s3v[5]);
        boost::python::extract<double> r7(s3v[6]);
        boost::python::extract<double> r8(s3v[7]);
        boost::python::extract<double> r9(s3v[8]);
        
        boost::python::extract<double> t1(s3v[9]);
        boost::python::extract<double> t2(s3v[10]);
        boost::python::extract<double> t3(s3v[11]);
        
        boost::python::extract<double> s(s3v[12]);
        R << r1,r2,r3,
             r4,r5,r6,
             r7,r8,r9;
             
        Eigen::Matrix<double,3,1> T;
        T << t1,t2,t3;
        
        g2o::Sim3 sim3Corrected(R, T, s);
        
        // Set corrected pose
        CorrectedSim3[pKF] = sim3Corrected;
        
        cv::Mat Tiw = pKF->GetPose();
        cv::Mat Riw = Tiw.rowRange(0,3).colRange(0,3);
        cv::Mat tiw = Tiw.rowRange(0,3).col(3);
        g2o::Sim3 g2oSiw(ORB_SLAM2::Converter::toMatrix3d(Riw),ORB_SLAM2::Converter::toVector3d(tiw),1.0);
        
        // Set non-corrected pose
        NonCorrectedSim3[pKF]=g2oSiw;
    } 
    
    boost::python::list conKeys = loopConnections.keys();
    for (int index = 0; index < boost::python::len(conKeys); ++index)
    {
        boost::python::extract<long unsigned int> extractedKey(conKeys[index]);
        if (!extractedKey.check())
        {
            std::cout << "Problem with key in connections dictionary" << std::endl;
            continue;
        }
        
        long unsigned int mnId = extractedKey;
        ORB_SLAM2::KeyFrame* pKF = ORBSlamPython::GetKeyFrameById(mnId);
        
        if (pKF == NULL) {
            std::cout << "ORBSlamPython: Got mnId of non-existant keyframe (2): " << mnId << std::endl;
            continue;
        }
        
        boost::python::list kfList = boost::python::extract<boost::python::list>(loopConnections[mnId]);
        
        // Deal with loop connection related matters
        set<ORB_SLAM2::KeyFrame*> connectedKFs;
        for (int i = 0; i < len(kfList); ++i) {
            boost::python::extract<long unsigned int> mnIdOther(kfList[i]);
            ORB_SLAM2::KeyFrame* opKF = ORBSlamPython::GetKeyFrameById(mnIdOther);
            if (opKF == NULL) {
                std::cout << "ORBSlamPython: Got mnId of non-existant keyframe (3): " << mnIdOther << std::endl;
                continue;
            }
            connectedKFs.insert(opKF);
        }
        
        // Set loop connection
        LoopConnections[pKF] = connectedKFs;
    }
    
    // Find the loop key frame and current key frame
    ORB_SLAM2::KeyFrame* loopKF = ORBSlamPython::GetKeyFrameById(loopMnId);
    if (loopKF == NULL) {
        std::cout << "ORBSlamPython: loopKF mnId points to non-existant keyframe (3): " << loopMnId << std::endl;
        return;
    }
    ORB_SLAM2::KeyFrame* curKF = ORBSlamPython::GetKeyFrameById(curMnId);
    if (curKF == NULL) {
        std::cout << "ORBSlamPython: curKF mnId points to non-existant keyframe (3): " << curMnId << std::endl;
        return;
    }
    
    
    // Inform loop closer about the loop
    system->mpLoopCloser->InformExternalLoop(loopKF, curKF, NonCorrectedSim3, CorrectedSim3, LoopConnections); 
}

boost::python::list ORBSlamPython::getTrajectoryPoints() const
{
    if (!system)
    {
        return boost::python::list();
    }

    // This is copied from the ORB_SLAM2 System.SaveTrajectoryKITTI function, with some changes to output a python tuple.
    vector<ORB_SLAM2::KeyFrame*> vpKFs = system->GetKeyFrames();
    std::sort(vpKFs.begin(), vpKFs.end(), ORB_SLAM2::KeyFrame::lId);

    // Transform all keyframes so that the first keyframe is at the origin.
    // After a loop closure the first keyframe might not be at the origin.
    // Of course, if we have no keyframes, then just use the identity matrix.
    cv::Mat Two = cv::Mat::eye(4,4,CV_32F);
    if (vpKFs.size() > 0) {
        cv::Mat Two = vpKFs[0]->GetPoseInverse();
    }

    boost::python::list trajectory;

    // Frame pose is stored relative to its reference keyframe (which is optimized by BA and pose graph).
    // We need to get first the keyframe pose and then concatenate the relative transformation.
    // Frames not localized (tracking failure) are not saved.

    // For each frame we have a reference keyframe (lRit), the timestamp (lT) and a flag
    // which is true when tracking failed (lbL).
    std::list<ORB_SLAM2::KeyFrame*>::iterator lRit = system->GetTracker()->mlpReferences.begin();
    std::list<double>::iterator lT = system->GetTracker()->mlFrameTimes.begin();
    for(std::list<cv::Mat>::iterator lit=system->GetTracker()->mlRelativeFramePoses.begin(), lend=system->GetTracker()->mlRelativeFramePoses.end();lit!=lend;lit++, lRit++, lT++)
    {
        ORB_SLAM2::KeyFrame* pKF = *lRit;

        cv::Mat Trw = cv::Mat::eye(4,4,CV_32F);

        while(pKF != NULL && pKF->isBad())
        {
            ORB_SLAM2::KeyFrame* pKFParent;

            // std::cout << "bad parent" << std::endl;
            Trw = Trw*pKF->mTcp;
            pKFParent = pKF->GetParent();
            if (pKFParent == pKF) {
                // We've found a frame that is it's own parent, presumably a root or something. Break out
                break;
            } else {
                pKF = pKFParent;
            }
        }
        if (pKF != NULL && !pKF->isBad()) {
            Trw = Trw*pKF->GetPose()*Two;

            cv::Mat Tcw = (*lit)*Trw;
            cv::Mat Rwc = Tcw.rowRange(0,3).colRange(0,3).t();
            cv::Mat twc = -Rwc*Tcw.rowRange(0,3).col(3);

            trajectory.append(boost::python::make_tuple(
                                *lT,
                                Rwc.at<float>(0,0),
                                Rwc.at<float>(0,1),
                                Rwc.at<float>(0,2),
                                twc.at<float>(0),
                                Rwc.at<float>(1,0),
                                Rwc.at<float>(1,1),
                                Rwc.at<float>(1,2),
                                twc.at<float>(1),
                                Rwc.at<float>(2,0),
                                Rwc.at<float>(2,1),
                                Rwc.at<float>(2,2),
                                twc.at<float>(2)
                            ));
        }
    }

    return trajectory;
}

boost::python::list ORBSlamPython::getAllMapPoints() const {
  if (!system)
  {
      return boost::python::list();
  }
  
  boost::python::list points;
  const vector<ORB_SLAM2::MapPoint*> &vpMPs = system->mpMap->GetAllMapPoints();
  
  for(size_t i=0, iend=vpMPs.size(); i<iend;i++)
  {
      if(vpMPs[i]->isBad())
          continue;
      cv::Mat pos = vpMPs[i]->GetWorldPos();
      points.append(
        boost::python::make_tuple(pos.at<float>(0), pos.at<float>(1), pos.at<float>(2))
      );
  }
  return points;
}

void ORBSlamPython::saveMap(std::string path) {
  system->SaveMap(path);
}

void ORBSlamPython::loadMap(std::string path) {
  system->LoadMap(path);
}

void ORBSlamPython::activateLocalizationMode()
{
  if (system) {
    system->ActivateLocalizationMode();
  }
}

void ORBSlamPython::deactivateLocalizationMode()
{
  if (system) {
    system->DeactivateLocalizationMode();
  }
}

void ORBSlamPython::setMode(ORB_SLAM2::System::eSensor mode)
{
    sensorMode = mode;
}

void ORBSlamPython::setUseViewer(bool useViewer)
{
    bUseViewer = useViewer;
}

void ORBSlamPython::setRGBMode(bool rgb)
{
    bUseRGB = rgb;
}

bool ORBSlamPython::saveSettings(boost::python::dict settings) const
{
    return ORBSlamPython::saveSettingsFile(settings, settingsFile);
}

boost::python::dict ORBSlamPython::loadSettings() const
{
    return ORBSlamPython::loadSettingsFile(settingsFile);
}

bool ORBSlamPython::saveSettingsFile(boost::python::dict settings, std::string settingsFilename)
{
    cv::FileStorage fs(settingsFilename.c_str(), cv::FileStorage::WRITE);
    
    boost::python::list keys = settings.keys();
    for (int index = 0; index < boost::python::len(keys); ++index)
    {
        boost::python::extract<std::string> extractedKey(keys[index]);
        if (!extractedKey.check())
        {
            continue;
        }
        std::string key = extractedKey;
        
        boost::python::extract<int> intValue(settings[key]);
        if (intValue.check())
        {
            fs << key << int(intValue);
            continue;
        }
        
        boost::python::extract<float> floatValue(settings[key]);
        if (floatValue.check())
        {
            fs << key << float(floatValue);
            continue;
        }
        
        boost::python::extract<std::string> stringValue(settings[key]);
        if (stringValue.check())
        {
            fs << key << std::string(stringValue);
            continue;
        }
    }
    
    return true;
}

// Helpers for reading cv::FileNode objects into python objects.
boost::python::list readSequence(cv::FileNode fn, int depth=10);
boost::python::dict readMap(cv::FileNode fn, int depth=10);

boost::python::dict ORBSlamPython::loadSettingsFile(std::string settingsFilename)
{
    cv::FileStorage fs(settingsFilename.c_str(), cv::FileStorage::READ);
    cv::FileNode root = fs.root();
    if (root.isMap()) 
    {
        return readMap(root);
    }
    else if (root.isSeq())
    {
        boost::python::dict settings;
        settings["root"] = readSequence(root);
        return settings;
    }
    return boost::python::dict();
}


// ----------- HELPER DEFINITIONS -----------
boost::python::dict readMap(cv::FileNode fn, int depth)
{
    boost::python::dict map;
    if (fn.isMap()) {
        cv::FileNodeIterator it = fn.begin(), itEnd = fn.end();
        for (; it != itEnd; ++it) {
            cv::FileNode item = *it;
            std::string key = item.name();
            
            if (item.isNone())
            {
                map[key] = boost::python::object();
            }
            else if (item.isInt())
            {
                map[key] = int(item);
            }
            else if (item.isString())
            {
                map[key] = std::string(item);
            }
            else if (item.isReal())
            {
                map[key] = float(item);
            }
            else if (item.isSeq() && depth > 0)
            {
                map[key] = readSequence(item, depth-1);
            }
            else if (item.isMap() && depth > 0)
            {
                map[key] = readMap(item, depth-1);  // Depth-limited recursive call to read inner maps
            }
        }
    }
    return map;
}

boost::python::list readSequence(cv::FileNode fn, int depth)
{
    boost::python::list sequence;
    if (fn.isSeq()) {
        cv::FileNodeIterator it = fn.begin(), itEnd = fn.end();
        for (; it != itEnd; ++it) {
            cv::FileNode item = *it;
            
            if (item.isNone())
            {
                sequence.append(boost::python::object());
            }
            else if (item.isInt())
            {
                sequence.append(int(item));
            }
            else if (item.isString())
            {
                sequence.append(std::string(item));
            }
            else if (item.isReal())
            {
                sequence.append(float(item));
            }
            else if (item.isSeq() && depth > 0)
            {
                sequence.append(readSequence(item, depth-1)); // Depth-limited recursive call to read nested sequences
            }
            else if (item.isMap() && depth > 0)
            {
                sequence.append(readMap(item, depth-1));
            }
        }
    }
    return sequence;
}

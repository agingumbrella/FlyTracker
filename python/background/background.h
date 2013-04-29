#ifndef TRACKER_H
#define TRACKER_H
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <set>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/gpu.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include "npinterface.h"

using namespace std;
typedef vector<cv::Point> Contour;
typedef vector<Contour> ContourVector;

struct ObsState
{
    ObsState() : ID(-1) {}
    ObsState(int _ID, cv::Point2f _obs_pos, cv::RotatedRect _ellipse)
        : ID(_ID), pos(_obs_pos), ellipse( _ellipse) {}
    virtual ~ObsState() {}

    // Fly ID
    int ID;
    // Observed position
    cv::Point2f pos;
    // Observed ellipse
    cv::RotatedRect ellipse;
};

struct TrackerState
{
    TrackerState() {}
    TrackerState(int _framenum, int _numobs)
        : framenum(_framenum), numobs(_numobs) {}
    virtual ~TrackerState() {}

    // frame number of state
    int framenum;
    // number of observed objects in state
    int numobs;
    // vector of current observations
    vector<ObsState> obs;
    // vector of current inferred positions
    vector<InferredState> inferred;
};

class BackgroundFinder
{
public:
    BackgroundFinder() : nframes_(0) {}
    virtual ~BackgroundFinder() {}
    void init(cv::Mat& frame) { frame.convertTo(background_, CV_32FC1); }
    void addFrame(cv::Mat& frame);
    void getBackground(cv::Mat& mat);

private:
    float nframes_;
    cv::Mat background_;
};

class BGSubtractor
{
public:
    BGSubtractor();
    virtual ~BGSubtractor() {}

    // Compute background image
    void setBackground(cv::Mat& bg) { background_ = bg; }

    // Add another frame to the tracker
    void addFrame(cv::Mat& currimg);
	
	// serialize history to disk
    void saveHistory(const char* fname);

    // Accessor methods dealing with current frame
    vector<cv::Point2f> getCurrentPositions();
    vector<cv::RotatedRect> getCurrentEllipses();
    int getCurrentNumObjects() { return currstate_.numobs; }
    int getTotalNumObjects() { return nobjects_; }
    TrackerState getCurrentState() { return currstate_; }
    TrackerState getState(int i) { return history_[i]; }


private:
    void findCentroids(const ContourVector& contours, vector<cv::Point2f> &currpos);
    void findEllipses(const ContourVector& contours, vector<cv::RotatedRect> &currellipse);
    void filterContours(ContourVector& contours);
    void updateState(const vector<cv::Point2f>& currpos, const vector<cv::RotatedRect>& currellipse);

private:
    // current frame
    int nframes_;

    // total number of objects in current frame
    int nobjects_;

    // background subtractors
    cv::gpu::VIBE_GPU vibe_subtractor_;

    // Current state
    TrackerState currstate_;

    // Past states
    vector<TrackerState> history_;

    cv::Mat background_;

    // Size threshold for object rejection
    double sizeThreshold_;
};

#endif // TRACKER_H

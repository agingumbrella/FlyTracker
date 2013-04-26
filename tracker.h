#ifndef TRACKER_H
#define TRACKER_H
#include <vector>
#include <opencv2/opencv.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/progress.hpp>
#include <iostream>

using namespace std;
typedef vector<cv::Point> Contour;
typedef vector<Contour> ContourVector;

struct TrackerState
{
    TrackerState(cv::Point2f _obs_pos, cv::Point2f _inf_pos, cv::RotatedRect _ellipse)
        : obs_pos(_obs_pos), inf_pos(_inf_pos), ellipse(_ellipse) {}
    virtual ~TrackerState() {}

    // Observed position
    cv::Point2f obs_pos;
    // Inferred position from particle filter
    cv::Point2f inf_pos;
    // Observed ellipse
    cv::RotatedRect ellipse;
};
//
// Tracker
// Step 1: Add all frames to be analyzed
// Step 2: Run trainClassifier to train classifier on all fg/bg segmented images
// Step 3:
// Step 3: Run compute
class Tracker
{
public:
    Tracker();
    virtual ~Tracker();

    // Add another frame to the tracker
    void addFrame(cv::Mat& currimg);

    // Draw the locations of objects in the current frame of the tracker
    void drawCurrent(cv::Mat& img);

    // Compute trajectories for each object, starting from the first frame
    void computeTrajectories();

    // Accessor methods dealing with current frame
    vector<cv::Point2f> getCurrentPositions() { return currpos_; }
    vector<cv::RotatedRect> getCurrentEllipses() { return currellipse_; }
    int getCurrentNumObjects() { return currpos_.size(); }

    // DON'T USE -- SLOW
    //void computeMedianBackgroundModel();

private:
    void findCentroids(const ContourVector& contours);
    void findEllipses(const ContourVector& contours);
    void filterContours(ContourVector& contours);

private:
    // background subtractor
    cv::BackgroundSubtractorMOG2 subtractor_;

    // Current centroid centers
    vector<cv::Point2f> currpos_;
    // Current ellipses
    vector<cv::RotatedRect> currellipse_;

    // Past states
    vector<TrackerState> history_;

    // Size threshold for object rejection
    double sizeThreshold_;
};


#endif // TRACKER_H

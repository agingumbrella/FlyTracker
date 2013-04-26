#include "tracker.h"

Tracker::Tracker()
{
    // TODO Make this a parameter
    sizeThreshold_ = 200.0;
    subtractor_ = cv::BackgroundSubtractorMOG2(150, 10.0, false);
}

Tracker::~Tracker() {
}

void Tracker::addFrame(cv::Mat &currimg) {

    ContourVector contours;

    // Subtract background
    cv::Mat fmask;
    subtractor_(currimg, fmask, -1);

    // Find contours in foreground mask
    cv::erode(fmask, fmask, cv::Mat());
    cv::dilate(fmask, fmask, cv::Mat());
    cv::findContours(fmask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // Remove contours below a size threshold
    filterContours(contours);

    // Estimate object positions from contours
    findCentroids(contours);
    findEllipses(contours);

    // TODO Find ellipses and contours that seem to contain more than one fly
    // then split them appropriately
    // Estimate object positions given previous frame

    // Update history
//    history_.push_back(TrackerState(currpos_, currpos_, currellipse_));

}

void Tracker::findCentroids(const ContourVector& contours) {
    vector<cv::Moments> mu(contours.size() );
    for( int i = 0; i < contours.size(); i++ ) {
        mu[i] = cv::moments( contours[i], false );
    }

    //vector<cv::Point2f> mc(contours_.size());
    currpos_.clear();
    for( int i = 0; i < contours.size(); i++ ) {
        currpos_.push_back(cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ));
    }
}

void Tracker::findEllipses(const ContourVector& contours) {
    currellipse_.clear();
    for (int i = 0; i < contours.size(); ++i) {
        currellipse_.push_back(cv::fitEllipse(cv::Mat(contours[i])));
    }
}

void Tracker::filterContours(ContourVector& contours) {
    double area;
    for (ContourVector::iterator it = contours.begin(); it != contours.end(); ) {
        area = cv::contourArea(*it);
        // remove contours with
        if (area < sizeThreshold_) {
            contours.erase(it);
        } else {
            ++it;
        }
    }
}

void Tracker::drawCurrent(cv::Mat &img) {
    cv::cvtColor(img, img, CV_GRAY2RGB);
    //cv::drawContours(img, contours_, -1, cv::Scalar(0,0,255), CV_FILLED);

    // draw positions
    for (int i = 0; i < currpos_.size(); ++i) {
        cv::circle(img, currpos_[i], 5, cv::Scalar(0,255,0), 5);
        cv::ellipse(img, currellipse_[i], cv::Scalar(0,0,255), 2, 8);
    }
}


// same background modeling method as in Dickinson paper
// compute median and std dev of each pixel over the course of the movie
// to parameterize a gaussian distribution at each pixel
// DON'T USE -- VERY SLOW
/*
void Tracker::computeMedianBackgroundModel() {
    using namespace boost::accumulators;
    cv::Mat med(height_, width_, CV_8UC1);
    cv::Mat stddev = cv::Mat::zeros(height_, width_, CV_8UC1);
    float c = 1.4826;

    int total = height_*width_;
    boost::progress_display display(total);

    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            accumulator_set<uint8_t, stats<tag::median(with_p_square_quantile)> > median_acc;
            for (int t = 0; t < images_->size(); ++t) {
                uint8_t temp = images_->at(t).at<uchar>(i,j);
                median_acc(temp);
            }
            uint8_t curr_med = median(median_acc);
            med.at<uint8_t>(i,j) = curr_med;

            accumulator_set<uint8_t, stats<tag::median(with_p_square_quantile)> > stddev_acc;
            for (int t = 0; t < images_->size(); ++t) {
                uint8_t temp = images_->at(t).at<uchar>(i,j);
                stddev_acc(abs(temp - curr_med));
            }

            uint8_t curr_stddev = median(stddev_acc);
            stddev.at<uint8_t>(i,j) = curr_stddev;
            ++display;
        }
    }
}*/

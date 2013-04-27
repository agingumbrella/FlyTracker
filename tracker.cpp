#include "tracker.h"

void Background::addFrame(cv::Mat &frame) {
    //background_ += frame;
    cv::accumulate(frame, background_);
    nframes_++;
}

void Background::getBackground(cv::Mat &mat) {
    if (nframes_ > 0) {
        cv::Mat temp = cv::Mat::ones(background_.rows, background_.cols, CV_32F) * nframes_;
        cv::divide(background_, temp, mat);
        cv::convertScaleAbs(mat, mat);
    } else {
        mat = background_;
    }
}


Tracker::Tracker(bool useVIBE)
{
    useVIBE_ = useVIBE;
    // TODO Make these parameters
    sizeThreshold_ = 200.0;
    nframes_ = 0;
    nobjects_ = 0;
    mog_subtractor_ = cv::BackgroundSubtractorMOG2(150, 10.0, false);
}



void Tracker::addFrame(cv::Mat &currimg) {

    vector<cv::Point2f> currpos;
    vector<cv::RotatedRect> currellipse;

    cv::gpu::GpuMat curr_gpu(currimg);
    // allow 10 frames to burn
    if (nframes_ == 0) {

        // if no preset background, use first frame (may cause ghosting)
        if (background_.empty()) {
                background_ = currimg;
        }
        if (useVIBE_) {
            cv::gpu::GpuMat bg(background_);
            vibe_subtractor_.initialize(bg);
        }
    }
    ContourVector contours;

    // Subtract background
    cv::Mat fmask;
    if (useVIBE_) {
        cv::gpu::GpuMat fmask_gpu;
        vibe_subtractor_(curr_gpu, fmask_gpu);
        fmask_gpu.download(fmask);
    } else {
        mog_subtractor_(currimg, fmask, -1);
    }

    // Find contours in foreground mask
    cv::erode(fmask, fmask, cv::Mat());
    cv::dilate(fmask, fmask, cv::Mat(), cv::Point(-1,-1));
    cv::findContours(fmask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // Remove contours below a size threshold
    filterContours(contours);

    // Estimate object positions from contours
    findCentroids(contours, currpos);
    findEllipses(contours, currellipse);


    // TODO Find ellipses and contours that seem to contain more than one fly
    // then split them appropriately

    // Add current positions and ellipses to the history
    updateState(currpos, currellipse);

    // Update history
//    history_.push_back(TrackerState(currpos_, currpos_, currellipse_));

    ++nframes_;
}

vector<cv::Point2f> Tracker::getCurrentPositions() {
    vector<cv::Point2f> toret;
    for (int i = 0; i < currstate_.obs.size(); ++i) {
        toret.push_back(currstate_.obs[i].pos);
    }
    return toret;
}

vector<cv::RotatedRect> Tracker::getCurrentEllipses() {
    vector<cv::RotatedRect> toret;
    for (int i = 0; i < currstate_.obs.size(); ++i) {
        toret.push_back(currstate_.obs[i].ellipse);
    }
    return toret;
}

void Tracker::updateState(const vector<cv::Point2f>& currpos, const vector<cv::RotatedRect>& currellipse) {
    // initialize the total number of objects on the first frame
    if (nframes_ == 0) {
        nobjects_ = currpos.size();
    }
    currstate_ = TrackerState(nframes_, currpos.size());
    for (int i = 0; i < currpos.size(); ++i) {
        // give each fly an ID of 0
        FlyState obs(-1, currpos[i], currellipse[i]);
        currstate_.obs.push_back(obs);
    }
    history_.push_back(currstate_);
}

void Tracker::findCentroids(const ContourVector& contours, vector<cv::Point2f>& currpos) {
    vector<cv::Moments> mu(contours.size() );
    for( int i = 0; i < contours.size(); i++ ) {
        mu[i] = cv::moments( contours[i], false );
    }

    currpos.clear();
    for( int i = 0; i < contours.size(); i++ ) {
        currpos.push_back(cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ));
    }
}

void Tracker::findEllipses(const ContourVector& contours, vector<cv::RotatedRect>& currellipse) {
    currellipse.clear();
    for (int i = 0; i < contours.size(); ++i) {
        currellipse.push_back(cv::fitEllipse(cv::Mat(contours[i])));
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

void Tracker::computeTrajectories() {
}

// know there are K flies
void Tracker::assignIdentities() {

    int ntrue = 19; // true number from first frame
    double maxcost = 100.0;

    // initialize kalman filters
    vector<cv::KalmanFilter> filters;
    int dynamicParams = 4;
    int measureParams = 2;
    for (int i = 0; i < ntrue; ++i) {
        cv::KalmanFilter KF(dynamicParams, measureParams);
        KF.transitionMatrix = *(cv::Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
        KF.statePre.at<float>(0) = history_[0].obs[i].pos.x;
        KF.statePre.at<float>(1) = history_[0].obs[i].pos.y;
        KF.statePre.at<float>(2) = 0;
        KF.statePre.at<float>(3) = 0;
        setIdentity(KF.measurementMatrix);
        setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-4));
        setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-1));
        setIdentity(KF.errorCovPost, cv::Scalar::all(.1));
        filters.push_back(KF);
    }

    for (int t = 0; t < 1; ++t) {
        // at each time step q_t have K' observations
        // have K x K' matrix

        int nobs = history_[t].numobs; // number observed
//        int N = max(ntrue, nobs);
        int N = ntrue + nobs;
        // turn into N x N matrix where N = max(K,K'),
        // by adding dummy variables for additional objects and for removed objects
        cv::Mat weights = cv::Mat::zeros(N, N, CV_64FC1);

        // set costs
        cv::Mat costs = cv::Mat::zeros(nobs, ntrue, CV_64FC1);
        for (int i = 0; i < ntrue; ++i) {
            cv::KalmanFilter kf = filters[i];
            const cv::Mat predpos = kf.predict();
            for (int j = 0; j < nobs; ++j) {
                cv::Point2f obspos = history_[t].obs[j].pos;
                costs.at<double>(j,i) = (double) pow(predpos.at<float>(0) - obspos.x,2) + pow(predpos.at<float>(1) - obspos.y,2);
            }
        }

        // set up weights
        // set top left of matrix to costs
        for (int i = 0; i < nobs; ++i) {
            for (int j = 0; j < ntrue; ++j) {
                weights.at<double>(i,j) = (double) costs.at<double>(i,j);
            }
        }
        // set top right of matrix to maxcost
        for (int i = 0; i < nobs; ++i) {
            for (int j = ntrue; j < N; ++j) {
                weights.at<double>(i,j) = maxcost;
            }
        }
        // set bottom left to maxcost
        for (int i = nobs; i < N; ++i) {
            for (int j = 0; j < ntrue; ++j) {
                weights.at<double>(i,j) = maxcost;
            }
        }
        for (int i = 0; i < N; ++i) {
            for (int  j = 0; j < N; ++j) {
                cout << weights.at<double>(i,j) << ", ";
            }
            cout << endl;
        }
        // compute maximal weight bipartite matching on matrix
        vector<int> target2obs;
        vector<int> obs2target;
//        hungarian(weights, target2obs, obs2target);
        //cout << target2obs[0] << endl;
        // conditional on assignments from Hungarian algorithm, perform kalman filter update
        // where objects assigned to dummy observations don't perform a measurement update
    }
}

void Tracker::predictFlyState() {
    // XXX Will fail if lose any flies in the first two frames
    if (nframes_ < 2) {
        prediction_ = currstate_.obs;
    } else {
    }
}

void Tracker::drawCurrent(cv::Mat &img) {
    cv::cvtColor(img, img, CV_GRAY2RGB);

    // draw positions
    for (int i = 0; i < currstate_.obs.size(); ++i) {
        cv::circle(img, currstate_.obs[i].pos, 5, cv::Scalar(0,255,0), 5);
        cv::ellipse(img, currstate_.obs[i].ellipse, cv::Scalar(0,0,255), 2, 8);
    }
}

void Tracker::drawFrame(const TrackerState& state, cv::Mat& img) {
    cv::cvtColor(img, img, CV_GRAY2RGB);
    int fontFace = cv::FONT_HERSHEY_PLAIN;
    double fontScale = 2;
    int thickness = 3;

    // draw positions
    for (int i = 0; i < state.obs.size(); ++i) {
        cv::circle(img, state.obs[i].pos, 5, cv::Scalar(0,255,0), 5);
        cv::ellipse(img, state.obs[i].ellipse, cv::Scalar(0,0,255), 2, 8);
        string label = boost::lexical_cast<string>(state.obs[i].ID);
        cv::putText(img, label, state.obs[i].pos, fontFace, fontScale, cv::Scalar(255,255,255),thickness);
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

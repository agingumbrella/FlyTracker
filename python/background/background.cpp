#include "background.h"

void BackgroundFinder::addFrame(cv::Mat &frame) {
    cv::accumulate(frame, background_);
    nframes_++;
}

void BackgroundFinder::getBackground(cv::Mat &mat) {
    if (nframes_ > 0) {
        cv::Mat temp = cv::Mat::ones(background_.rows, background_.cols, CV_32F) * nframes_;
        cv::divide(background_, temp, mat);
        cv::convertScaleAbs(mat, mat);
    } else {
        mat = background_;
    }
}


BGSubtractor::BGSubtractor()
{
    sizeThreshold_ = 200.0;
    nframes_ = 0;
    nobjects_ = 0;
}



void BGSubtractor::addFrame(cv::Mat &currimg) {

    vector<cv::Point2f> currpos;
    vector<cv::RotatedRect> currellipse;

    cv::gpu::GpuMat curr_gpu(currimg);
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

    // Add current positions and ellipses to the history
    updateState(currpos, currellipse);

    ++nframes_;
}

vector<cv::Point2f> BGSubtractor::getCurrentPositions() {
    vector<cv::Point2f> toret;
    for (int i = 0; i < currstate_.obs.size(); ++i) {
        toret.push_back(currstate_.obs[i].pos);
    }
    return toret;
}

vector<cv::RotatedRect> BGSubtractor::getCurrentEllipses() {
    vector<cv::RotatedRect> toret;
    for (int i = 0; i < currstate_.obs.size(); ++i) {
        toret.push_back(currstate_.obs[i].ellipse);
    }
    return toret;
}

void BGSubtractor::updateState(const vector<cv::Point2f>& currpos, const vector<cv::RotatedRect>& currellipse) {
    // initialize the total number of objects on the first frame
    if (nframes_ == 0) {
        nobjects_ = currpos.size();
    }
    currstate_ = BackgroundState(nframes_, currpos.size());
    for (int i = 0; i < currpos.size(); ++i) {
        // give each fly an ID of -1
        ObsState obs(-1, currpos[i], currellipse[i]);
        currstate_.obs.push_back(obs);
    }
    history_.push_back(currstate_);
}

void BGSubtractor::findCentroids(const ContourVector& contours, vector<cv::Point2f>& currpos) {
    vector<cv::Moments> mu(contours.size() );
    for( int i = 0; i < contours.size(); i++ ) {
        mu[i] = cv::moments( contours[i], false );
    }

    currpos.clear();
    for( int i = 0; i < contours.size(); i++ ) {
        currpos.push_back(cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ));
    }
}

void BGSubtractor::findEllipses(const ContourVector& contours, vector<cv::RotatedRect>& currellipse) {
    currellipse.clear();
    for (int i = 0; i < contours.size(); ++i) {
        currellipse.push_back(cv::fitEllipse(cv::Mat(contours[i])));
    }
}

void BGSubtractor::filterContours(ContourVector& contours) {
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

// serialize history to file
void Background::saveHistory(const char *fname) {
    ofstream file;
    file.open(fname);
    for (int t = 0; t < history_.size(); ++t) {
        file << t << " ";
        file << history_[t].numobs << " ";
        for (int i = 0; i < history_[t].obs.size(); ++i) {
            ObsState currobs = history_[t].obs[i];
            file << currobs.ellipse.angle << ":" << currobs.ellipse.center.x << ":" << currobs.ellipse.center.y << ":" << currobs.ellipse.size.width << ":" << currobs.ellipse.size.height << " ";
        }
        file << endl;
    }
    file.close();
}

BOOST_PYTHON_MODULE(BGSubtractor) {
	using namespace boost::python;
	class_<BackgroundFinder>("BackgroundFinder")
		.def("init", &BackgroundFinder::init)
		.def("addFrame", &BackgroundFinder::addFrame)
		.def("getBackground", &BackgroundFinder::getBackground)
	;
}

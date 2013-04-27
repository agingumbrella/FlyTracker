//#include "mainwindow.h"
//#include <QApplication>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "fmf.h"
#include "tracker.h"

using namespace std;
const char* testfile = "/home/jlab/Documents/WillData/or83bGal420130422_172747.fmf";

void displayScaled(const char* winName, cv::Mat& currFrame, int scaleFactor) {
    cv::Mat resized;
    if (!currFrame.empty()) {
        cv::resize(currFrame, resized, cv::Size(currFrame.cols/scaleFactor, currFrame.rows/scaleFactor));
        cv::imshow(winName, resized);
    }
}

int main(int argc, char *argv[]) {
    cv::setUseOptimized(true);
    cv::setNumThreads(8);

    cout << "Loading movie info..." << endl;
    FMFReader fmfreader(testfile);
    cout << "Num frames: " << fmfreader.getNFrames() << endl;
    cout << "Width: " << fmfreader.getWidth() << ", Height: " << fmfreader.getHeight() << endl;
    cout << "Loading all frames.." << std::endl;
    vector<double> timestamps = fmfreader.getTimestamps();

    const char* WIN_RF = "Reference";
    const char* WIN_PROC = "Processed";
    int scalefactor = 3; // scaling factor for display
    cv::namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
 //   cv::namedWindow(WIN_PROC, CV_WINDOW_AUTOSIZE);

    cout << "Computing background..." << endl;
    Background bg;
    cv::Mat frame = fmfreader.readFrame(0);
    bg.init(frame);
    for (int i = 1; i < fmfreader.getNFrames(); ++i) {
        frame = fmfreader.readFrame(i);
        bg.addFrame(frame);
    }
    cv::Mat background;
    bg.getBackground(background);

    Tracker tracker;
    tracker.setBackground(background);
    cv::Mat currframe;
    cout << "Computing fly positions..." << endl;
    for (int i = 0; i < fmfreader.getNFrames(); ++i) {
        //currframe = frames[i];
        currframe = fmfreader.readFrame(i);

        tracker.addFrame(currframe);
    }
    cout << "Computing fly trajectories..." << endl;
    tracker.assignIdentities();

    cout << "Displaying fly trajectories..." << endl;
    for (int i = 0; i < fmfreader.getNFrames(); ++i) {
        TrackerState state = tracker.getState(i);
        currframe = fmfreader.readFrame(i);
        tracker.drawFrame(state, currframe);
        cout << "Showing frame " << i << " with N=" << state.numobs << endl;
        displayScaled(WIN_RF, currframe, scalefactor);
//        displayScaled(WIN_PROC, fmask, scalefactor);
        cv::waitKey(100);
    }
    cv::waitKey(0);
}


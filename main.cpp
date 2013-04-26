//#include "mainwindow.h"
//#include <QApplication>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>

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
    cv::namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
 //   cv::namedWindow(WIN_PROC, CV_WINDOW_AUTOSIZE);

    Tracker tracker;
    cv::Mat currframe;
    cout << "Displaying frames" << std::endl;
    int scalefactor = 3; // scaling factor for display

    for (int i = 0; i < fmfreader.getNFrames(); ++i) {
        //currframe = frames[i];
        currframe = fmfreader.readFrame(i);
        tracker.addFrame(currframe);
        tracker.drawCurrent(currframe);
        displayScaled(WIN_RF, currframe, scalefactor);

        cout << "Showing frame " << i << " with N=" ;
        cout << tracker.getCurrentNumObjects() << endl;
        cout << tracker.getCurrentPositions() << endl;
//        displayScaled(WIN_PROC, fmask, scalefactor);
        cv::waitKey(10);
    }
    cv::waitKey(0);
}


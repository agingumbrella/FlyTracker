//#include "mainwindow.h"
//#include <QApplication>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <sys/stat.h>

#include "fmf.h"
#include "tracker.h"

using namespace std;

void displayScaled(const char* winName, cv::Mat& currFrame, int scaleFactor) {
    cv::Mat resized;
    if (!currFrame.empty()) {
        cv::resize(currFrame, resized, cv::Size(currFrame.cols/scaleFactor, currFrame.rows/scaleFactor));
        cv::imshow(winName, resized);
    }
}

int main(int argc, char *argv[]) {
    string fname;
    string outname;
    cv::setUseOptimized(true);
    cv::setNumThreads(8);
    if (argc == 3) {
        cout << "Reading " << argv[1] << " and saving to " << argv[2] << endl;
        fname = string(argv[1]);
        outname = string(argv[2]);
    } else {
        cout << "Two arguments expected" << endl;
		exit(0);
    }

    //ifstream f(fname.c_str());
    //if (!f.good()) {
     //   cout << "Input file " << fname << " not found" << endl;
//        exit(0);
    //}
    //f.close();

    FMFReader fmfreader;
    if (fmfreader.open(fname.c_str()) == -1) {
        cout << "Input file " << fname << " not found" << endl;
        exit(0);
    }
    cout << "Loading movie info..." << endl;
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
        displayScaled(WIN_RF, frame, 3);
        cv::waitKey(10);
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
        displayScaled(WIN_RF, currframe, scalefactor);
        cv::waitKey(1);
    }
    cout << "Saving fly positions..." << endl;
    tracker.saveHistory(outname.c_str());
    //cout << "Computing fly trajectories..." << endl;
    //tracker.assignIdentities();
    /*
    cout << "Displaying fly trajectories..." << endl;
    for (int i = 0; i < fmfreader.getNFrames(); ++i) {
        TrackerState state = tracker.getState(i);
        currframe = fmfreader.readFrame(i);
        tracker.drawFrame(state, currframe);
        cout << "Showing frame " << i << " with N=" << state.numobs << endl;
        displayScaled(WIN_RF, currframe, scalefactor);
        cv::waitKey(0);
    }
    cv::waitKey(0);
    */
    return 1;
}


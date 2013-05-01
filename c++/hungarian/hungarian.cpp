#include "hungarian.h"

void hungarian(const cv::Mat &weights, vector<int>& colids, vector<int>& rowids) {
    colids.clear();
    rowids.clear();
    int n = weights.rows;
    cost** ccosts = (cost **) malloc(sizeof(cost *) * n);
    for (int i = 0; i < n; ++i) {
        ccosts[i] = (cost*) malloc(sizeof(cost) * n);
    }
    long* rowsol = (long *)malloc(sizeof(long)*n);
    long* colsol = (long *)malloc(sizeof(long)*n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            ccosts[i][j] = weights.at<double>(i,j);
        }
    }
    asp(n, ccosts, rowsol, colsol);

    for (int i = 0; i < n; ++i) {
        colids.push_back(colsol[i]);
        rowids.push_back(rowsol[i]);
    }
    free(rowsol);
    free(colsol);
    for (int i = 0; i < n; ++i) {
        free(ccosts[i]);
    }
    free(ccosts);
}

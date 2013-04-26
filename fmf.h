#ifndef FMF_H
#define FMF_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;

typedef unsigned int uint32;
typedef unsigned long long uint64;

struct FMFHeader
{
    /* Number of pixels in each frame */
    int npixels;
    /* Number of bytes in header */
    uint64 headersize;
    /* version of fmf file */
    uint32 version;
    /* number of rows of pixels in each frame */
    uint32 nr;
    /* number of columns of pixels in each frame */
    uint32 nc;
    /* number of bytes to encode each frame */
    uint64 bytesperchunk;
    /* number of frames */
    uint64 nframes;
    /* number of bits used to encode each pixel */
    uint32 bitsperpixel;
    /* 1 if nr is odd, 0 o.w. This is necessary because the
       video is required to have and even number of rows and
       columns. We pad one column on the right and one column
       on th bottom if necessary. */
    int isoddrows;
    /* 1 if nc is odd, 0 o.w. */
    int isoddcols;
    /* if isoddcols==1, then we will need an extra buffer to
       read in the unpadded image. */
};

/* Based on Kristin Bransen's code in fmfcompress.c in  motmot/FlyMovieFormat/fmf2m4v/*/
class FMFReader
{
public:
    FMFReader();
    FMFReader(const char* fname);
    virtual ~FMFReader();
    void open(const char* fname);
    void close();
    cv::Mat readFrame(int frame);
    vector<cv::Mat> readFrames();

    std::vector<double> getTimestamps() { return timestamps_; }
    bool isOpened() { return is_opened_; }
    uint32 getVersion() { return header_.version; }
    int getNPixels() { return header_.npixels; }
    uint64 getNFrames() { return header_.nframes - 1; }
    uint32 getBitsPerPixel() { return header_.bitsperpixel; }
    uint32 getWidth() { return header_.nc; }
    uint32 getHeight() { return header_.nr; }

private:
    /* Reads the header from the stream fmffp and stores
      information about the movie */
    void readHeader();
    void readFrameImpl(int frame, cv::Mat* img);

private:
    bool is_opened_;

    FILE* fmffp_;
    FMFHeader header_;
    uint8_t * extrabuf_;
    uint8_t* buf_;
    std::vector<double> timestamps_;
};

/* TODO Implement FMFWriter to save to AVI */
class FMFWriter
{
public:
    FMFWriter() {}
    virtual ~FMFWriter() {}
};

#endif // FMF_H

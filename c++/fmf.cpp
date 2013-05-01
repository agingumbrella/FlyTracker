#include "fmf.h"

FMFReader::FMFReader()
    : is_opened_(false)
{
}


FMFReader::~FMFReader() {
    if (header_.isoddcols) {
        free(extrabuf_);
    }
    free(buf_);
    if (is_opened_) {
        fclose(fmffp_);
    }
}

int FMFReader::open(const char *fname) {
    fmffp_ = fopen(fname, "rb");
    if (fmffp_ == 0) {
        perror("Couldn't open file\n");
        fprintf(stderr, "Could not open input fmf file %s for reading.\n", fname);
        return -1;
    }
    is_opened_ = true;
    readHeader();
    buf_ = (uint8_t*)malloc(sizeof(uint8_t)*header_.npixels);

    timestamps_.clear();
    double curr_timestamp;
    for (int i = 0; i < (int)getNFrames(); ++i) {
        fseek(fmffp_, header_.headersize + header_.bytesperchunk*i, SEEK_SET);
        fread(&curr_timestamp, sizeof(double), 1, fmffp_);
        timestamps_.push_back(curr_timestamp);
    }
    return 1;
}

void FMFReader::close() {
    fclose(fmffp_);
    free(buf_);
    if (header_.isoddcols) {
        free(extrabuf_);
    }
    is_opened_ = false;
}


/*
 * Frame Chunks
 * d timestamp (double64)
 * \*B frame (row*col bytes)
 */
void FMFReader::readFrameImpl(int frame, cv::Mat* mat=0) {

    int i;
    fseek(fmffp_, header_.headersize + header_.bytesperchunk*frame+sizeof(double), SEEK_SET);
    cv::Mat empty; // empty matrix to return on failure
    if (header_.isoddcols) {
        if(fread(extrabuf_,sizeof(uint8_t),header_.npixels,fmffp_) < header_.npixels){
            return;
        }

        /* then copy over to buf, adding an extra pixel at every row */
        for(i = 0; i < header_.nr; i++){
            memcpy(buf_,extrabuf_,header_.nc);
            buf_[header_.nc] = 128;
            buf_ = &buf_[header_.nc+1];
            extrabuf_ = &extrabuf_[header_.nc];
        }
    }
    else {
        /* otherwise, just read in the frame */
        size_t size = fread(buf_, sizeof(uint8_t), header_.npixels, fmffp_);
        if(fread(buf_,sizeof(uint8_t),header_.npixels,fmffp_) < header_.npixels){
            return;
        }
    }

    /* if the number of rows is odd, we are going to pad with a row
     on the bottom that is 128 */
    if(header_.isoddrows){
        for(i = 0; i < header_.nc; i++) {
            buf_[header_.npixels-i-1] = 128;
        }
    }

    *mat = cv::Mat(header_.nr, header_.nc, CV_8UC1, buf_);
    return;
}

cv::Mat FMFReader::readFrame(int frame) {
    cv::Mat img;
    readFrameImpl(frame, &img);
    return img;
}

vector<cv::Mat> FMFReader::readFrames() {
    vector<cv::Mat> frames;
    for (int i = 0; i < getNFrames(); ++i) {
        cv::Mat temp = readFrame(i);
        frames.push_back(temp.clone());
    }
    return frames;
}

void FMFReader::readHeader() {
    if (!isOpened()) {
        return;
    }

    uint32 formatlen;
    int sizeofuint32 = 4;
    int sizeofuint64 = 8;

    /* seek to the start of the movie */
    fseek(fmffp_,0,SEEK_SET);

    /* version number */
    if(fread(&header_.version,sizeofuint32,1,fmffp_) < 1){
        fprintf(stderr,"Error reading version number of input fmf file.\n");
        return;
    }

    if(header_.version == 1){
        /* version 1 only formats with MONO8 */
        header_.bitsperpixel = 8;
    }

    else if(header_.version == 3){
        /* version 3 encodes the length of the format string */
        if(fread(&formatlen,sizeofuint32,1,fmffp_)<1){
            fprintf(stderr,"Error reading format length of input fmf file.\n");
            return;
        }
        /* followed by the format string */
        fseek(fmffp_,formatlen,SEEK_CUR);
        /* followed by the bits per pixel */
        if(fread(&header_.bitsperpixel,sizeofuint32,1,fmffp_)<1){
            fprintf(stderr,"Error reading bits per pixel of input fmf file.\n");
            return;
        }
    }
    /* all versions then have the height of the frame */
    if(fread(&header_.nr,sizeofuint32,1,fmffp_)<1){
        fprintf(stderr,"Error reading frame height of input fmf file.\n");
        return;
    }
    /* width of the frame */
    if(fread(&header_.nc,sizeofuint32,1,fmffp_)<1){
        fprintf(stderr,"Error reading frame width of input fmf file.\n");
        return;
    }
    /* bytes encoding a frame */
    if(fread(&header_.bytesperchunk,sizeofuint64,1,fmffp_)<1){
        fprintf(stderr,"Error reading bytes per chunk of input fmf file.\n");
        return;
    }
    /* number of frames */
    if(fread(&header_.nframes,sizeofuint64,1,fmffp_)<1){
        fprintf(stderr,"Error reading number of frames of input fmf file.\n");
        return;
    }
    /* we've now read in the whole header, so use ftell
     to get the header size */
    header_.headersize = ftell(fmffp_);
    /* check to see if the number of frames was written */
    if (header_.nframes == 0) {
        /* if not, then seek to the end of the file, and compute number
       of frames based on file size */
        fseek(fmffp_,0,SEEK_END);
        header_.nframes = (ftell(fmffp_) - header_.headersize)/header_.bytesperchunk;
    }

    /* compute total number of pixels */
    header_.npixels = header_.nr * header_.nc;
    /* store whether the number of rows, columns is odd */
    header_.isoddrows = (header_.nr % 2) == 1;
    header_.isoddcols = (header_.nc % 2) == 1;
    /* if number of columns is odd, allocate extrabuf */
    if(header_.isoddcols){
        /* allocate an extra buffer so that we can pad images */
        extrabuf_ = (uint8_t*)malloc(sizeof(uint8_t)*header_.npixels);
    }
    return;
}

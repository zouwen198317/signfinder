#include <opencv/cv.h>
#include "lib/bloblib/Blob.h"
#include "lib/bloblib/BlobResult.h"

void findCorners(CBlob& blob, CvPoint* corners, int numCorners = 4, double distThr = 10, double angleThr = M_PI / 8.);

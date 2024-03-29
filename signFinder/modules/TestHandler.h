/* 
 * See .cpp file for more information
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "lib/bloblib/Blob.h"
#include "lib/bloblib/BlobResult.h"
#include <string>

using namespace std;


double compareMasks(IplImage* estimation, IplImage* label, double* _fp = NULL);
bool blobCorrect(IplImage* blob, IplImage* label,double numBlobs);

void fillConvexHull(IplImage* img, CvSeq* hull, CvScalar color);
void fillConvexHull(IplImage* img, CBlob* blob, CvScalar color);

bool checkLabeledBlobs(CBlobResult& detectedBlobs, CvSize origImg, char* file, int& fp, int& fn, int& multipleDetections, CBlobResult* correctBlobsOut = NULL, CBlobResult* incorrectBlobsOut = NULL);

int compareText(string detected, char* imgfile);

/* Support Functions */
int levenshtein(const char* a, const char* b);
string trim(string in);


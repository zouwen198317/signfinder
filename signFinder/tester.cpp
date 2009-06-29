/*
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is tester.cpp .
 *
 * The Initial Developer of the Original Code is Tijs Zwinkels.
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 * Tijs Zwinkels <opensource AT tumblecow DOT net>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 */


#include <iostream>
#include <deque>
#include "lib/histogramtool/histogramTool.h"
#include "TestHandler.h"
//#include "lib/bloblib/Blob.h"
//#include "lib/bloblib/BlobResult.h"

using namespace std;

const double HISTTHRESHOLD = 0.0001;
const int WINDOWX = 1600;
const int WINDOWY = 1200;

int _curFile=0;

CvHistogram* _posHist;
CvHistogram* _negHist;
deque<double> _tp;
deque<double> _fp;

void processFile(char* file)
{
	// See if we can find a mask for this file.
        string maskfile(file);
        IplImage* label = cvLoadImage((maskfile+"_mask.png").c_str());
	// We can't test performance is there's no known correct mask.
	if (!label)
	{
		cerr << "No mask for file " << file << " skipping..\n";
		return;	
	}

	IplImage* img;
	img = cvLoadImage(file);
        if (!img)
        {
                cerr << "Could not load file " << file << endl;
                exit(1);
        }
	cout << "Processing " << file << endl;
	// return mask of images that have been detected.
	CvMat* imgMat = cvCreateMatHeader(img->height, img->width,CV_8UC3);
	imgMat = cvGetMat(img,imgMat);
	IplImage* histMatched = skinDetectBayes(imgMat,_posHist,_negHist,HISTTHRESHOLD); 
	IplImage* result = cvCreateImage(cvGetSize(histMatched), IPL_DEPTH_8U, 3);
	cvCvtColor(histMatched, result, CV_GRAY2RGB);

	// Compare results we known-correct mask.
	double fp;
	double tp = compareMasks(result,label,&fp);
	printf("**** file: %s, tp: %f, fp: %f\n",file,tp,fp);
	_tp.push_back(tp);
	_fp.push_back(fp);	

	// Cleanup
	cvReleaseMatHeader(&imgMat);
	cvReleaseImage(&img);
	cvReleaseImage(&result);
	cvReleaseImage(&histMatched);
	cvReleaseImage(&label);
}

void printResults(deque<double>& tp, deque<double>& fp)
{
	// calculate averages
	double tpavg = 0;
	double fpavg = 0;
	for (int i=0; i<tp.size(); ++i)
	{
		tpavg += tp[i];
		fpavg += fp[i];
	}
	tpavg /= tp.size();
	fpavg /= tp.size();

	// calculate standard deviation
	double tpdev = 0;
	double fpdev = 0;	
	for (int i=0; i<tp.size(); ++i)
	{
		tpdev += pow(tp[i] - tpavg,2);
		fpdev += pow(fp[i] - fpavg,2);
	}
	tpdev = sqrt(tpdev / tp.size());
	fpdev = sqrt(fpdev / fp.size());

	printf("\n\nTrue Positives avg: %f, sd: %f\n",tpavg,tpdev);
	printf("False Positives avg: %f, sd: %f\n",fpavg,fpdev);
}

void init()
{
	_posHist = loadHistogram("posHist.hist");
        _negHist = loadHistogram("negHist.hist");
	if (!(_posHist && _negHist))
	{
		cerr << "ERROR: posHist.hist and/or negHist.hist histogram failed to load." << endl;
		exit(1);
	}
}

void cleanup()
{
	cvReleaseHist(&_posHist);
	cvReleaseHist(&_negHist);
	_posHist = NULL;
	_negHist = NULL;
}

int main(int argc, char** argv)
{
        if (argc < 2)
        {
                cerr << "Usage: " << argv[0] << " <image-files>" << endl;
                exit(0);
        }

	init();
	// iterate through all files.
	while (++_curFile < argc)
        	processFile(argv[_curFile]);

	printResults(_tp,_fp);
	cleanup();

	return 0;
}
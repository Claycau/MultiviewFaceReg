#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
#include <math.h>

using namespace cv;

namespace POHE{

	// @param01: src: input image
	// @param02: dst: output image
	// @param03: blockHeight: height
	// @param04: blockWidth: width
	bool enhance(Mat &src,Mat &dst,int blockHeight,int blockWidth);

}
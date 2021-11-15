#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

unsigned char getPixel(Mat& src, int c, int r)
{
	if (c < 0) c = 0;
	if (c >= src.cols) c = src.cols - 1;
	if (r < 0) r = 0;
	if (r >= src.rows) r = src.rows - 1;

	return src.at<unsigned char>(r, c);
}

int main(int argc, char** argv)
{
	//Kép betöltése
	CommandLineParser parser(argc, argv, "{@input | lena.jpg | input image}");
	Mat src = imread(samples::findFile(parser.get<cv::String>("@input")), IMREAD_COLOR);
	if (src.empty())
	{
		return EXIT_FAILURE;
	}

	Mat gsrc;
	cvtColor(src, gsrc, COLOR_BGR2GRAY);
	imshow("Original", gsrc);

	int radius = 10;

	Mat avg = Mat::zeros(gsrc.rows, gsrc.cols, gsrc.type());
	//Elore kiszamoljuk pow((2 * radius + 1), 2)
	float dev = pow((2 * radius + 1), 2);

	//Lokális átlag mátrix
	for (int i = 0; i < avg.rows; i++) {
		for (int j = 0; j < avg.cols; j++) {
			float sum = 0;
			//Kép mátrix
			for (int k = i - radius; k <= i + radius; k++) {
				for (int l = j - radius; l <= j + radius; l++) {

					sum += getPixel(gsrc, l, k);
				}
			}
			avg.at<unsigned char>(i, j) = sum / dev;
		}
	}

	//SZÓRÁSNÉGYZET
	Mat variance = Mat::zeros(gsrc.rows, gsrc.cols, gsrc.type());
	for (int i = 0; i < variance.rows; i++) {
		for (int j = 0; j < variance.cols; j++) {
			float sum = 0;
			for (int k = i - radius; k <= i + radius; k++) {
				for (int l = j - radius; l <= j + radius; l++) {
					uchar src_pix = getPixel(gsrc, l, k);
					uchar avg_pix = getPixel(avg, j, i);
					int dif = src_pix - avg_pix;
					int res = dif * dif;
					sum += res; //pow((getPixel(src, src_c, src_r) - getPixel(avg, var_c, var_r)), 2);
				}
			}
			variance.at<unsigned char>(i, j) = sum / dev;
		}
	}

	int contrast = 100;         //Sd elvárt kontraszt
	int brightness = 128;       //Md elvárt világosság
	float bright_mod = 0.25f;   //r brightness modifier
	float cont_mod = 2.5f;      //Amax contrast modifier

	//Wallis
	Mat dest = Mat::zeros(gsrc.rows, gsrc.cols, gsrc.type());
	for (int i = 0; i < dest.rows; i++) {
		for (int j = 0; j < dest.cols; j++) {

			float temp = ((gsrc.at<unsigned char>(i, j) - avg.at<unsigned char>(i, j))
				* ((cont_mod * contrast) / (contrast + (cont_mod * (float)sqrt(variance.at<unsigned char>(i, j))))))
				+ ((bright_mod * brightness) + ((1.0f - bright_mod) * avg.at<unsigned char>(i, j)));

			if (temp <= 0) {
				temp = 0;
			}
			else if (temp >= 255) {
				temp = 255;
			}
			dest.at<unsigned char>(i, j) = temp;
		}
	}
	imshow("Wallis", dest);

	waitKey(0);

	return EXIT_SUCCESS;
}
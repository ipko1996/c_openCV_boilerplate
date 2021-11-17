#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int comp(const void* p1, const void* p2) // az összehasonlító függény definíciója
{
	return *(const unsigned char*)p1 - *(const unsigned char*)p2;
}

int main(int argc, char** argv)
{
	//Kép betöltése
	CommandLineParser parser(argc, argv, "{@input | sajat_zajos.jpg | input image}");
	Mat src = imread(samples::findFile(parser.get<cv::String>("@input")), IMREAD_COLOR);
	if (src.empty())
	{
		return EXIT_FAILURE;
	}

	Mat gsrc;
	cvtColor(src, gsrc, COLOR_BGR2GRAY);
	imshow("Original", gsrc);

	cvtColor(src, gsrc, cv::COLOR_BGR2GRAY);
	Mat avg = gsrc.clone();
	Mat out = gsrc.clone();

	Mat outlier = (Mat_<double>(3, 3) <<
		1.0 / 8.0, 1.0 / 8.0, 1.0 / 8.0,
		1.0 / 8.0, 0, 1.0 / 8.0,
		1.0 / 8.0, 1.0 / 8.0, 1.0 / 8.0
		);
	int kernel_rad = 1;

	for (int i = 0; i < gsrc.rows - (2 * kernel_rad); i++)
	{
		for (int j = 0; j < gsrc.cols - (2 * kernel_rad); j++)
		{
			double temp = 0;
			for (int k = 0; k < 2 * kernel_rad + 1; k++)
			{
				for (int l = 0; l < 2 * kernel_rad + 1; l++)
				{
					temp +=
						gsrc.at<unsigned char>(i + k, j + l) *
						outlier.at<double>(k, l);
				}
			}
			//if (temp < 0) {
			//	temp = 0;
			//}
			//else if (temp > 255) {
			//	temp = 255;
			//}
			avg.at<unsigned char>(i + kernel_rad, j + kernel_rad) = (unsigned char)temp;

			if (abs(avg.at<unsigned char>(i, j) - gsrc.at<unsigned char>(i, j)) <= 30)
			{
				out.at<unsigned char>(i, j) = gsrc.at<unsigned char>(i, j);
			}
			else
			{
				out.at<unsigned char>(i, j) = avg.at<unsigned char>(i, j);
			}
		}
	}

	Mat output = gsrc.clone();

	int w = 1;
	int* o = new int[4];
	int ww = (w + 2) * (w + 2);
	unsigned char* pixels = new unsigned char[ww];
	int x;

	for (size_t i = w; i < gsrc.rows - w; i++)
	{
		for (size_t j = w; j < gsrc.cols - w; j++)
		{
			x = 0;
			for (size_t k = i - w; k <= i + w; k++)
			{
				for (size_t l = j - w; l <= j + w; l++)
				{
					pixels[x] = gsrc.at<unsigned char>(k, l);
					x++;
				}
			}

			qsort(pixels, 9, sizeof(unsigned char), comp);
			output.at<unsigned char>(i, j) = pixels[5];
		}
	}

	imshow("Avg filter", out);
	imshow("Median filter", output);

	waitKey(0);

	return EXIT_SUCCESS;
}
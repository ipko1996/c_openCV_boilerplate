#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <math.h>  
#include <string>
using namespace std;
using namespace cv;

void saveImg(string name, Mat src)
{
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
    compression_params.push_back(90);
    bool result = false;
    try
    {
        result = imwrite(name, src, compression_params);
    }
    catch (const cv::Exception& ex)
    {
        fprintf(stderr, "Exception saving file: %s\n", ex.what());
    }
    if (result)
        printf("File saved.\n");
    else
        printf("ERROR: Can't save file.\n");

    compression_params.pop_back();
    compression_params.pop_back();

}

void showHist(string name, Mat* src)
{
    //Mat temp = src.clone();
    Mat hist;
    int hist_size = 256;
    float range[] = { 0,256 };
    const float* hist_range = { range };
    bool uniform = true, accumulate = false;

    calcHist(src, 1, 0, Mat(), hist, 1, &hist_size, &hist_range, uniform, accumulate);
    int hist_w = 512;
    int hist_h = 512;
    int bin_w = cvRound((double)hist_w / hist_size);

    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    for (int i = 1; i < hist_size; i++)
    {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
            Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
            Scalar(255, 255, 255), 2, 1, 0);
    }

    imshow(name, histImage);
    saveImg("hist_" + name + ".jpg", histImage);
}

int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv, "{@input | lena.jpg | input image}");
    Mat src = imread(samples::findFile(parser.get<String>("@input")), IMREAD_COLOR);
    if (src.empty())
    {
        return EXIT_FAILURE;
    }

    Mat gsrc;
    cvtColor(src, gsrc, cv::COLOR_BGR2GRAY);
        
    Mat out = gsrc.clone();

    Mat kernel = (Mat_<double>(3, 3) << 
        1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 
        1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
        1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0);

    Mat gauss = (Mat_<double>(5, 5) <<
        1.0 / 126.0, 2.0 / 126.0, 3.0 / 126.0, 2.0 / 126.0, 1.0 / 126.0,
        2.0 / 126.0, 7.0 / 126.0, 11.0 / 126.0, 7.0 / 126.0, 2.0 / 126.0,
        3.0 / 126.0, 11.0 / 126.0, 17.0 / 126.0, 11.0 / 126.0, 3.0 / 126.0,
        2.0 / 126.0, 7.0 / 126.0, 11.0 / 126.0, 7.0 / 126.0, 2.0 / 126.0,
        1.0 / 126.0, 2.0 / 126.0, 3.0 / 126.0, 2.0 / 126.0, 1.0 / 126.0
        );

    Mat laplace = (Mat_<double>(3, 3) <<
        0, -1.0 / 4.0, 0,
        -1.0 / 4.0, 4.0 / 4.0, -1.0 / 4.0,
        0, -1.0 / 4.0, 0
        );

    Mat prewitt_ver = (Mat_<double>(3, 3) <<
        1.0 / 3.0, 0, -1.0 / 3.0,
        1.0 / 3.0, 0, -1.0 / 3.0,
        1.0 / 3.0, 0, -1.0 / 3.0);

    Mat prewitt_hor = (Mat_<double>(3, 3) <<
        1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0,
        0, 0, 0,
        -1.0 / 3.0, -1.0 / 3.0, -1.0 / 3.0);

    int kernel_rad = (int) (3-1) / 2;

    for (int i = 0; i < gsrc.rows-(2*kernel_rad); i++)
    {
        for (int j = 0; j < gsrc.cols-(2*kernel_rad); j++)
        {
            double temp = 0;
            for (int k = 0; k < 2*kernel_rad+1; k++)
            {
                for (int l = 0; l < 2*kernel_rad+1; l++)
                {
                    temp += 
                        gsrc.at<unsigned char>(i + k, j + l) * 
                        laplace.at<double>(k, l);
                }
            }
            if (temp < 0) {
                temp = 0;
            }
            else if (temp > 255) {
                temp = 255;
            }
            out.at<unsigned char>(i + kernel_rad, j + kernel_rad) = (unsigned char)temp;
        }
    }

    for (size_t i = 0; i < out.rows; i++)
    {
        for (size_t j = 0; j < out.cols; j++)
        {
            out.at<unsigned char>(i, j) *= 3;
        }
    }

    unsigned char max = 0, min = 255;
    for (size_t i = 0; i < out.rows; i++)
    {
        for (size_t j = 0; j < out.cols; j++)
        {
            if (out.at<unsigned char>(i, j) > max)
                max = out.at<unsigned char>(i, j);
            if (out.at<unsigned char>(i, j) < min)
                min = out.at<unsigned char>(i, j);
        }
    }
    cout << "min: " << (int) min << endl;
    cout << "max: " << (int) max << endl;


    imshow("original", gsrc); 
    imshow("out", out);
    //showHist("original_test", &gsrc);
    //showHist("out_test", &out);

    //saveImg("test_orig.jpg", gsrc);
    //saveImg("test_out.jpg", out);

    waitKey();
    return EXIT_SUCCESS;
}


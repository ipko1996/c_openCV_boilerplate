#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <math.h>  
#include <string>
using namespace std;
using namespace cv;

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
}


int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv, "{@input | peppers_sotet.bmp | lena.jpg | input image}");
    Mat src = imread(samples::findFile(parser.get<String>("@input")), IMREAD_COLOR);
    if (src.empty())
    {
        return EXIT_FAILURE;
    }

    Mat gsrc;
    cvtColor(src, gsrc, cv::COLOR_BGR2GRAY);
        
    Mat out = gsrc.clone();
    Mat square_root_img = gsrc.clone();
    Mat square_img = gsrc.clone();

    unsigned char max=0, min=255;
    for (size_t i = 0; i < gsrc.rows; i++)
    {
        for (size_t j = 0; j < gsrc.cols; j++)
        {
            if (gsrc.at<unsigned char>(i, j) > max)
                max = gsrc.at<unsigned char>(i, j);
            if (gsrc.at<unsigned char>(i, j) < min)
                min = gsrc.at<unsigned char>(i, j);
        }
    }

    for (size_t i = 0; i < out.rows; i++)
    {
        for (size_t j = 0; j < out.cols; j++)
        {
            unsigned char x = gsrc.at<unsigned char>(i, j);
            out.at<unsigned char>(i, j) = (unsigned char) (255.0f / (max - min)) * (x - min);
        }
    }

    for (size_t i = 0; i < square_root_img.rows; i++)
    {
        for (size_t j = 0; j < square_root_img.cols; j++)
        {
            unsigned char x = gsrc.at<unsigned char>(i, j);
            unsigned char y = (unsigned char) (255 *sqrt(x / 255.0f));
            square_root_img.at<unsigned char>(i, j) = y;
        }
    }

    for (size_t i = 0; i < square_img.rows; i++)
    {
        for (size_t j = 0; j < square_img.cols; j++)
        {
            unsigned char x = gsrc.at<unsigned char>(i, j);
            unsigned char y = (unsigned char)(255 * pow((x/255.0f),2));
            square_img.at<unsigned char>(i, j) = y;
        }
    }

    showHist("src hist", &gsrc);
    showHist("Square_root hist", &square_root_img);
    showHist("Square image hist", &square_img);
    showHist("Out hist", &out);
      
    imshow("Source image", src);
    imshow("Square_root image", square_root_img);
    imshow("Square image", square_img);
    imshow("Out image", out);

    waitKey();
    return EXIT_SUCCESS;
}


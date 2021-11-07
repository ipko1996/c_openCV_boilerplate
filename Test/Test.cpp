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
    //saveImg("hist_over_exp_" + name + ".jpg", histImage);
}

int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv, "{@input | airplane.bmp | lena.jpg | input image}");
    Mat src = imread(samples::findFile(parser.get<String>("@input")), IMREAD_COLOR);
    if (src.empty())
    {
        return EXIT_FAILURE;
    }

    Mat gsrc;
    cvtColor(src, gsrc, cv::COLOR_BGR2GRAY);
        
    Mat out = gsrc.clone();

    int histogram[256];
    int look_up_table[256];
    for (size_t i = 0; i < 256; i++)
    {
        histogram[i] = 0;
        look_up_table[i] = 0;
    }

    int size = gsrc.rows * gsrc.cols; // N
    int k = 128;
    int dist = (int) size / k;

    for (size_t i = 0; i < gsrc.rows; i++)
    {
        for (size_t j = 0; j < gsrc.cols; j++)
        {
            histogram[gsrc.at<unsigned char>(i, j)]++;
        }
    }

    int sum = 0;
    int i = 1;
    for (size_t j = 0; j < 256; j++)
    {
        sum += histogram[j];
        look_up_table[j] = (unsigned char)(i * 256.0f / k);
        if (sum >= dist) 
        {
            if (histogram[j] > dist) 
                i += 2;
            else
                i++;
            //i++;
            sum = 0;
        }
    }

    for (size_t i = 0; i < gsrc.rows; i++)
    {
        for (size_t j = 0; j < gsrc.cols; j++)
        {
            out.at<unsigned char>(i, j) = look_up_table[out.at<unsigned char>(i, j)];
        }
    }


    imshow("original", gsrc);
    imshow("out", out);
    showHist("original_hist", &gsrc);
    showHist("out_hist", &out);

    waitKey();
    return EXIT_SUCCESS;
}


// Third party libraries
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

namespace binarization {
    cv::Mat binarize_red(cv::Mat image) {
        const int hue_min = 200;
        const int hue_max = 50;
        const int sat_min = 150;
        const int sat_max = 255;
        const int bright_min = 90;
        const int bright_max = 170;
        // Convert RGB input image to HSV format.
        cv::Mat hsv_image;
        cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);
        // Split the 3 HSV channels.
        cv::Mat hsv_channels[3];
        cv::split(hsv_image, hsv_channels);
        // Set red region to 1, using 2 masks for the red colour (around 0).
        cv::Mat red1;
        cv::Mat red2;
        cv::inRange(hsv_image, cv::Scalar(hue_min, sat_min, bright_min),
                    cv::Scalar(255, sat_max, bright_max), red1);
        cv::inRange(hsv_image, cv::Scalar(0, sat_min, bright_min),
                    cv::Scalar(hue_max, sat_max, bright_max), red2);
        cv::Mat binarized = red1 | red2;
        return binarized;
    }
    
    cv::Mat binarize_blue(cv::Mat image) {
        const int hue_min = 110;
        const int hue_max = 120;
        const int sat_min = 80;
        const int sat_max = 180;
        const int bright_min = 40;
        const int bright_max = 130;
        // Convert RGB input image to HSV format.
        cv::Mat hsv_image;
        cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);
        // Split the 3 HSV channels.
        cv::Mat hsv_channels[3];
        cv::split(hsv_image, hsv_channels);
        // Set blue region to 1.
        cv::Mat binarized;
        cv::inRange(hsv_image, cv::Scalar(hue_min, sat_min, bright_min),
                    cv::Scalar(hue_max, sat_max, bright_max), binarized);
        return binarized;
    }
}
    
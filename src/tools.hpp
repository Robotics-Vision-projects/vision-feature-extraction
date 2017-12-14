// Third party libraries
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

using namespace std;

namespace tools {
    cv::Mat draw_shapes(cv::Mat image, vector< vector<cv::Point> > contours,
                        vector< vector<double> > centres, string colour) {
        cv::Scalar shape_colour(255, 255, 255);
        cv::Scalar centre_colour(0, 0, 0);
        if (colour == "red") {
            shape_colour = {0, 0, 255};
            centre_colour = {255, 0, 0};
        }
        else if (colour == "blue") {
            shape_colour = {255, 0, 0};
            centre_colour = {0, 0, 255};
        }
        for (auto index = 0; index < contours.size(); ++index) {
            cout << colour << " centre at (" << centres[index][0] << ", " 
                    << centres[index][1] << ")\n";
            cv::Point centre_point(centres[index][0], centres[index][1]);
            cv::drawContours(image, contours, index, shape_colour, CV_FILLED);
            cv::circle(image, centre_point, 7, centre_colour, -1);
            cv::putText(image, "CENTRE "+to_string(index), centre_point,
                        cv::FONT_HERSHEY_SIMPLEX, 1, centre_colour, 2);
        }
        return image;
    }
}

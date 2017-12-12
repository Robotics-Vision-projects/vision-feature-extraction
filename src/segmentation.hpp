// Third party libraries
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

using namespace std;

namespace segmentation {
    tuple< int, vector< vector<cv::Point> > > get_contours(cv::Mat bin_image) {
        int n_contours = 0;
        vector< vector<cv::Point> > contours;
        // Get the contours, sorted with a hierarchy.
        vector< vector<cv::Point> > contour_points;
        vector<cv::Vec4i> hierarchy;
        cv::findContours(bin_image, contour_points, hierarchy, cv::RETR_CCOMP,
                         cv::CHAIN_APPROX_SIMPLE);
        if (contour_points.size() == 0) {
            return make_tuple(n_contours, contours);
        }
        double min_area = 2000;
        // Filter the obtained contours, keeping only the big ones.
        for(auto index = 0 ; index < contour_points.size(); ++index)
        {
            const vector<cv::Point>& contour = contour_points[index];
            double area = fabs(contourArea(cv::Mat(contour)));
            if (area > min_area) {
                contours.push_back(contour_points[index]);
                n_contours += 1;
            }
        }
        return make_tuple(n_contours, contours);
    }
}

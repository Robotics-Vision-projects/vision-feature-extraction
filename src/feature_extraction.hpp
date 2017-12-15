// Third party libraries
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

using namespace std;

namespace feature_extraction {
    /**
      * Filter the provided shapes, getting only the circles or ellipses.
    **/
    vector< vector<cv::Point> > get_circles(
                vector< vector<cv::Point> > contours) {
        vector< vector<cv::Point> > circles;
        return circles;
    }

    /**
      * Get the centers of the provided shapes, using their moments.
    **/
    vector< vector<double> > get_centers(vector< vector<cv::Point> > contours) {
        vector< vector<double> > centers;
        // Compute moments and calculate center.
        for (auto index = 0; index < contours.size(); ++index) {
            cv::Moments moments = cv::moments(contours[index]);
            vector<double> center = {moments.m10/moments.m00,
                                  moments.m01/moments.m00};
            centers.push_back(center);
        }
        return centers;
    }

    vector< vector<double> > get_interest_points(
            vector< vector<double> > red_centres,
            vector< vector<double> > blue_centres) {
        vector<double> centre;
        vector<double> blue_circles_pos_sum = {0, 0};
        vector< vector<double> > ordered_centres;
        for (auto b_idx = 0; b_idx < blue_centres.size(); ++b_idx) {
            blue_circles_pos_sum[0] += blue_centres[b_idx][0];
            blue_circles_pos_sum[1] += blue_centres[b_idx][1];
        }
        if (blue_centres.size()==3) {
            cout << "using 3 blue circles for centre detection.\n";
            centre = {0, 0};
            vector<double> lengths = {0, 0, 0};
            // Calculate the distances between the 3 circles centres.
            lengths[0] = hypot(blue_centres[0][0] - blue_centres[1][0],
                              blue_centres[0][1] - blue_centres[1][1]);
            lengths[1] = hypot(blue_centres[1][0] - blue_centres[2][0],
                              blue_centres[1][1] - blue_centres[2][1]);
            lengths[2] = hypot(blue_centres[2][0] - blue_centres[0][0],
                              blue_centres[2][1] - blue_centres[0][1]);
            // Find the longest triangle side.
            int maximum = 0;
            int idx_1;
            int idx_2;
            int idx_middle;
            for (auto i = 0; i < lengths.size(); i++)
                if (lengths[i] > maximum) {
                    maximum = lengths[i];
                    idx_1 = i;
                if (i==0){
                    idx_middle = lengths.size()-1;
                    idx_2 = i + 1;
                }
                else if (i==lengths.size()-1) {
                    idx_middle = i - 1;
                    idx_2 = 0;
                }
                else {
                    idx_middle = i - 1;
                    idx_2 = i + 1;
                }
                }
            // The marker centre is the middle point between the 2 farthest away
            // circles.
            centre[0] = (blue_centres[idx_1][0]+blue_centres[idx_2][0]) / 2;
            centre[1] = (blue_centres[idx_1][1]+blue_centres[idx_2][1]) / 2;
            ordered_centres.push_back(centre);
            ordered_centres.push_back(blue_centres[idx_middle]);
            // Order the triangle vertices
            if (blue_centres[idx_middle][0] < centre[0]) {
                if (blue_centres[idx_1][1] < blue_centres[idx_2][1]) {
                    ordered_centres.push_back(blue_centres[idx_1]);
                    ordered_centres.push_back(blue_centres[idx_2]);
                }
                else {
                    ordered_centres.push_back(blue_centres[idx_2]);
                    ordered_centres.push_back(blue_centres[idx_1]);
                }
            }
            else {
                if (blue_centres[idx_1][1] >= blue_centres[idx_2][1]) {
                    ordered_centres.push_back(blue_centres[idx_1]);
                    ordered_centres.push_back(blue_centres[idx_2]);
                }
                else {
                    ordered_centres.push_back(blue_centres[idx_2]);
                    ordered_centres.push_back(blue_centres[idx_1]);
                }
            }
        } 
        else if (red_centres.size()==1 && blue_centres.size()==3) {
            centre = {0, 0};
            vector<double> total_sum = {0, 0};
            total_sum[0] = blue_circles_pos_sum[0] + red_centres[0][0];
            total_sum[1] = blue_circles_pos_sum[1] + red_centres[0][1];
            centre[0] = total_sum[0] / 4;
            centre[1] = total_sum[1] / 4;
            ordered_centres.push_back(centre);
        }
        else {
            cout << "Not a valid number of circles!\n"
                    << red_centres.size() << " red circles and "
                    << blue_centres.size() << " blue circles\n";
        }
        return ordered_centres;
    }

    tuple<vector<cv::KeyPoint>, cv::Mat> get_surf_keypoints(cv::Mat image) {
        // Detect the keypoints in the input image.
        // Hessian threshold
        int hessian = 400;
        vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        cv::Ptr<cv::xfeatures2d::SURF> detector =
                cv::xfeatures2d::SURF::create(hessian);
        // detector->detect(image, keypoints);
        detector->detectAndCompute(image, cv::Mat(), keypoints, descriptors);
        return make_tuple(keypoints, descriptors);
    }

    tuple<vector<cv::Point2f>, cv::Mat> get_marker_corners(cv::Mat ref_img,
            cv::Mat image, cv::Mat result_img, vector<cv::Point2f> object,
            vector<cv::Point2f> scene, bool show) {
        // Get homography, using RANSAC algorithm
        cv::Mat H = findHomography(object, scene, CV_RANSAC);
        //-- Get the corners from the scene image
        vector<cv::Point2f> obj_corners(4);
        obj_corners[0] = cvPoint(0,0); 
        obj_corners[1] = cvPoint(ref_img.cols, 0);
        obj_corners[2] = cvPoint(ref_img.cols, ref_img.rows);
        obj_corners[3] = cvPoint(0, ref_img.rows);
        vector<cv::Point2f> corners(4);
        cv::perspectiveTransform(obj_corners, corners, H);
        for (auto idx = 0; idx < 4; ++idx) {
            cout << "Point " << idx << ": ";
            cout << "(" << corners[idx].x << ", " << corners[idx].y << ")\n";
        }
        //-- Draw lines between the corners
        if (show == true) {
            cv::line(result_img, corners[0]+cv::Point2f(ref_img.cols, 0),
                     corners[1]+cv::Point2f(ref_img.cols, 0),
                     cv::Scalar(0, 255, 0), 4);
            cv::line(result_img, corners[1]+cv::Point2f(ref_img.cols, 0),
                     corners[2]+cv::Point2f(ref_img.cols, 0),
                     cv::Scalar(0, 255, 0), 4);
            cv::line(result_img, corners[2]+cv::Point2f(ref_img.cols, 0),
                     corners[3]+cv::Point2f(ref_img.cols, 0),
                     cv::Scalar(0, 255, 0), 4);
            cv::line(result_img, corners[3]+cv::Point2f(ref_img.cols, 0),
                     corners[0]+cv::Point2f(ref_img.cols, 0),
                     cv::Scalar(0, 255, 0), 4);
        }
        return make_tuple(corners, result_img);
    }
}

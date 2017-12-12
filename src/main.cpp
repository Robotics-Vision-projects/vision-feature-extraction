// Standard libraries
#include <chrono>
#include <iostream>
// Third party libraries
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv.hpp>
// Local libraries
#include "binarization.hpp"
#include "feature_extraction.hpp"
#include "segmentation.hpp"
#include "tools.hpp"

using namespace std;


tuple<cv::Mat, double> marker_colour_pipeline(cv::Mat image, double success,
                                              bool show) {
    cv::Mat shapes(image.rows, image.cols, CV_8UC3, cv::Scalar(0,0,0));
    // Get the blue circles position.
    cv::Mat blue_bins = binarization::binarize_blue(image);
    auto blues = segmentation::get_contours(blue_bins);
    int n_blues = get<0>(blues);
    vector< vector<cv::Point> > blue_contours = get<1>(blues);
    vector< vector<double> > blue_cntrs =
            feature_extraction::get_centers(blue_contours);
    // Get the red circles position.
    cv::Mat red_bins = binarization::binarize_red(image);
    auto reds = segmentation::get_contours(red_bins);
    int n_reds = get<0>(reds);
    vector< vector<cv::Point> > red_contours = get<1>(reds);
    vector< vector<double> > red_cntrs =
            feature_extraction::get_centers(red_contours);
    // Get the marker centre.
    vector<double> marker_centre = 
            feature_extraction::get_marker_centre(red_cntrs, blue_cntrs);
    // Draw the found circles on the shapes image
    if (show == true) {
        shapes = tools::draw_shapes(shapes, red_contours, red_cntrs, "red");
        shapes = tools::draw_shapes(shapes, blue_contours, blue_cntrs, "blue");
    }
    // Draw the marker centre on the shapes image.
    if (marker_centre.size() == 2) {
        success += 1;
        cout << "Marker centre at (" << marker_centre[0] << ", "
                << marker_centre[1] << ")\n";
        if (show == true) {
            cv::Point centre_point(marker_centre[0], marker_centre[1]);
            cv::Scalar centre_colour = {255, 255, 255};
            cv::circle(shapes, centre_point, 12, centre_colour, -1);
        }
    }
    return make_tuple(shapes, success);
}


tuple<cv::Mat, double> marker_corn_pipeline(cv::Mat image, double success,
                                            cv::Mat ref_descriptors,
                                            vector<cv::KeyPoint> ref_keypoints,
                                            cv::Mat ref_image, bool show) {
    cv::Mat result_img;
    auto surf_result = feature_extraction::get_surf_keypoints(image);
    vector<cv::KeyPoint> keypoints = get<0>(surf_result);
    cv::Mat descriptors = get<1>(surf_result);
    // Matching descriptor vectors with a brute force matcher
    cv::BFMatcher matcher(cv::NORM_L2);
    // vector<cv::DMatch> matches;
    // matcher.match(ref_descriptors, descriptors, matches);
    // Sort the matches using the k-means algorithm, with 2 clusters.
    vector< vector<cv::DMatch> > matches;
    matcher.knnMatch(ref_descriptors, descriptors, matches, 2);
    // Filter using Lowe's ratio
    vector<cv::DMatch> good_matches;
    const float ratio = 0.8;
    for (auto idx = 0; idx < matches.size(); ++idx) {
        if (matches[idx][0].distance < ratio * matches[idx][1].distance) {
            good_matches.push_back(matches[idx][0]);
        }
    }
    // Object localization.
    vector<cv::Point2f> object;
    vector<cv::Point2f> scene;
    for(auto i = 0; i < good_matches.size(); i++) {
        //-- Get the keypoints from the good matches
        object.push_back(ref_keypoints[good_matches[i].queryIdx].pt);
        scene.push_back(keypoints[good_matches[i].trainIdx].pt);
    }

    // Draw matches
    if (show == true) {
        cv::drawMatches(ref_image, ref_keypoints, image, keypoints,
                        good_matches, result_img);
        result_img = feature_extraction::get_marker_corners(ref_image, image,
                result_img, object, scene);
    }
    return make_tuple(result_img, success);
}


int main(int argc, char** argv)
{
    string marker = argv[1];
    string set = argv[2];
    string show_images = argv[3];
    // Establish the markers to be used, and thus the algorithm
    string marker_name;
    string ref_img_name;
    if (marker == "colour") {
        marker_name = "color";
    }
    else if (marker == "corn") {
        marker_name = "corny";
        ref_img_name = "../resources/marker_corny/reference.png";
    }
    else {
        cout << "Invalid argument...\nvision_feature_extraaction colour/corn"
                 << " easy/hard show\n";
        return 0;
    }
    // Establish the total samples and files names depending on the user input.
    string base_name;
    int samples;
    if (set=="hard") {
        samples = 52;
        base_name = "../resources/marker_" + marker_name + "_hard/marker_" +
                    marker_name + "_hard_";
    }
    else if (set=="easy") {
        samples = 30;
        base_name = "../resources/marker_" + marker_name + "/marker_" + 
                    marker_name + "_";
    }
    else {
        cout << "Invalid argument...\nvision_feature_extraaction easy/hard\n";
        return 0;
    }
    // Decide whether to show the result images depending on the user input.
    bool show = false;
    if (show_images == "show") {
        show = true;
    }
    // Start measuring time, and initialize the puzzle.
    chrono::high_resolution_clock::time_point t_i =
            chrono::high_resolution_clock::now();
    double success = 0;
    vector<cv::KeyPoint> ref_keypoints;
    cv::Mat ref_descriptors;
    cv::Mat ref_image;
    if (marker == "corn") {
        ref_image = cv::imread(ref_img_name);
        auto ref_surf = feature_extraction::get_surf_keypoints(ref_image);
        ref_keypoints = get<0>(ref_surf);
        ref_descriptors = get<1>(ref_surf);
    }
    for (auto index = 1; index <= samples; ++index) {
        string image_name = base_name;
        if (index < 10) {
            image_name += "0";
        }
        cout << "Image " << index << "\n";
        image_name += to_string(index) + ".png";
        cv::Mat image = cv::imread(image_name, cv::IMREAD_COLOR);
        // Image for plotting the results.
        cv::Mat result_img;
        if (marker == "colour") {
            auto result = marker_colour_pipeline(image, success, show);
            result_img = get<0>(result);
            success = get<1>(result);
        }
        else if (marker == "corn") {
            auto result = marker_corn_pipeline(image, success, ref_descriptors,
                                               ref_keypoints, ref_image, show);
            result_img = get<0>(result);
            success = get<1>(result);            
        }
        // Show segmented image.
        if (show == true) {
            cv::namedWindow("Image "+to_string(index), cv::WINDOW_NORMAL);
            cv::imshow("Image "+to_string(index), result_img);
            cv::waitKey(0);
            cv::destroyWindow("Image "+to_string(index));
        }
    }
    // Stop measuring time.
    chrono::high_resolution_clock::time_point t_f =
            chrono::high_resolution_clock::now();
    auto t_tot = chrono::duration_cast<chrono::milliseconds>(t_f - t_i).count();
    // Calculate the success percentage
    double rate = (success/samples) * 100;
    cout << "Success rate: " << rate << "%\n";
    cout << "Elapsed time: " << t_tot << " milliseconds.\n";
    return 0;
}

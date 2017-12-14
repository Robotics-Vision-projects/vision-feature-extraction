
using namespace std;

namespace geometry {
    vector< vector<double> > order_centers(vector< vector<double> > centers) {
        vector< vector<double> > ordered_centers;
        if (centers.size()!=3) {
            return ordered_centers;
        }
        vector<double> lengths = {0, 0, 0};
        // Calculate the distances between the 3 circles centres.
        lengths[0] = hypot(centers[0][0] - centers[1][0],
                           centers[0][1] - centers[1][1]);
        lengths[1] = hypot(centers[1][0] - centers[2][0],
                           centers[1][1] - centers[2][1]);
        lengths[2] = hypot(centers[2][0] - centers[0][0],
                           centers[2][1] - centers[0][1]);
        // Find the longest triangle side.
        int maximum = 0;
        int idx_middle;
        int idx_1;
        int idx_2;
        for (auto i = 0; i < lengths.size(); i++) {
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
        } 
        vector<float> dx = {centers[idx_1][0] - centers[idx_2][0];
        vector<float> dy = centers[idx_1][1] - centers[idx_2][1];
        float angle1 = atan2(lengths[idx_middle], lengths[idx_2]);
        cout << "First angle: " << angle1 << " rads\n";
        // dx = centers[idx_2][0] - centers[idx_middle][0];
        // dy = centers[idx_2][1] - centers[idx_middle][1];
        // float angle2 = atan2(dy, dx);
        // cout << "Second angle: " << angle2 << " rads\n";
        return ordered_centers;
    }
}

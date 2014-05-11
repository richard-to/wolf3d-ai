#include <opencv2/nonfree/nonfree.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;

const char kWindowName[] = "SIFT Matches";
const char kDoorImage[] = "images/door.png";
const char kSceneImage[] = "images/scene4.png";

int main(int argc, char *argv[])
{
    initModule_nonfree();

    Mat doorImage = imread(kDoorImage);
    Mat sceneImage = imread(kSceneImage);

    Ptr<FeatureDetector> detector = FeatureDetector::create("SIFT");
    vector<KeyPoint> doorKeypoints;
    vector<KeyPoint> sceneKeypoints;
    detector->detect(doorImage, doorKeypoints);
    detector->detect(sceneImage, sceneKeypoints);

    Ptr<DescriptorExtractor> descriptorExtractor = DescriptorExtractor::create("SIFT");
    Mat doorDescriptors;
    Mat sceneDescriptors;
    descriptorExtractor->compute(doorImage, doorKeypoints, doorDescriptors);
    descriptorExtractor->compute(sceneImage, sceneKeypoints, sceneDescriptors);    

    Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create("BruteForce");
    vector<DMatch> matches;
    descriptorMatcher->match(doorDescriptors, sceneDescriptors, matches);

    namedWindow(kWindowName, WINDOW_AUTOSIZE);
    Mat imgMatches;
    drawMatches(doorImage, doorKeypoints, sceneImage, sceneKeypoints, matches, imgMatches);
    imshow(kWindowName, imgMatches);
    waitKey(0);
}
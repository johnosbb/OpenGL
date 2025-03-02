#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cv;
using namespace cv::face;
using namespace std;

int main()
{
    // Initialize the LBPH face recognizer
    Ptr<LBPHFaceRecognizer> model = LBPHFaceRecognizer::create();

    // Prepare training data: image paths and corresponding labels
    vector<Mat> images;
    vector<int> labels;
    vector<String> imagePaths = {"face1.jpg", "face2.jpg", "face3.jpg"}; // Example image paths
    vector<int> personLabels = {0, 1, 2};                                // Example labels for each person

    // Load images and labels
    for (size_t i = 0; i < imagePaths.size(); i++)
    {
        Mat img = imread(imagePaths[i], IMREAD_GRAYSCALE);
        if (img.empty())
        {
            cout << "Error loading image: " << imagePaths[i] << endl;
            continue;
        }
        images.push_back(img);
        labels.push_back(personLabels[i]);
    }

    // Train the model
    model->train(images, labels);

    // Save the trained model to file
    model->save("face_recognition_model.xml");

    cout << "Training complete!" << endl;
    return 0;
}

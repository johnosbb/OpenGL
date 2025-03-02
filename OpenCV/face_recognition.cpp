#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <iostream>

using namespace cv;
using namespace cv::face;
using namespace std;

int main()
{
    // Load the trained model
    Ptr<LBPHFaceRecognizer> model = LBPHFaceRecognizer::create();
    model->read("face_recognition_model.xml");

    // Load the unknown image for recognition
    Mat unknown_image = imread("unknown_face.jpg", IMREAD_GRAYSCALE);
    if (unknown_image.empty())
    {
        cout << "Error loading image!" << endl;
        return -1;
    }

    // Recognize the face
    int label = -1;
    double confidence = 0.0;
    model->predict(unknown_image, label, confidence);

    // Print the recognized label and confidence
    cout << "Predicted label: " << label << ", Confidence: " << confidence << endl;

    // Display the result
    if (label != -1)
    {
        cout << "Face recognized!" << endl;
    }
    else
    {
        cout << "Unknown face!" << endl;
    }

    return 0;
}

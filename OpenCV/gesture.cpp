#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Error: Cannot open webcam" << endl;
        return -1;
    }

    // Load multiple Haar cascade models
    CascadeClassifier rpalm_cascade, lpalm_cascade, fist_cascade, left_cascade, right_cascade, palm_cascade;

    if (!rpalm_cascade.load("rpalm.xml"))
    {
        cout << "Error loading rpalm.xml" << endl;
        return -1;
    }
    if (!lpalm_cascade.load("lpalm.xml"))
    {
        cout << "Error loading lpalm.xml" << endl;
        return -1;
    }
    if (!fist_cascade.load("fist.xml"))
    {
        cout << "Error loading fist.xml" << endl;
        return -1;
    }
    if (!left_cascade.load("left.xml"))
    {
        cout << "Error loading left.xml" << endl;
        return -1;
    }
    if (!right_cascade.load("right.xml"))
    {
        cout << "Error loading right.xml" << endl;
        return -1;
    }
    if (!palm_cascade.load("palm.xml"))
    {
        cout << "Error loading palm.xml" << endl;
        return -1;
    }

    Mat frame, gray;
    vector<Rect> rpalm, lpalm, fists, left_swipe, right_swipe, palm;

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Detect different gestures
        rpalm_cascade.detectMultiScale(gray, rpalm, 1.1, 3, 0, Size(50, 50));
        lpalm_cascade.detectMultiScale(gray, lpalm, 1.1, 3, 0, Size(50, 50));
        fist_cascade.detectMultiScale(gray, fists, 1.3, 5, 0, Size(100, 100)); // 1.1 scaling factor, A smaller scaling factor (like 1.05) means more detailed checking (more levels), while a larger value (like 1.3) checks fewer levels but is faster. 3 = min neighbors - Higher values make the detector more selective, meaning fewer false positives but more missed detections. Lower values make the detector more sensitive and allow more detections, but this could also lead to false positives (detecting something that’s not actually a fist). Min Size -This defines the minimum size (width, height) of objects that the detector will consider.  Any detected object smaller than 30x30 pixels will be ignored. This helps in filtering out noise (tiny irrelevant objects).
        left_cascade.detectMultiScale(gray, left_swipe, 1.1, 3, 0, Size(50, 50));
        right_cascade.detectMultiScale(gray, right_swipe, 1.1, 3, 0, Size(50, 50));
        palm_cascade.detectMultiScale(gray, palm, 1.3, 6, 0, Size(100, 100));
        // Draw rectangles for gestures
        for (const auto &r : rpalm)
        {
            rectangle(frame, r, Scalar(0, 255, 0), 2);
            cout << "Right Palm Detected" << endl;
        }
        for (const auto &l : lpalm)
        {
            rectangle(frame, l, Scalar(0, 255, 255), 2);
            cout << "Left Palm Detected" << endl;
        }
        for (const auto &f : fists)
        {
            rectangle(frame, f, Scalar(255, 0, 0), 2);
            cout << "Fist Detected" << endl;
        }
        for (const auto &ls : left_swipe)
        {
            rectangle(frame, ls, Scalar(255, 255, 0), 2);
            cout << "Left Swipe Detected - Switching KVM Left" << endl;
            // system("xdotool key ctrl+alt+Left");
        }
        for (const auto &rs : right_swipe)
        {
            rectangle(frame, rs, Scalar(255, 0, 255), 2);
            cout << "Right Swipe Detected - Switching KVM Right" << endl;
            // system("xdotool key ctrl+alt+Right");
        }
        for (const auto &rs : palm)
        {
            rectangle(frame, rs, Scalar(255, 0, 255), 2);
            cout << "Palm Detected - Stopping KVM Right" << endl;
            // system("xdotool key ctrl+alt+Right");
        }

        imshow("Multi-Gesture Detection", frame);
        if (waitKey(30) == 'q')
            break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}

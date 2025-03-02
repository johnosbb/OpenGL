#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
    // declares all required variables
    Rect roi;
    Mat frame;

    // create a tracker object
    // Ptr<Tracker> tracker = TrackerKCF::create();
    Ptr<Tracker> tracker = TrackerCSRT::create();

    // open the default camera (0)
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cerr << "Error: Could not open webcam" << endl;
        return -1;
    }

    // get the first frame from the webcam
    cap >> frame;
    if (frame.empty())
    {
        cerr << "Error: Could not capture frame" << endl;
        return -1;
    }

    // select the bounding box
    roi = selectROI("tracker", frame);

    // quit if ROI was not selected
    if (roi.width == 0 || roi.height == 0)
        return 0;

    // initialize the tracker
    tracker->init(frame, roi);

    // perform the tracking process
    printf("Start the tracking process, press ESC to quit.\n");
    for (;;)
    {
        // get frame from the webcam
        cap >> frame;
        if (frame.empty())
            break;

        // Convert to grayscale for better contrast
        cvtColor(frame, frame, COLOR_BGR2GRAY);

        // Apply histogram equalization to improve contrast
        equalizeHist(frame, frame);

        // Convert back to color (trackers expect a 3-channel image)
        cvtColor(frame, frame, COLOR_GRAY2BGR);

        bool ok = tracker->update(frame, roi);
        if (!ok)
        {
            cout << "Tracking failure detected!" << endl;
        }
        if (1)
        {
            // Define the vertical counting line at x = 200
            int countingLineX = 200;

            // Get the center of the tracked object
            Point objectCenter(roi.x + roi.width / 2, roi.y + roi.height / 2);

            // Check if the object crosses the vertical line
            if (objectCenter.x > countingLineX - 5 && objectCenter.x < countingLineX + 5)
            {
                cout << "Object crossed the vertical line!" << endl;
                static int count = 0;
                count++;
                cout << "Count: " << count << endl;
            }

            // Draw the vertical counting line
            line(frame, Point(countingLineX, 0), Point(countingLineX, frame.rows), Scalar(0, 255, 0), 2);
        }
        // draw the tracked object
        rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);

        // show image with the tracked object
        imshow("tracker", frame);

        // quit on ESC button
        if (waitKey(1) == 27)
            break;
    }

    return 0;
}

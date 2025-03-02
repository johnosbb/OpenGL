#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main()
{
    // Open the webcam (default camera is 0)
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Error: Could not open camera!" << endl;
        return -1;
    }

    // Window name
    string windowName = "Press 'c' to Capture Image";

    // Set up a window to show the webcam feed
    namedWindow(windowName, WINDOW_NORMAL);

    // Variable to store the captured image
    Mat frame;
    string nameTag = "";

    // Loop to keep the webcam window open
    while (true)
    {
        // Capture the frame from the webcam
        cap >> frame;

        // If the frame is empty, break the loop
        if (frame.empty())
        {
            cout << "Error: No frame captured!" << endl;
            break;
        }

        // Display the frame
        imshow(windowName, frame);

        // Wait for user input
        char key = (char)waitKey(1);

        // If the user presses 'c', capture the image and assign the label
        if (key == 'c')
        {
            cout << "Enter the name label: ";
            getline(cin, nameTag); // Read the name label from the user

            // Draw the label on the image
            putText(frame, nameTag, Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2, LINE_AA);

            // Save the image with the label
            string filename = nameTag + "_captured_image.jpg";
            imwrite(filename, frame);
            cout << "Image saved as " << filename << endl;

            // Show the image with the label
            imshow("Captured Image", frame);

            // Wait until a key is pressed and break the loop
            waitKey(0);
            break;
        }

        // If the user presses 'q', quit the program
        if (key == 'q')
        {
            cout << "Quitting program!" << endl;
            break;
        }
    }

    // Release the webcam and close all OpenCV windows
    cap.release();
    destroyAllWindows();
    return 0;
}

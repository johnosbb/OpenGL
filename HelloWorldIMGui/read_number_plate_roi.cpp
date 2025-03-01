#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <tesseract/baseapi.h>

cv::Rect selection;       // Rect to store the selected region
cv::Point origin;         // Origin of the rectangle (mouse down position)
bool isSelecting = false; // Flag to indicate selection status

// Mouse callback function
void onMouse(int event, int x, int y, int flags, void *userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        origin = cv::Point(x, y); // Store mouse down position
        isSelecting = true;
    }
    else if (event == cv::EVENT_MOUSEMOVE)
    {
        if (isSelecting)
        {
            selection = cv::Rect(origin, cv::Point(x, y)); // Update rectangle while dragging
        }
    }
    else if (event == cv::EVENT_LBUTTONUP)
    {
        isSelecting = false;                           // End selection on mouse release
        selection = cv::Rect(origin, cv::Point(x, y)); // Finalize selection
    }
}

void showImageWithROI(cv::Mat &img)
{
    cv::Mat imgCopy = img.clone();
    if (selection.width > 0 && selection.height > 0)
    {
        cv::rectangle(imgCopy, selection, cv::Scalar(0, 255, 0), 2); // Draw the selected rectangle
    }
    cv::imshow("Select License Plate Region", imgCopy);
}

void performOCR(cv::Mat &img)
{
    // Crop the selected region for OCR
    if (selection.width > 0 && selection.height > 0)
    {
        cv::Mat plate_img = img(selection); // Crop image using selected rectangle

        // Convert the cropped region to grayscale
        cv::Mat plate_gray;
        cv::cvtColor(plate_img, plate_gray, cv::COLOR_BGR2GRAY);

        // Threshold to improve OCR results
        cv::Mat plate_thresh;
        cv::threshold(plate_gray, plate_thresh, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

        // Initialize tesseract and perform OCR
        tesseract::TessBaseAPI ocr;
        ocr.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY); // English language and LSTM mode
        ocr.SetImage(plate_thresh.data, plate_thresh.cols, plate_thresh.rows, 1, plate_thresh.cols);
        std::string extracted_text = std::string(ocr.GetUTF8Text());

        // Output the OCR result
        std::cout << "Extracted text: " << extracted_text << std::endl;
    }
}

int main()
{
    // Load the image
    cv::Mat img = cv::imread("test_image.png"); // Replace with your image path
    if (img.empty())
    {
        std::cerr << "Error loading image!" << std::endl;
        return -1;
    }

    // Set up the mouse callback to allow ROI selection
    cv::namedWindow("Select License Plate Region");
    cv::setMouseCallback("Select License Plate Region", onMouse, nullptr);

    // Loop to display the image and allow user to select ROI
    while (true)
    {
        showImageWithROI(img);

        // Wait for key press
        char key = cv::waitKey(1);

        if (key == 27)
        { // Escape key to exit
            break;
        }
        if (key == 13)
        { // Enter key to trigger OCR
            performOCR(img);
        }
    }

    return 0;
}

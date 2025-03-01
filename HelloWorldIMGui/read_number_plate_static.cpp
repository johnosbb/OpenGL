#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <vector>
#include <iostream>

int main()
{
    // Load image
    cv::Mat img = cv::imread("test_image.png");
    if (img.empty())
    {
        std::cerr << "Error: Could not load test image!" << std::endl;
        return -1;
    }

    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Apply Bilateral Filter (Noise Reduction)
    cv::Mat bfilter;
    cv::bilateralFilter(gray, bfilter, 11, 17, 17);

    // Apply Canny Edge Detection
    cv::Mat edged;
    cv::Canny(bfilter, edged, 30, 200);

    // Debug: Show the edged image
    cv::imshow("Edged Image", edged);
    cv::waitKey(0);

    // Find Contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edged, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // Debug: Draw all contours
    cv::Mat contourImg = img.clone();
    cv::Mat debugImg = img.clone(); // Make a copy of the input image

    // Draw contours
    cv::drawContours(debugImg, contours, -1, cv::Scalar(0, 255, 0), 2);

    // Draw bounding boxes around contours
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::Rect rect = cv::boundingRect(contours[i]);
        cv::rectangle(debugImg, rect, cv::Scalar(255, 0, 0), 2); // Blue bounding boxes
    }

    cv::imshow("All Contours & Bounding Boxes", debugImg);
    cv::waitKey(0);

    // Sort contours by area (keep the 10 largest)
    std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b)
              { return cv::contourArea(a) > cv::contourArea(b); });

    contours.resize(contours.size());
    // Debug: Draw top 10 contours
    cv::Mat topContoursImg = img.clone();
    for (size_t i = 0; i < std::min(contours.size(), (size_t)10); i++)
    {
        cv::drawContours(topContoursImg, contours, (int)i, cv::Scalar(255, 0, 0), 2);
    }
    cv::imshow("Top 10 Contours", topContoursImg);
    cv::waitKey(0);

    cv::Rect plateRect;
    // Try filtering by aspect ratio
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::Rect rect = cv::boundingRect(contours[i]);
        double aspectRatio = (double)rect.width / rect.height;

        if (aspectRatio > 2.0 && aspectRatio < 5.5 && rect.area() > 200)
        {
            // Draw the potential plate contour
            cv::Mat filteredImg = img.clone();
            cv::rectangle(filteredImg, rect, cv::Scalar(0, 0, 255), 3);
            cv::imshow("Potential License Plate", filteredImg);
            cv::waitKey(0);

            plateRect = rect;
            break; // Exit once we find a good candidate
        }
    }

    // Extract and show the license plate
    cv::Mat plate;
    if (plateRect.area() > 0)
    {
        plate = img(plateRect);
        cv::imshow("License Plate", plate);
        cv::waitKey(0);
    }
    else
    {
        std::cerr << "No license plate detected!" << std::endl;
        return -1;
    }

    // Initialize Tesseract OCR
    tesseract::TessBaseAPI ocr;
    if (ocr.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY))
    {
        std::cerr << "Error initializing Tesseract!" << std::endl;
        return -1;
    }
    ocr.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    ocr.SetImage(plate.data, plate.cols, plate.rows, 3, plate.step);

    // Perform OCR
    std::string text = ocr.GetUTF8Text();
    std::cout << "Detected Number Plate: " << text << std::endl;

    // Overlay text on the original image
    cv::putText(img, text, cv::Point(plateRect.x, plateRect.y - 10),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);

    // Show final result
    cv::imshow("Detected Plate", img);
    cv::waitKey(0);

    return 0;
}

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <iostream>

int main()
{
    // Load Haar cascade for license plate detection
    cv::CascadeClassifier plate_cascade;
    if (!plate_cascade.load("./haarcascade_russian_plate_number.xml"))
    {
        std::cerr << "Error loading Haar cascade!\n";
        return -1;
    }

    // Open webcam
    cv::VideoCapture cap(0); // 0 for default webcam
    if (!cap.isOpened())
    {
        std::cerr << "Error opening webcam\n";
        return -1;
    }

    // Initialize Tesseract OCR
    tesseract::TessBaseAPI tess;
    if (tess.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY))
    {
        std::cerr << "Error initializing Tesseract\n";
        return -1;
    }
    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

    while (true)
    {

        cv::Mat frame, gray;
        cap >> frame; // Capture frame
        if (frame.empty())
            break;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); // Convert to grayscale

        // Detect plates
        std::vector<cv::Rect> plates;
        plate_cascade.detectMultiScale(gray, plates, 1.1, 10, 0, cv::Size(30, 30));

        for (const auto &plate : plates)
        {
            cv::rectangle(frame, plate, cv::Scalar(0, 255, 0), 2);

            // Extract and preprocess the plate region
            cv::Mat plateROI = gray(plate);
            cv::Mat processedPlate;
            cv::threshold(plateROI, processedPlate, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

            // OCR Processing
            tess.SetImage(processedPlate.data, processedPlate.cols, processedPlate.rows, 1, processedPlate.step);
            std::string plateText = tess.GetUTF8Text();
            std::cout << "Detected Plate: " << plateText << std::endl;

            // Display extracted text on frame
            cv::putText(frame, plateText, cv::Point(plate.x, plate.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0), 2);
        }

        // Show output frame
        cv::imshow("Number Plate Detection", frame);
        if (cv::waitKey(30) == 27)
            break; // Exit on 'Esc' key
    }

    // Cleanup
    tess.End();
    cap.release();
    cv::destroyAllWindows();
    return 0;
}

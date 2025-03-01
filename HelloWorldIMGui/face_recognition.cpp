#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

// Function to find the OpenCV window using its title
Window findOpenCVWindow(Display *display, const std::string &windowName)
{
    Window root = DefaultRootWindow(display);
    Window parent;
    Window *children;
    unsigned int numChildren;

    // Get the list of windows
    if (XQueryTree(display, root, &root, &parent, &children, &numChildren) == 0)
    {
        return 0;
    }

    Window foundWindow = 0;
    for (unsigned int i = 0; i < numChildren; i++)
    {
        char *winName = NULL;
        if (XFetchName(display, children[i], &winName) > 0)
        {
            if (winName && windowName == winName)
            {
                foundWindow = children[i];
                XFree(winName);
                break;
            }
            XFree(winName);
        }
    }

    if (children)
    {
        XFree(children);
    }

    return foundWindow;
}

// Function to remove decorations from the OpenCV window
void removeWindowDecorations(const std::string &windowName)
{
    Display *display = XOpenDisplay(NULL);
    if (!display)
    {
        std::cerr << "Cannot open X display\n";
        return;
    }

    Window win = findOpenCVWindow(display, windowName);
    if (!win)
    {
        std::cerr << "Could not find window: " << windowName << "\n";
        XCloseDisplay(display);
        return;
    }

    Atom wmHints = XInternAtom(display, "_MOTIF_WM_HINTS", True);
    if (wmHints != None)
    {
        struct
        {
            unsigned long flags;
            unsigned long functions;
            unsigned long decorations;
            long inputMode;
            unsigned long status;
        } hints = {2, 0, 0, 0, 0};

        XChangeProperty(display, win, wmHints, wmHints, 32, PropModeReplace, (unsigned char *)&hints, 5);
    }

    XCloseDisplay(display);
}

int main()
{
    // Load the face cascade classifier
    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load("haarcascade_frontalface_default.xml"))
    {
        std::cerr << "Error loading Haar cascade file!\n";
        return -1;
    }

    // Load the image
    cv::Mat img = cv::imread("face.png");
    if (img.empty())
    {
        std::cerr << "Error loading image!\n";
        return -1;
    }

    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Equalize histogram to improve contrast
    cv::equalizeHist(gray, gray);

    // Detect faces
    std::vector<cv::Rect> faces;
    face_cascade.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(30, 30));

    // Draw rectangles around faces
    for (const auto &face : faces)
    {
        cv::rectangle(img, face, cv::Scalar(0, 255, 0), 2);
    }

    // Show the image

    cv::imshow("Face Detection", img);

    // Remove decorations
    removeWindowDecorations("Face Detection");

    cv::waitKey(0); // Wait for a key press
    return 0;
}

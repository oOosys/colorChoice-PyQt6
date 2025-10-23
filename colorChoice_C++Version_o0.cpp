/*								oOosys (Grok)						     2025-10-23_22:16
    colorChoice_C++Version_o0.cpp   translated from Python PyQt6 script to C++ by Grok
	How does the compiled version being an ELF executable compare to running the same
		functionality from Python script? 
	In tests using   tinyBenchmarkQt_Cpp-vs-Py.sh   and appropriate 'executables' it 
		seems that the compiled C++ version is more then 20 times the Python code
			 in size with a startup time comparable to Python's .

 Compile on Linux with GCC assuming Qt6 and OpenCV are installed : 
	^--	source file code size	:  22 kByte 
~ $ g++ colorChoice_C++Version_o0.cpp -o colorChoiceQt  -fPIC -O2						-I/usr/include/x86_64-linux-gnu/qt6     -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets   -I/usr/include/x86_64-linux-gnu/qt6/QtGui       -I/usr/include/x86_64-linux-gnu/qt6/QtCore      -lQt6Widgets -lQt6Gui -lQt6Core         `pkg-config --cflags --libs opencv4`    -std=c++17
	^-- 92 kByte executable
~ $ g++ colorChoice_C++Version_o0.cpp -o colorChoiceQt  -fPIC -Os   -s -Wl,--as-needed	-I/usr/include/x86_64-linux-gnu/qt6     -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets   -I/usr/include/x86_64-linux-gnu/qt6/QtGui       -I/usr/include/x86_64-linux-gnu/qt6/QtCore      -lQt6Widgets -lQt6Gui -lQt6Core         `pkg-config --cflags --libs opencv4`    -std=c++17
	^-- 59 kByte executable
**/

 // For webcolors equivalent: Hardcoded CSS3 color names (147 colors) as in webcolors library.
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtCore/QRect>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/Qt>

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include <fstream>
#include <array>
#include <algorithm>

// Configuration presets (from Python script)
bool fullscreenMode = true; // true for fullscreen, false otherwise
bool withOutputOfClosestColorName = true; // true to output color name
std::string iniFileName = "colorChoiceQt.ini"; // Adjusted for C++ executable name

// Preset values
int firstSliderValue = 254;
int secondSliderValue = 127;
int thirdSliderValue = 97;
std::string colorMode = "RGB";
int activeChannelIndex = 0;

// UI constants
const std::string wID = "[f]:Fullscreen  ||  nextActiveSlider:[Tab], [Left/Right/Up/Down], [PageUp/Down]   ||   [r]/[h]/[l]/[a] -> RGB/HSV/HLS/Lab   ||   Enter/Click to print (r,g,b) to stdout and EXIT.";
int windowXpos = 30;
int windowYpos = 10;
int windowWidth = 1890;
int windowHeight = 1050;
int widthColorStripColor = 7;
int heightColorStrip = 130;
int verticalSpacingBetweenStrips = 12;
int topStripsOffset = 590;
int topMargin = 10;
int bottomMargin = 10;
int leftMargin = 30;
int rightMargin = 30;
int sliderPositionMarkerLineWidth = 3;
cv::Scalar sliderPositionMarkerColor(0, 0, 0);
std::map<std::string, cv::Scalar> activeSliderPositionMarkerColor = {
    {"RGB", cv::Scalar(255, 0, 0)}, // RED (BGR in OpenCV, but we'll adjust)
    {"HSV", cv::Scalar(0, 255, 255)}, // YELLOW
    {"HLS", cv::Scalar(255, 255, 0)}, // CYAN
    {"Lab", cv::Scalar(0, 255, 0)} // GREEN
};
int sliderChangeOnPageUpDown = 10;
int sliderChangeOnUpDown = 5;
int offsetX = 30;
int offsetY = 30;

// Color strips (vectors of RGB tuples)
std::vector<cv::Vec3b> FirstSliderColors(256);
std::vector<cv::Vec3b> SecondSliderColors(256);
std::vector<cv::Vec3b> ThirdSliderColors(256);

// Web colors map (name to RGB) - CSS3 colors from webcolors
std::map<std::string, cv::Vec3b> webColorMap = {
    {"aliceblue", {240,248,255}}, {"antiquewhite", {250,235,215}}, {"aqua", {0,255,255}},
    {"aquamarine", {127,255,212}}, {"azure", {240,255,255}}, {"beige", {245,245,220}},
    {"bisque", {255,228,196}}, {"black", {0,0,0}}, {"blanchedalmond", {255,235,205}},
    {"blue", {0,0,255}}, {"blueviolet", {138,43,226}}, {"brown", {165,42,42}},
    {"burlywood", {222,184,135}}, {"cadetblue", {95,158,160}}, {"chartreuse", {127,255,0}},
    {"chocolate", {210,105,30}}, {"coral", {255,127,80}}, {"cornflowerblue", {100,149,237}},
    {"cornsilk", {255,248,220}}, {"crimson", {220,20,60}}, {"cyan", {0,255,255}},
    {"darkblue", {0,0,139}}, {"darkcyan", {0,139,139}}, {"darkgoldenrod", {184,134,11}},
    {"darkgray", {169,169,169}}, {"darkgreen", {0,100,0}}, {"darkgrey", {169,169,169}},
    {"darkkhaki", {189,183,107}}, {"darkmagenta", {139,0,139}}, {"darkolivegreen", {85,107,47}},
    {"darkorange", {255,140,0}}, {"darkorchid", {153,50,204}}, {"darkred", {139,0,0}},
    {"darksalmon", {233,150,122}}, {"darkseagreen", {143,188,143}}, {"darkslateblue", {72,61,139}},
    {"darkslategray", {47,79,79}}, {"darkslategrey", {47,79,79}}, {"darkturquoise", {0,206,209}},
    {"darkviolet", {148,0,211}}, {"deeppink", {255,20,147}}, {"deepskyblue", {0,191,255}},
    {"dimgray", {105,105,105}}, {"dimgrey", {105,105,105}}, {"dodgerblue", {30,144,255}},
    {"firebrick", {178,34,34}}, {"floralwhite", {255,250,240}}, {"forestgreen", {34,139,34}},
    {"fuchsia", {255,0,255}}, {"gainsboro", {220,220,220}}, {"ghostwhite", {248,248,255}},
    {"gold", {255,215,0}}, {"goldenrod", {218,165,32}}, {"gray", {128,128,128}},
    {"green", {0,128,0}}, {"greenyellow", {173,255,47}}, {"grey", {128,128,128}},
    {"honeydew", {240,255,240}}, {"hotpink", {255,105,180}}, {"indianred", {205,92,92}},
    {"indigo", {75,0,130}}, {"ivory", {255,255,240}}, {"khaki", {240,230,140}},
    {"lavender", {230,230,250}}, {"lavenderblush", {255,240,245}}, {"lawngreen", {124,252,0}},
    {"lemonchiffon", {255,250,205}}, {"lightblue", {173,216,230}}, {"lightcoral", {240,128,128}},
    {"lightcyan", {224,255,255}}, {"lightgoldenrodyellow", {250,250,210}}, {"lightgray", {211,211,211}},
    {"lightgreen", {144,238,144}}, {"lightgrey", {211,211,211}}, {"lightpink", {255,182,193}},
    {"lightsalmon", {255,160,122}}, {"lightseagreen", {32,178,170}}, {"lightskyblue", {135,206,250}},
    {"lightslategray", {119,136,153}}, {"lightslategrey", {119,136,153}}, {"lightsteelblue", {176,196,222}},
    {"lightyellow", {255,255,224}}, {"lime", {0,255,0}}, {"limegreen", {50,205,50}},
    {"linen", {250,240,230}}, {"magenta", {255,0,255}}, {"maroon", {128,0,0}},
    {"mediumaquamarine", {102,205,170}}, {"mediumblue", {0,0,205}}, {"mediumorchid", {186,85,211}},
    {"mediumpurple", {147,112,219}}, {"mediumseagreen", {60,179,113}}, {"mediumslateblue", {123,104,238}},
    {"mediumspringgreen", {0,250,154}}, {"mediumturquoise", {72,209,204}}, {"mediumvioletred", {199,21,133}},
    {"midnightblue", {25,25,112}}, {"mintcream", {245,255,250}}, {"mistyrose", {255,228,225}},
    {"moccasin", {255,228,181}}, {"navajowhite", {255,222,173}}, {"navy", {0,0,128}},
    {"oldlace", {253,245,230}}, {"olive", {128,128,0}}, {"olivedrab", {107,142,35}},
    {"orange", {255,165,0}}, {"orangered", {255,69,0}}, {"orchid", {218,112,214}},
    {"palegoldenrod", {238,232,170}}, {"palegreen", {152,251,152}}, {"paleturquoise", {175,238,238}},
    {"palevioletred", {219,112,147}}, {"papayawhip", {255,239,213}}, {"peachpuff", {255,218,185}},
    {"peru", {205,133,63}}, {"pink", {255,192,203}}, {"plum", {221,160,221}},
    {"powderblue", {176,224,230}}, {"purple", {128,0,128}}, {"rebeccapurple", {102,51,153}},
    {"red", {255,0,0}}, {"rosybrown", {188,143,143}}, {"royalblue", {65,105,225}},
    {"saddlebrown", {139,69,19}}, {"salmon", {250,128,114}}, {"sandybrown", {244,164,96}},
    {"seagreen", {46,139,87}}, {"seashell", {255,245,238}}, {"sienna", {160,82,45}},
    {"silver", {192,192,192}}, {"skyblue", {135,206,235}}, {"slateblue", {106,90,205}},
    {"slategray", {112,128,144}}, {"slategrey", {112,128,144}}, {"snow", {255,250,250}},
    {"springgreen", {0,255,127}}, {"steelblue", {70,130,180}}, {"tan", {210,180,140}},
    {"teal", {0,128,128}}, {"thistle", {216,191,216}}, {"tomato", {255,99,71}},
    {"turquoise", {64,224,208}}, {"violet", {238,130,238}}, {"wheat", {245,222,179}},
    {"white", {255,255,255}}, {"whitesmoke", {245,245,245}}, {"yellow", {255,255,0}},
    {"yellowgreen", {154,205,50}}
};

// Load settings from .ini
void loadSettings() {
    std::ifstream iniFile(iniFileName);
    if (iniFile.is_open()) {
        std::string line;
        while (std::getline(iniFile, line)) {
            // Parse simple format from Python exec
            // Assuming format like: firstSliderValue, secondSliderValue, thirdSliderValue = 254, 127, 97
            size_t pos = line.find("firstSliderValue");
            if (pos != std::string::npos) {
                sscanf(line.c_str(), "firstSliderValue, secondSliderValue, thirdSliderValue = %d, %d, %d", &firstSliderValue, &secondSliderValue, &thirdSliderValue);
            }
            pos = line.find("colorMode");
            if (pos != std::string::npos) {
                sscanf(line.c_str(), "colorMode = '%3s'", colorMode.data());
            }
            pos = line.find("activeChannelIndex");
            if (pos != std::string::npos) {
                sscanf(line.c_str(), "activeChannelIndex = %d", &activeChannelIndex);
            }
        }
        iniFile.close();
    } else {
        // Default if no file
        firstSliderValue = 254;
        secondSliderValue = 127;
        thirdSliderValue = 97;
        colorMode = "RGB";
        activeChannelIndex = 0;
    }
}

// Save settings to .ini
void saveSettings() {
    std::ofstream iniFile(iniFileName);
    if (iniFile.is_open()) {
        iniFile << "firstSliderValue, secondSliderValue, thirdSliderValue = " << firstSliderValue << ", " << secondSliderValue << ", " << thirdSliderValue << "\n";
        iniFile << "colorMode = '" << colorMode << "'\n";
        iniFile << "activeChannelIndex = " << activeChannelIndex << "\n";
        iniFile.close();
    }
}

// Update slider colors
void updateSliderColors() {
    int colorConversionFlag = -1;
    if (colorMode == "RGB") {
        for (int tmp = 0; tmp < 256; ++tmp) {
            FirstSliderColors[tmp] = {static_cast<uchar>(thirdSliderValue), static_cast<uchar>(secondSliderValue), static_cast<uchar>(tmp)}; // BGR for OpenCV, but we'll use as RGB
            SecondSliderColors[tmp] = {static_cast<uchar>(thirdSliderValue), static_cast<uchar>(tmp), static_cast<uchar>(firstSliderValue)};
            ThirdSliderColors[tmp] = {static_cast<uchar>(tmp), static_cast<uchar>(secondSliderValue), static_cast<uchar>(firstSliderValue)};
        }
        return;
    } else if (colorMode == "HLS") {
        colorConversionFlag = cv::COLOR_HLS2RGB;
    } else if (colorMode == "HSV") {
        colorConversionFlag = cv::COLOR_HSV2RGB;
    } else if (colorMode == "Lab") {
        colorConversionFlag = cv::COLOR_Lab2RGB;
    }

    for (int tmp = 0; tmp < 256; ++tmp) {
        cv::Mat color(1, 1, CV_8UC3, cv::Scalar(tmp, secondSliderValue, thirdSliderValue));
        cv::cvtColor(color, color, colorConversionFlag);
        FirstSliderColors[tmp] = color.at<cv::Vec3b>(0, 0);

        color = cv::Mat(1, 1, CV_8UC3, cv::Scalar(firstSliderValue, tmp, thirdSliderValue));
        cv::cvtColor(color, color, colorConversionFlag);
        SecondSliderColors[tmp] = color.at<cv::Vec3b>(0, 0);

        color = cv::Mat(1, 1, CV_8UC3, cv::Scalar(firstSliderValue, secondSliderValue, tmp));
        cv::cvtColor(color, color, colorConversionFlag);
        ThirdSliderColors[tmp] = color.at<cv::Vec3b>(0, 0);
    }
}

// Get RGB from current values
cv::Vec3b getCurrentRGB() {
    int colorConversionFlag = -1;
    if (colorMode == "RGB") {
        return {static_cast<uchar>(thirdSliderValue), static_cast<uchar>(secondSliderValue), static_cast<uchar>(firstSliderValue)}; // BGR order for consistency
    } else if (colorMode == "HLS") {
        colorConversionFlag = cv::COLOR_HLS2RGB;
    } else if (colorMode == "HSV") {
        colorConversionFlag = cv::COLOR_HSV2RGB;
    } else if (colorMode == "Lab") {
        colorConversionFlag = cv::COLOR_Lab2RGB;
    }

    cv::Mat color(1, 1, CV_8UC3, cv::Scalar(firstSliderValue, secondSliderValue, thirdSliderValue));
    cv::cvtColor(color, color, colorConversionFlag);
    return color.at<cv::Vec3b>(0, 0);
}

// Exact or closest web color name
std::string exactOrClosestWebColorName(cv::Vec3b rgb) {
    uchar b = rgb[0], g = rgb[1], r = rgb[2]; // OpenCV is BGR
    for (const auto& pair : webColorMap) {
        if (pair.second[0] == b && pair.second[1] == g && pair.second[2] == r) {
            return "'WebColorName: " + pair.first + " (exact match)'";
        }
    }
    // Closest
    double min_dist = std::numeric_limits<double>::infinity();
    std::string closestName;
    cv::Vec3b closestRGB;
    for (const auto& pair : webColorMap) {
        double dist = std::sqrt(std::pow(pair.second[2] - r, 2) + std::pow(pair.second[1] - g, 2) + std::pow(pair.second[0] - b, 2));
        if (dist < min_dist) {
            min_dist = dist;
            closestName = pair.first;
            closestRGB = pair.second;
        }
    }
    return "'WebColorName: " + closestName + " (closest match) rgb(" + std::to_string(closestRGB[2]) + "," + std::to_string(closestRGB[1]) + "," + std::to_string(closestRGB[0]) + ")'";
}

// Print color and exit
void printColorChoiceToStdout_andEXIT() {
    cv::Vec3b bgr = getCurrentRGB();
    int r = bgr[2], g = bgr[1], b = bgr[0];
    std::string hex = "#" + std::string(1, "0123456789ABCDEF"[(r >> 4) & 0xF]) + std::string(1, "0123456789ABCDEF"[r & 0xF]) +
                           std::string(1, "0123456789ABCDEF"[(g >> 4) & 0xF]) + std::string(1, "0123456789ABCDEF"[g & 0xF]) +
                           std::string(1, "0123456789ABCDEF"[(b >> 4) & 0xF]) + std::string(1, "0123456789ABCDEF"[b & 0xF]);
    if (withOutputOfClosestColorName) {
        std::cout << hex << " --> rgb(" << r << "," << g << "," << b << ") --> " << exactOrClosestWebColorName(bgr) << std::endl;
    } else {
        std::cout << "rgb( " << r << ", " << g << ", " << b << " )  -->  " << hex << std::endl;
    }
    saveSettings();
    QApplication::quit();
}

class ColorChoice : public QWidget {
public:
    ColorChoice(QWidget *parent = nullptr) : QWidget(parent), fullscreen(fullscreenMode) {
        setWindowTitle(QString::fromStdString(wID));
        setGeometry(windowXpos, windowYpos, windowWidth, windowHeight);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        if (fullscreen) {
            painter.translate(offsetX, offsetY);
        }
        painter.fillRect(rect(), QColor(230, 255, 230));
        painter.setPen(QPen(QColor(0, 0, 0), 1));
        painter.drawRect(1, 1,
                         widthColorStripColor * 256 + leftMargin + rightMargin - 2,
                         topStripsOffset + bottomMargin + 3 * (heightColorStrip + verticalSpacingBetweenStrips) - 2);

        std::array<std::vector<cv::Vec3b>*, 3> channels = {&FirstSliderColors, &SecondSliderColors, &ThirdSliderColors};
        std::array<int, 3> values = {firstSliderValue, secondSliderValue, thirdSliderValue};

        for (int channelIndex = 0; channelIndex < 3; ++channelIndex) {
            int yOffset = channelIndex * (heightColorStrip + verticalSpacingBetweenStrips);
            for (int colorIndex = 0; colorIndex < 256; ++colorIndex) {
                cv::Vec3b col = (*channels[channelIndex])[colorIndex];
                painter.setBrush(QColor(col[2], col[1], col[0])); // RGB
                painter.setPen(Qt::NoPen);
                painter.drawRect(leftMargin + colorIndex * widthColorStripColor,
                                 topStripsOffset + yOffset,
                                 widthColorStripColor,
                                 heightColorStrip);
            }
            QColor markerColor;
            int penSize;
            if (activeChannelIndex == channelIndex) {
                cv::Scalar ac = activeSliderPositionMarkerColor[colorMode];
                markerColor = QColor(ac[2], ac[1], ac[0]); // RGB
                penSize = 7;
            } else {
                markerColor = QColor(sliderPositionMarkerColor[2], sliderPositionMarkerColor[1], sliderPositionMarkerColor[0]);
                penSize = 4;
            }
            painter.setPen(QPen(markerColor, penSize));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(leftMargin + values[channelIndex] * widthColorStripColor - sliderPositionMarkerLineWidth,
                             topStripsOffset + yOffset - sliderPositionMarkerLineWidth,
                             widthColorStripColor + 2 * sliderPositionMarkerLineWidth,
                             heightColorStrip + 2 * sliderPositionMarkerLineWidth);
        }

        cv::Vec3b current = FirstSliderColors[firstSliderValue];
        painter.setBrush(QColor(current[2], current[1], current[0]));
        painter.setPen(Qt::NoPen);
        painter.drawRect(leftMargin, topMargin,
                         256 * widthColorStripColor,
                         topStripsOffset - 15 - topMargin);
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            int mouseOffsetX = fullscreen ? offsetX : 0;
            int mouseOffsetY = fullscreen ? offsetY : 0;
            int x = event->pos().x() - mouseOffsetX;
            int y = event->pos().y() - mouseOffsetY;

            if (topMargin <= y && y <= topStripsOffset - 15 && leftMargin <= x && x <= leftMargin + 256 * widthColorStripColor) {
                printColorChoiceToStdout_andEXIT();
            } else {
                int strip_height = heightColorStrip;
                int strip_width = 256 * widthColorStripColor;
                int yOffset0 = topStripsOffset + 0 * (strip_height + verticalSpacingBetweenStrips);
                int yOffset1 = topStripsOffset + 1 * (strip_height + verticalSpacingBetweenStrips);
                int yOffset2 = topStripsOffset + 2 * (strip_height + verticalSpacingBetweenStrips);

                if (yOffset0 <= y && y <= yOffset0 + strip_height && leftMargin <= x && x <= leftMargin + strip_width) {
                    firstSliderValue = std::max(0, std::min(255, (x - leftMargin) / widthColorStripColor));
                } else if (yOffset1 <= y && y <= yOffset1 + strip_height && leftMargin <= x && x <= leftMargin + strip_width) {
                    secondSliderValue = std::max(0, std::min(255, (x - leftMargin) / widthColorStripColor));
                } else if (yOffset2 <= y && y <= yOffset2 + strip_height && leftMargin <= x && x <= leftMargin + strip_width) {
                    thirdSliderValue = std::max(0, std::min(255, (x - leftMargin) / widthColorStripColor));
                }
                updateSliderColors();
                update();
            }
        }
    }

    void keyPressEvent(QKeyEvent *event) override {
        int key = event->key();
        std::string text = QString(QChar(event->text()[0])).toLower().toStdString();

        if (key == Qt::Key_Enter || key == Qt::Key_Return) {
            printColorChoiceToStdout_andEXIT();
        } else if (key == Qt::Key_Escape) {
            QApplication::quit();
        } else if (key == Qt::Key_Tab) {
            activeChannelIndex = (activeChannelIndex + 1) % 3;
            update();
        } else if (text == "r") {
            if (colorMode == "RGB") return;
            convertToMode("RGB");
            update();
        } else if (text == "h") {
            if (colorMode == "HSV") return;
            convertToMode("HSV");
            update();
        } else if (text == "l") {
            if (colorMode == "HLS") return;
            convertToMode("HLS");
            update();
        } else if (text == "a") {
            if (colorMode == "Lab") return;
            convertToMode("Lab");
            update();
        } else if (text == "f") {
            if (!fullscreen) {
                fullscreen = true;
                showFullScreen();
            }
        } else if (key == Qt::Key_Up) {
            firstSliderValue = std::min(255, firstSliderValue + sliderChangeOnUpDown);
            secondSliderValue = std::min(255, secondSliderValue + sliderChangeOnUpDown);
            thirdSliderValue = std::min(255, thirdSliderValue + sliderChangeOnUpDown);
            updateSliderColors();
            update();
        } else if (key == Qt::Key_Down) {
            firstSliderValue = std::max(0, firstSliderValue - sliderChangeOnUpDown);
            secondSliderValue = std::max(0, secondSliderValue - sliderChangeOnUpDown);
            thirdSliderValue = std::max(0, thirdSliderValue - sliderChangeOnUpDown);
            updateSliderColors();
            update();
        } else if (key == Qt::Key_Left) {
            changeActiveSlider(-1);
            updateSliderColors();
            update();
        } else if (key == Qt::Key_Right) {
            changeActiveSlider(1);
            updateSliderColors();
            update();
        } else if (key == Qt::Key_PageUp) {
            changeActiveSlider(-sliderChangeOnPageUpDown);
            updateSliderColors();
            update();
        } else if (key == Qt::Key_PageDown) {
            changeActiveSlider(sliderChangeOnPageUpDown);
            updateSliderColors();
            update();
        }
    }

private:
    bool fullscreen;

    void changeActiveSlider(int delta) {
        if (activeChannelIndex == 0) {
            firstSliderValue = std::max(0, std::min(255, firstSliderValue + delta));
        } else if (activeChannelIndex == 1) {
            secondSliderValue = std::max(0, std::min(255, secondSliderValue + delta));
        } else if (activeChannelIndex == 2) {
            thirdSliderValue = std::max(0, std::min(255, thirdSliderValue + delta));
        }
    }

    void convertToMode(const std::string& newMode) {
        // Get current RGB
        cv::Vec3b rgb = getCurrentRGB();

        // Convert RGB to new mode
        int flagToNew = -1;
        if (newMode == "RGB") {
            firstSliderValue = rgb[2]; // R
            secondSliderValue = rgb[1]; // G
            thirdSliderValue = rgb[0]; // B
        } else if (newMode == "HLS") {
            flagToNew = cv::COLOR_RGB2HLS;
        } else if (newMode == "HSV") {
            flagToNew = cv::COLOR_RGB2HSV;
        } else if (newMode == "Lab") {
            flagToNew = cv::COLOR_RGB2Lab;
        }

        if (flagToNew != -1) {
            cv::Mat color(1, 1, CV_8UC3, cv::Scalar(rgb[0], rgb[1], rgb[2])); // BGR to RGB? Wait, cvtColor expects BGR input for some
            cv::cvtColor(color, color, flagToNew);
            cv::Vec3b newVal = color.at<cv::Vec3b>(0, 0);
            firstSliderValue = newVal[0];
            secondSliderValue = newVal[1];
            thirdSliderValue = newVal[2];
        }

        colorMode = newMode;
        updateSliderColors();
    }
};

int main(int argc, char *argv[]) {
    loadSettings();
    updateSliderColors();

    QApplication app(argc, argv);
    ColorChoice window;
    if (fullscreenMode) {
        window.showFullScreen();
    } else {
        window.show();
    }
    return app.exec();
}
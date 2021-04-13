//
//  main.cpp
//  1C_task_
//
//  Created by Александр on 13.04.2021.
//

#include <iostream>
#include <vector>
#include <deque>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "lines_intersects.hpp"

#define w(x) std::cout << #x << ": " << x << "\n"

int height = 0;
int width  = 0;
int chan   = 0;

class Color {
public:
    Color(uchar r = 0, uchar g = 0, uchar b = 0) : r(r), g(g), b(b) {}
    
    Color& operator=(const Color& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }
    
    bool near(const Color& other, int eps = 10) {
        int delta = std::abs(r - other.r) + std::abs(g - other.g) + std::abs(b - other.b);
        return delta <= eps;
    }
    
    int r, g, b;
};

struct RGB {
    uchar blue;
    uchar green;
    uchar red;
};

bool can_go_to(int i, int j, std::vector<std::vector<Color>> &points, std::vector<std::vector<int>> &used) {
    if (i < 0 || j < 0 || i > height || j > width) {
        return false;
    }
    return points[i][j].near(Color(0, 0, 0)) && used[i][j] == -1;
}

int main(int argc, const char* argv[]) {

    cv::Mat image = cv::imread("/Users/alex/Desktop/test.png");
    
    const int pixels_thin = 11;
    
    height = image.rows;
    width  = image.cols;
    chan   = image.channels();
    
    w(height);
    w(width);
    w(chan);
    
    std::vector<std::vector<Color>> points(height, std::vector<Color>(width));
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            cv::Vec3b intensity = image.at<cv::Vec3b>(i, j);
            uchar blue = intensity.val[0];
            uchar green = intensity.val[1];
            uchar red = intensity.val[2];
            
            points[i][j] = Color(red, green, blue);
        }
    }
    
    std::vector<std::pair<int, int>> mids;
    std::vector<std::pair<int, int>> vertexes;
    
//    std::deque<std::pair<int, int>> que;
    std::vector<std::vector<int>> used(height, std::vector<int>(width, -1));
    for (int x_1 = 0; x_1 < height; ++x_1) {
        for (int y_1 = 0; y_1 < width; ++y_1) {
            
            if (!can_go_to(x_1, y_1, points, used)) {
                continue;
            }
            vertexes.push_back({x_1, y_1});
            int x_etalon = -1;
            int y_etalon = -1;
            
            std::deque<std::pair<int, int>> que;
            que.push_back({x_1, y_1});
            used[x_1][y_1] = 1;
            
            while (!que.empty()) {
                int size = int(que.size());
                int x_accume = 0;
                int y_accume = 0;
                
                std::tie(x_etalon, y_etalon) = que.front();
                int counted = 0;
                
                
                for (int k = 0; k < size; ++k) {
                    auto [i, j] = que.front();
                    que.pop_front();
                    
                    if (std::abs(x_etalon - i) + std::abs(y_etalon - j) > pixels_thin) {
                        continue;
                    }
                    counted++;
                    
                    if (can_go_to(i + 1, j, points, used)) {
                        x_accume += i + 1;
                        y_accume += j;
                        que.push_back({i + 1, j});
                        used[i + 1][j] = 1;
                    }
                    if (can_go_to(i - 1, j, points, used)) {
                        x_accume += i - 1;
                        y_accume += j;
                        que.push_back({i - 1, j});
                        used[i - 1][j] = 1;
                    }
                    if (can_go_to(i, j + 1, points, used)) {
                        x_accume += i;
                        y_accume += j + 1;
                        que.push_back({i, j + 1});
                        used[i][j + 1] = 1;
                    }
                    if (can_go_to(i, j - 1, points, used)) {
                        x_accume += i;
                        y_accume += j - 1;
                        que.push_back({i, j - 1});
                        used[i][j - 1] = 1;
                    }
                }
                
                int x_mid = x_accume / counted;
                int y_mid = y_accume / counted;
                assert(counted > 0);
                
                mids.push_back({x_mid, y_mid});
                
            }
            
            vertexes.push_back({x_etalon, y_etalon});
            
        }
        
    }
    
    std::vector<std::pair<int, int>> good_points;
    uchar c1 = 0;
    
    for (auto [x, y] : mids) {
        if (x < 0 || y < 0 || x >= height || y >= width) {
            continue;
        }
        if (points[x][y].near(Color(255, 255, 255))) {
            continue;
        }
        c1 += 10;
        
        good_points.push_back({x, y});
        RGB& rgb = image.ptr<RGB>(x)[y];
        rgb.red = c1;
        rgb.green = 255;
        rgb.blue = 0;
    }
    
    
    int vertexes_sz = int(vertexes.size());
    std::vector<bool> can_use(vertexes_sz, true);
    
    for (int i = 0; i < vertexes_sz; ++i) {
        for (int j = 0; j < vertexes_sz; ++j) {
            for (int k = i + 1; k < vertexes_sz; ++k) {
                
                if (i == j || j == k) {
                    continue;
                }
                
                auto [x1, y1] = vertexes[i];
                auto [x2, y2] = vertexes[j];
                auto [x3, y3] = vertexes[k];
                
                double l1 = std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
                double l2 = std::sqrt(std::pow(x3 - x2, 2) + std::pow(y3 - y2, 2));
                double l  = std::sqrt(std::pow(x1 - x3, 2) + std::pow(y1 - y3, 2));
                
                if (std::abs(l1 + l2 - l) < 0.005 * l) {
                    can_use[j] = false;
                }
            }
        }
    }
    
    std::vector<std::pair<int, int>> result;
    for (int i = 0; i < vertexes_sz; ++i) {
        if (!can_use[i]) {
            continue;
        }
        result.push_back(vertexes[i]);
    }
    
    
    for (auto [x, y] : result) {

        RGB& rgb = image.ptr<RGB>(x)[y];
        rgb.red = 255;
        rgb.green = 0;
        rgb.blue = 0;
    }
    
    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", image);
    cv::waitKey();
    
    return 0;
}

#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\photo.hpp>
#include <iostream>
#include <map>
#include <QDebug>
#include <QString>


// https://stackoverflow.com/a/34734939/5008845
void reduceColor_kmeans_old(const cv::Mat& src, cv::Mat& dst)
{
    int K = 6;
    int n = src.rows * src.cols;
    cv::Mat data = src.reshape(1, n);
    data.convertTo(data, CV_32F);

    std::vector<int> labels;
    cv::Mat1f colors;
    kmeans(data, K, labels, cv::TermCriteria(), 1, cv::KMEANS_PP_CENTERS, colors);

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < src.channels(); ++j)
            data.at<float>(i, j) = colors(labels[i], j);

    cv::Mat reduced = data.reshape(src.channels(), src.rows);
    reduced.convertTo(dst, src.type());
}

void reduceColor_kmeans(const cv::Mat& src, cv::Mat& dst, int K_clusters = 10)
{
    std::vector<cv::Vec4b> points;
    std::vector<cv::Point> locations;
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            if (src.at<cv::Vec4b>(y, x)[3] != 0) {
                points.push_back(src.at<cv::Vec4b>(y, x));
                locations.push_back(cv::Point(x, y));
            }
        }
    }
    cv::Mat kmeanPoints(points.size(), 4, CV_32F);
    for (int y = 0; y < points.size(); y++) {
        for (int z = 0; z < 4; z++) {
            kmeanPoints.at<float>(y, z) = points[y][z];
        }
    }

    cv::Mat labels;
    cv::Mat centers;
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1.0);
    cv::kmeans(kmeanPoints, K_clusters, labels, criteria, 3, cv::KMEANS_PP_CENTERS, centers);

    dst = cv::Mat(src);
    cv::Vec4b tempColor;
    for (int i = 0; i < locations.size(); i++) {
        int cluster_idx = labels.at<int>(i, 0);
        tempColor[0] = centers.at<float>(cluster_idx, 0);
        tempColor[1] = centers.at<float>(cluster_idx, 1);
        tempColor[2] = centers.at<float>(cluster_idx, 2);
        tempColor[3] = centers.at<float>(cluster_idx, 3);
        dst.at<cv::Vec4b>(locations[i]) = tempColor;
    }
}

struct lessVec4b
{
    bool operator()(const cv::Vec4b& lhs, const cv::Vec4b& rhs) const {
        return (lhs[0] != rhs[0]) ? (lhs[0] < rhs[0]) : ((lhs[1] != rhs[1]) ? (lhs[1] < rhs[1]) : (lhs[2] < rhs[2]));
    }
};

std::map<cv::Vec4b, int, lessVec4b> getPalette(const cv::Mat& src)
{
    std::map<cv::Vec4b, int, lessVec4b> palette;
    for (int r = 0; r < src.rows; ++r)
    {
        for (int c = 0; c < src.cols; ++c)
        {
            cv::Vec4b color = src.at<cv::Vec4b>(r, c);
            if(color[3] != 0)
                if (palette.count(color) == 0)
                {
                    palette[color] = 1;
                }
                else
                {
                    palette[color] = palette[color] + 1;
                }
        }
    }
    return palette;
}

struct palette_output {
    std::map<cv::Vec4b, int, lessVec4b> palette;
    int area;
    cv::Mat reduced_img;
};

palette_output palette_extractor(cv::Mat& img, int K_clusters = 10)
{
    //Mat img = imread(path.toStdString(), IMREAD_UNCHANGED);
    cv::Size img_size = img.size();

    int target_size = 300;
    double scale = img_size.width > img_size.width ? float(target_size) / img_size.width : float(target_size) / img_size.height;
    cv::resize(img, img, cv::Size(0, 0), scale, scale);

    if(img.channels() == 3)
        cv::cvtColor(img, img, cv::COLOR_BGR2BGRA, 4);
    if (img.channels() == 1)
        cv::cvtColor(img, img, cv::COLOR_GRAY2BGRA, 4);
    // Reduce color
    cv::Mat reduced;
    reduceColor_kmeans(img, reduced, K_clusters);

    // Get palette
    std::map<cv::Vec4b, int, lessVec4b> palette = getPalette(reduced);
    int area = img.rows * img.cols;

    return { palette, area, reduced};
}
#pragma once
#include <QPixmap>
#include <QString>
#include <QColor>
#include <QList>
#include <random>
#include "colorPaletteExtractor.h"

int randint(int start, int stop);
double randfloat(double start, double stop);
color_area& get_weighted_random_color(QList<color_area>& colors);
QPixmap openImage(QString path);
double get_luminance(int& R, int& G, int& B);
double get_luminance(QColor& color);
QColor complementary_color(QColor& color);

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return select_randomly(start, end, gen);
}
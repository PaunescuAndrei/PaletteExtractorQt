#include "utils.h"
#include <qDebug>

int randint(int start, int stop) {
	std::random_device seed;
	std::mt19937 gen{ seed() };
	std::uniform_int_distribution<int> distribution(start, stop);
	return distribution(gen);
}

double randfloat(double start, double stop)
{
	std::random_device seed;
	std::mt19937 gen{ seed() };
	std::uniform_real_distribution<> distribution(start, stop);
	return distribution(gen);
}

color_area& get_weighted_random_color(QList<color_area>& colors) {
	double sum_of_weight = 0;
	for (auto& c : colors) {
		sum_of_weight += c.area_percent;
	}
	double rnd = randfloat(0, sum_of_weight);
	for (auto& c : colors) {
		if (rnd < c.area_percent)
			return c;
		rnd -= c.area_percent;
	}
	assert(!"should never get here");
	return colors.first();
}

QPixmap openImage(QString path) {
	QPixmap img = QPixmap(path);
	return img;
}

double get_luminance(int &R, int &G, int &B) {
	const double rg = R <= 10 ? R / 3294.0 : std::pow((R / 269.0) + 0.0513, 2.4);
	const double gg = G <= 10 ? G / 3294.0 : std::pow((G / 269.0) + 0.0513, 2.4);
	const double bg = B <= 10 ? B / 3294.0 : std::pow((B / 269.0) + 0.0513, 2.4);
	return (0.2126 * rg) + (0.7152 * gg) + (0.0722 * bg);
}

double get_luminance(QColor& color) {
	int R, G, B;
	color.getRgb(&R, &G, &B);
	const double rg = R <= 10 ? R / 3294.0 : std::pow((R / 269.0) + 0.0513, 2.4);
	const double gg = G <= 10 ? G / 3294.0 : std::pow((G / 269.0) + 0.0513, 2.4);
	const double bg = B <= 10 ? B / 3294.0 : std::pow((B / 269.0) + 0.0513, 2.4);
	return (0.2126 * rg) + (0.7152 * gg) + (0.0722 * bg);
}

QColor complementary_color(QColor& color) {
	double Luminance = get_luminance(color);
	if (Luminance <= 0.35)
		return QColor(Qt::white);
	else
		return QColor(Qt::black);
}
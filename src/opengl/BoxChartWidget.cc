#include "opengl/BoxChartWidget.hh"

#include <algorithm>
#include <numbers>

#include "data/Stats.hh"
#include "opengl/AngledMultiText.hh"
#include "util/Ex.hh"

using namespace std;

void BoxChartWidget::setWhiskerStyle(const Style *s) { whiskerStyle = s; }

void BoxChartWidget::setBoxStyle(const Style *s) { boxStyle = s; }

void BoxChartWidget::setBoxWidth(double width) { boxWidth = width; }

void BoxChartWidget::setBoxColors(vector<glm::vec4> &colors) {
  boxColors = colors;
}

void BoxChartWidget::setWhiskerColors(vector<glm::vec4> &colors) {
  whiskerColors = colors;
}

void BoxChartWidget::setOutlineColors(vector<glm::vec4> &colors) {
  outlineColors = colors;
}

void BoxChartWidget::setPointsPerBox(int n) { pointsPerBox = n; }

void BoxChartWidget::setData(const vector<double> &data) { this->data = data; }

void BoxChartWidget::setNames(const ::vector<std::string> &names) {
  this->names = names;
}

void BoxChartWidget::createXAxis(AxisType a) {
  xAxisType = a;

  StyledMultiShape2D *mnew = c->addLayer(new StyledMultiShape2D(c, xAxisStyle));
  MultiText *tnew = c->addLayer(new MultiText(c, xAxisTextStyle));

  switch (a) {
    case LINEAR: {
      cout << "a box chart can't have a linear x axis\n";
      throw Ex1(Errcode::BAD_ARGUMENT);
    }; break;

    case LOGARITHMIC: {
      cout << "a box chart can't have a logarithmic x axis\n";
      throw Ex1(Errcode::BAD_ARGUMENT);
    }; break;

    case TEXT: {
      xAxis = new TextAxisWidget(mnew, tnew, x, y, w, h);
      xAxis->setTickLabels(names);
    }; break;
  }
}

void BoxChartWidget::createYAxis(AxisType a) {
  yAxisType = a;

  StyledMultiShape2D *rot90 = c->addLayer(
      new StyledMultiShape2D(c, yAxisStyle, numbers::pi / 2, x - w, y + h));
  AngledMultiText *t90 = c->addLayer(
      new AngledMultiText(c, yAxisTextStyle, numbers::pi / 2, x - w, y + h));

  switch (a) {
    case LINEAR: {
      yAxis = new LinearAxisWidget(rot90, t90, 0, 0, h, w);
    }; break;

    case LOGARITHMIC: {
      cout << "a box chart can't have a logarithmic y axis\n";
      throw Ex1(Errcode::BAD_ARGUMENT);
    }; break;

    case TEXT: {
      cout << "a box chart can't have a text y axis\n";
      throw Ex1(Errcode::BAD_ARGUMENT);
    }; break;
  }
}

void BoxChartWidget::init() {
  if (data.size() < pointsPerBox) {
    cerr << "the data vector must contain at least one data set (minimum "
         << pointsPerBox << " points)\n";
    throw(Ex1(Errcode::VECTOR_ZERO_LENGTH));
  }

  if (names.size() < 1) {
    cerr << "names vectors cannot be zero length\n";
    throw(Ex1(Errcode::VECTOR_ZERO_LENGTH));
  }

  if (data.size() != names.size() * 5) {
    cerr
        << "data vector must contain 5 times the number of elements as names\n";
    throw(Ex1(Errcode::VECTOR_MISMATCHED_LENGTHS));
  }

  StyledMultiShape2D *whiskers =
      c->addLayer(new StyledMultiShape2D(c, whiskerStyle));
  StyledMultiShape2D *boxes = c->addLayer(new StyledMultiShape2D(c, boxStyle));

  double min = yAxis->getMinBound();
  double max = yAxis->getMaxBound();

  double axisInterval = yAxis->getTickInterval();
  double yscale = -h / abs(max - min);
  double xscale = w / (names.size() + 1);
  double barCorrection = -yscale * min;
  double halfBoxWidth = boxWidth / 2;

  auto currentBoxColor = boxColors.begin();
  auto currentWhiskerColor = whiskerColors.begin();
  auto currentOutlineColor = outlineColors.begin();

  for (int i = 0, counter = 1; i < data.size(); i += pointsPerBox, counter++) {
    auto first = data.begin() + i;
    auto last = data.begin() + i + pointsPerBox;
    vector<double> currentBoxData(first, last);

    Stats1D<double> untransformedData(&currentBoxData[0], pointsPerBox);

    transform(currentBoxData.begin(), currentBoxData.end(),
              currentBoxData.begin(),
              [=, this](double d) -> double { return y + h + yscale * d; });

    Stats1D<double> dataSummary(&currentBoxData[0], pointsPerBox);

    double xLocation = x + xscale * counter - halfBoxWidth;
    double yTopLine = dataSummary.getSummary().min;
    double yBottomLine = dataSummary.getSummary().max;
    double yMedianLine = dataSummary.getSummary().median;
    double yBoxTop = dataSummary.getSummary().q1;
    double yBoxBottom = dataSummary.getSummary().q3;

    // top whisker line
    whiskers->drawLine(xLocation, yTopLine, xLocation + boxWidth, yTopLine,
                       *currentWhiskerColor);
    // bottom whisker line
    whiskers->drawLine(xLocation, yBottomLine, xLocation + boxWidth,
                       yBottomLine, *currentWhiskerColor);

    // median line
    boxes->drawLine(xLocation, yMedianLine, xLocation + boxWidth, yMedianLine,
                    *currentOutlineColor);

    // central lines
    whiskers->drawLine(xLocation + halfBoxWidth, yBottomLine,
                       xLocation + halfBoxWidth, yBoxBottom,
                       *currentWhiskerColor);
    whiskers->drawLine(xLocation + halfBoxWidth, yTopLine,
                       xLocation + halfBoxWidth, yBoxTop, *currentWhiskerColor);

    // rounded rectangle box
    boxes->fillRoundRect(xLocation, yBoxTop, boxWidth, -yBoxTop + yBoxBottom, 5,
                         5, *currentBoxColor);
    boxes->drawRoundRect(xLocation, yBoxTop, boxWidth, -yBoxTop + yBoxBottom, 5,
                         5, *currentOutlineColor);

    currentBoxColor++;
    currentWhiskerColor++;
    currentOutlineColor++;

    if (currentBoxColor == boxColors.end()) currentBoxColor = boxColors.begin();
    if (currentWhiskerColor == whiskerColors.end())
      currentWhiskerColor = whiskerColors.begin();
    if (currentOutlineColor == outlineColors.end())
      currentOutlineColor = outlineColors.begin();
  }

  commonRender();
}

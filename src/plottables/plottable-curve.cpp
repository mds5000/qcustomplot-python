/***************************************************************************
**                                                                        **
**  QCustomPlot, a simple to use, modern plotting widget for Qt           **
**  Copyright (C) 2011, 2012 Emanuel Eichhammer                           **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.WorksLikeClockwork.com/                   **
**             Date: 09.06.12                                             **
****************************************************************************/

#include "plottable-curve.h"

#include "../painter.h"
#include "../core.h"
#include "../axis.h"

// ================================================================================
// =================== QCPCurveData
// ================================================================================

/*! \class QCPCurveData
  \brief Holds the data of one single data point for QCPCurve.
  
  The stored data is:
  \li \a t: the free parameter of the curve at this curve point (cp. the mathematical vector <em>(x(t), y(t))</em>)
  \li \a key: coordinate on the key axis of this curve point
  \li \a value: coordinate on the value axis of this curve point
  
  \see QCPCurveDataMap
*/

/*!
  Constructs a curve data point with t, key and value set to zero.
*/
QCPCurveData::QCPCurveData() :
  t(0),
  key(0),
  value(0)
{
}

/*!
  Constructs a curve data point with the specified \a t, \a key and \a value.
*/
QCPCurveData::QCPCurveData(double t, double key, double value) :
  t(t),
  key(key),
  value(value)
{
}


// ================================================================================
// =================== QCPCurve
// ================================================================================

/*! \class QCPCurve
  \brief A plottable representing a parametric curve in a plot.

  To plot data, assign it with the \ref setData or \ref addData functions.
  
  \section appearance Changing the appearance
  
  The appearance of the curve is determined by the pen and the brush (\ref setPen, \ref setBrush).
  \section usage Usage
  
  Like all data representing objects in QCustomPlot, the QCPCurve is a plottable (QCPAbstractPlottable). So
  the plottable-interface of QCustomPlot applies (QCustomPlot::plottable, QCustomPlot::addPlottable, QCustomPlot::removePlottable, etc.) 
  
  Usually, you first create an instance:
  \code
  QCPCurve *newCurve = new QCPCurve(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newCurve);\endcode
  and then modify the properties of the newly created plottable, e.g.:
  \code
  newCurve->setName("Fermat's Spiral");
  newCurve->setData(tData, xData, yData);\endcode
*/

/*!
  Constructs a curve which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance and not have
  the same orientation. If either of these restrictions is violated, a corresponding message is
  printed to the debug output (qDebug), the construction is not aborted, though.
  
  The constructed QCPCurve can be added to the plot with QCustomPlot::addPlottable, QCustomPlot
  then takes ownership of the graph.
*/
QCPCurve::QCPCurve(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis)
{
  mData = new QCPCurveDataMap;
  mPen.setColor(Qt::blue);
  mPen.setStyle(Qt::SolidLine);
  mBrush.setColor(Qt::blue);
  mBrush.setStyle(Qt::NoBrush);
  mSelectedPen = mPen;
  mSelectedPen.setWidthF(2.5);
  mSelectedPen.setColor(QColor(80, 80, 255)); // lighter than Qt::blue of mPen
  mSelectedBrush = mBrush;
  
  setScatterSize(6);
  setScatterStyle(QCP::ssNone);
  setLineStyle(lsLine);
}

QCPCurve::~QCPCurve()
{
  delete mData;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the plottable
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
*/
void QCPCurve::setData(QCPCurveDataMap *data, bool copy)
{
  if (copy)
  {
    *mData = *data;
  } else
  {
    delete mData;
    mData = data;
  }
}

/*! \overload
  
  Replaces the current data with the provided points in \a t, \a key and \a value tuples. The
  provided vectors should have equal length. Else, the number of added points will be the size of
  the smallest vector.
*/
void QCPCurve::setData(const QVector<double> &t, const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = t.size();
  n = qMin(n, key.size());
  n = qMin(n, value.size());
  QCPCurveData newData;
  for (int i=0; i<n; ++i)
  {
    newData.t = t[i];
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.t, newData);
  }
}

/*! \overload
  
  Replaces the current data with the provided \a key and \a value pairs. The t parameter
  of each data point will be set to the integer index of the respective key/value pair.
*/
void QCPCurve::setData(const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  QCPCurveData newData;
  for (int i=0; i<n; ++i)
  {
    newData.t = i; // no t vector given, so we assign t the index of the key/value pair
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.t, newData);
  }
}

/*! 
  Sets the visual appearance of single data points in the plot. If set to \ref QCP::ssNone, no scatter points
  are drawn (e.g. for line-only-plots with appropriate line style).
  \see ScatterStyle, setLineStyle
*/
void QCPCurve::setScatterStyle(QCP::ScatterStyle style)
{
  mScatterStyle = style;
}

/*! 
  This defines how big (in pixels) single scatters are drawn, if scatter style (\ref
  setScatterStyle) isn't \ref QCP::ssNone, \ref QCP::ssDot or \ref QCP::ssPixmap. Floating point values are
  allowed for fine grained control over optical appearance with antialiased painting.
  
  \see ScatterStyle
*/
void QCPCurve::setScatterSize(double size)
{
  mScatterSize = size;
}

/*! 
  If the scatter style (\ref setScatterStyle) is set to ssPixmap, this function defines the QPixmap
  that will be drawn centered on the data point coordinate.
  
  \see ScatterStyle
*/
void QCPCurve::setScatterPixmap(const QPixmap &pixmap)
{
  mScatterPixmap = pixmap;
}

/*!
  Sets how the single data points are connected in the plot or how they are represented visually
  apart from the scatter symbol. For scatter-only plots, set \a style to \ref lsNone and \ref
  setScatterStyle to the desired scatter style.
  
  \see setScatterStyle
*/
void QCPCurve::setLineStyle(QCPCurve::LineStyle style)
{
  mLineStyle = style;
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  \see removeData
*/
void QCPCurve::addData(const QCPCurveDataMap &dataMap)
{
  mData->unite(dataMap);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  \see removeData
*/
void QCPCurve::addData(const QCPCurveData &data)
{
  mData->insertMulti(data.t, data);
}

/*! \overload
  Adds the provided single data point as \a t, \a key and \a value tuple to the current data
  \see removeData
*/
void QCPCurve::addData(double t, double key, double value)
{
  QCPCurveData newData;
  newData.t = t;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.t, newData);
}

/*! \overload
  
  Adds the provided single data point as \a key and \a value pair to the current data The t
  parameter of the data point is set to the t of the last data point plus 1. If there is no last
  data point, t will be set to 0.
  
  \see removeData
*/
void QCPCurve::addData(double key, double value)
{
  QCPCurveData newData;
  if (!mData->isEmpty())
    newData.t = (mData->constEnd()-1).key()+1;
  else
    newData.t = 0;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.t, newData);
}

/*! \overload
  Adds the provided data points as \a t, \a key and \a value tuples to the current data.
  \see removeData
*/
void QCPCurve::addData(const QVector<double> &ts, const QVector<double> &keys, const QVector<double> &values)
{
  int n = ts.size();
  n = qMin(n, keys.size());
  n = qMin(n, values.size());
  QCPCurveData newData;
  for (int i=0; i<n; ++i)
  {
    newData.t = ts[i];
    newData.key = keys[i];
    newData.value = values[i];
    mData->insertMulti(newData.t, newData);
  }
}

/*!
  Removes all data points with curve parameter t smaller than \a t.
  \see addData, clearData
*/
void QCPCurve::removeDataBefore(double t)
{
  QCPCurveDataMap::iterator it = mData->begin();
  while (it != mData->end() && it.key() < t)
    it = mData->erase(it);
}

/*!
  Removes all data points with curve parameter t greater than \a t.
  \see addData, clearData
*/
void QCPCurve::removeDataAfter(double t)
{
  if (mData->isEmpty()) return;
  QCPCurveDataMap::iterator it = mData->upperBound(t);
  while (it != mData->end())
    it = mData->erase(it);
}

/*!
  Removes all data points with curve parameter t between \a fromt and \a tot. if \a fromt is
  greater or equal to \a tot, the function does nothing. To remove a single data point with known
  t, use \ref removeData(double t).
  
  \see addData, clearData
*/
void QCPCurve::removeData(double fromt, double tot)
{
  if (fromt >= tot || mData->isEmpty()) return;
  QCPCurveDataMap::iterator it = mData->upperBound(fromt);
  QCPCurveDataMap::iterator itEnd = mData->upperBound(tot);
  while (it != itEnd)
    it = mData->erase(it);
}

/*! \overload
  
  Removes a single data point at curve parameter \a t. If the position is not known with absolute
  precision, consider using \ref removeData(double fromt, double tot) with a small fuzziness
  interval around the suspected position, depeding on the precision with which the curve parameter
  is known.
  
  \see addData, clearData
*/
void QCPCurve::removeData(double t)
{
  mData->remove(t);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPCurve::clearData()
{
  mData->clear();
}

/* inherits documentation from base class */
double QCPCurve::selectTest(const QPointF &pos) const
{
  if (mData->isEmpty() || !mVisible)
    return -1;
  
  return pointDistance(pos);
}

/* inherits documentation from base class */
void QCPCurve::draw(QCPPainter *painter)
{
  if (mData->isEmpty()) return;
  
  // allocate line vector:
  QVector<QPointF> *lineData = new QVector<QPointF>;
  // fill with curve data:
  getCurveData(lineData);
  // draw curve fill:
  if (mainBrush().style() != Qt::NoBrush && mainBrush().color().alpha() != 0)
  {
    applyFillAntialiasingHint(painter);
    painter->setPen(Qt::NoPen);
    painter->setBrush(mainBrush());
    painter->drawPolygon(QPolygonF(*lineData));
  }
  // draw curve line:
  if (mLineStyle != lsNone && mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
  {
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mainPen());
    painter->setBrush(Qt::NoBrush);
    // if drawing solid line and not in PDF, use much faster line drawing instead of polyline:
    if (mParentPlot->plottingHints().testFlag(QCP::phFastPolylines) &&
        painter->pen().style() == Qt::SolidLine &&
        !painter->pdfExportMode())
    {
      for (int i=1; i<lineData->size(); ++i)
        painter->drawLine(lineData->at(i-1), lineData->at(i));
    } else
    {  
      painter->drawPolyline(QPolygonF(*lineData));
    }
  }
  // draw scatters:
  if (mScatterStyle != QCP::ssNone)
    drawScatterPlot(painter, lineData);
  // free allocated line data:
  delete lineData;
}

/* inherits documentation from base class */
void QCPCurve::drawLegendIcon(QCPPainter *painter, const QRect &rect) const
{
  // draw fill:
  if (mBrush.style() != Qt::NoBrush)
  {
    applyFillAntialiasingHint(painter);
    painter->fillRect(QRectF(rect.left(), rect.top()+rect.height()/2.0, rect.width(), rect.height()/3.0), mBrush);
  }
  // draw line vertically centered:
  if (mLineStyle != lsNone)
  {
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mPen);
    painter->drawLine(QLineF(rect.left(), rect.top()+rect.height()/2.0, rect.right()+5, rect.top()+rect.height()/2.0)); // +5 on x2 else last segment is missing from dashed/dotted pens
  }
  // draw scatter symbol:
  if (mScatterStyle != QCP::ssNone)
  {
    applyScattersAntialiasingHint(painter);
    painter->setPen(mPen);
    if (mScatterStyle == QCP::ssPixmap)
    {
      if (mScatterPixmap.size().width() > rect.width() || mScatterPixmap.size().height() > rect.height()) // scale scatter pixmap if it's too large to fit in legend icon rect
        painter->setScatterPixmap(mScatterPixmap.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
      else
        painter->setScatterPixmap(mScatterPixmap);
    }
    painter->drawScatter(QRectF(rect).center().x(), QRectF(rect).center().y(), mScatterSize, mScatterStyle);
  }
}

/*! \internal
  
  Draws scatter symbols at every data point passed in \a pointData. scatter symbols are independent of
  the line style and are always drawn if scatter style is not \ref QCP::ssNone.
*/
void QCPCurve::drawScatterPlot(QCPPainter *painter, const QVector<QPointF> *pointData) const
{
  // draw scatter point symbols:
  applyScattersAntialiasingHint(painter);
  painter->setPen(mainPen());
  painter->setBrush(mainBrush());
  painter->setScatterPixmap(mScatterPixmap);
  for (int i=0; i<pointData->size(); ++i)
    painter->drawScatter(pointData->at(i).x(), pointData->at(i).y(), mScatterSize, mScatterStyle);
}

/*! \internal
  
  called by QCPCurve::draw to generate a point vector (pixels) which represents the line of the
  curve. Line segments that aren't visible in the current axis rect are handled in an optimized
  way.
*/
void QCPCurve::getCurveData(QVector<QPointF> *lineData) const
{
  /* Extended sides of axis rect R divide space into 9 regions:
     1__|_4_|__7  
     2__|_R_|__8
     3  | 6 |  9 
     General idea: If the two points of a line segment are in the same region (that is not R), the line segment corner is removed.
     Curves outside R become straight lines closely outside of R which greatly reduces drawing time, yet keeps the look of lines and
     fills inside R consistent.
     The region R has index 5.
  */
  lineData->reserve(mData->size());
  QCPCurveDataMap::const_iterator it;
  int lastRegion = 5;
  int currentRegion = 5;
  double RLeft = mKeyAxis->range().lower;
  double RRight = mKeyAxis->range().upper;
  double RBottom = mValueAxis->range().lower;
  double RTop = mValueAxis->range().upper;
  double x, y; // current key/value
  bool addedLastAlready = true;
  bool firstPoint = true; // first point must always be drawn, to make sure fill works correctly
  for (it = mData->constBegin(); it != mData->constEnd(); ++it)
  {
    x = it.value().key;
    y = it.value().value;
    // determine current region:
    if (x < RLeft) // region 123
    {
      if (y > RTop)
        currentRegion = 1;
      else if (y < RBottom)
        currentRegion = 3;
      else
        currentRegion = 2;
    } else if (x > RRight) // region 789
    {
      if (y > RTop)
        currentRegion = 7;
      else if (y < RBottom)
        currentRegion = 9;
      else
        currentRegion = 8;
    } else // region 456
    {
      if (y > RTop)
        currentRegion = 4;
      else if (y < RBottom)
        currentRegion = 6;
      else
        currentRegion = 5;
    }
    
    /*
      Watch out, the next part is very tricky. It modifies the curve such that it seems like the
      whole thing is still drawn, but actually the points outside the axisRect are simplified
      ("optimized") greatly. There are some subtle special cases when line segments are large and
      thereby each subsequent point may be in a different region or even skip some.
    */
    // determine whether to keep current point:
    if (currentRegion == 5 || (firstPoint && mBrush.style() != Qt::NoBrush)) // current is in R, add current and last if it wasn't added already
    {
      if (!addedLastAlready) // in case curve just entered R, make sure the last point outside R is also drawn correctly
        lineData->append(coordsToPixels((it-1).value().key, (it-1).value().value)); // add last point to vector
      else if (lastRegion != 5) // added last already. If that's the case, we probably added it at optimized position. So go back and make sure it's at original position (else the angle changes under which this segment enters R)
      {
        if (!firstPoint) // because on firstPoint, currentRegion is 5 and addedLastAlready is true, although there is no last point
          lineData->replace(lineData->size()-1, coordsToPixels((it-1).value().key, (it-1).value().value));
      }
      lineData->append(coordsToPixels(it.value().key, it.value().value)); // add current point to vector
      addedLastAlready = true; // so in next iteration, we don't add this point twice
    } else if (currentRegion != lastRegion) // changed region, add current and last if not added already
    {
      // using outsideCoordsToPixels instead of coorsToPixels for optimized point placement (places points just outside axisRect instead of potentially far away)
      
      // if we're coming from R or we skip diagonally over the corner regions (so line might still be visible in R), we can't place points optimized
      if (lastRegion == 5 || // coming from R
          ((lastRegion==2 && currentRegion==4) || (lastRegion==4 && currentRegion==2)) || // skip top left diagonal
          ((lastRegion==4 && currentRegion==8) || (lastRegion==8 && currentRegion==4)) || // skip top right diagonal
          ((lastRegion==8 && currentRegion==6) || (lastRegion==6 && currentRegion==8)) || // skip bottom right diagonal
          ((lastRegion==6 && currentRegion==2) || (lastRegion==2 && currentRegion==6))    // skip bottom left diagonal
          )
      {
        // always add last point if not added already, original:
        if (!addedLastAlready)
          lineData->append(coordsToPixels((it-1).value().key, (it-1).value().value));
        // add current point, original:
        lineData->append(coordsToPixels(it.value().key, it.value().value));
      } else // no special case that forbids optimized point placement, so do it:
      {
        // always add last point if not added already, optimized:
        if (!addedLastAlready)
          lineData->append(outsideCoordsToPixels((it-1).value().key, (it-1).value().value, currentRegion));
        // add current point, optimized:
        lineData->append(outsideCoordsToPixels(it.value().key, it.value().value, currentRegion));
      }
      addedLastAlready = true; // so that if next point enters 5, or crosses another region boundary, we don't add this point twice
    } else // neither in R, nor crossed a region boundary, skip current point
    {
      addedLastAlready = false;
    }
    lastRegion = currentRegion;
    firstPoint = false;
  }
  // If curve ends outside R, we want to add very last point so the fill looks like it should when the curve started inside R:
  if (lastRegion != 5 && mBrush.style() != Qt::NoBrush && !mData->isEmpty())
    lineData->append(coordsToPixels((mData->constEnd()-1).value().key, (mData->constEnd()-1).value().value));
}

/*! \internal 
  
  Calculates the (minimum) distance (in pixels) the curve's representation has from the given \a
  pixelPoint in pixels. This is used to determine whether the curve was clicked or not, e.g. in
  \ref selectTest.
*/
double QCPCurve::pointDistance(const QPointF &pixelPoint) const
{
  if (mData->isEmpty())
  {
    qDebug() << Q_FUNC_INFO << "requested point distance on curve" << mName << "without data";
    return 500;
  }
  if (mData->size() == 1)
  {
    QPointF dataPoint = coordsToPixels(mData->constBegin().key(), mData->constBegin().value().value);
    return QVector2D(dataPoint-pixelPoint).length();
  }
  
  // calculate minimum distance to line segments:
  QVector<QPointF> *lineData = new QVector<QPointF>;
  getCurveData(lineData);
  double minDistSqr = std::numeric_limits<double>::max();
  for (int i=0; i<lineData->size()-1; ++i)
  {
    double currentDistSqr = distSqrToLine(lineData->at(i), lineData->at(i+1), pixelPoint);
    if (currentDistSqr < minDistSqr)
      minDistSqr = currentDistSqr;
  }
  delete lineData;
  return sqrt(minDistSqr);
}

/*! \internal
  
  This is a specialized \ref coordsToPixels function for points that are outside the visible
  axisRect and just crossing a boundary (since \ref getCurveData reduces non-visible curve segments
  to those line segments that cross region boundaries, see documentation there). It only uses the
  coordinate parallel to the region boundary of the axisRect. The other coordinate is picked 10
  pixels outside the axisRect. Together with the optimization in \ref getCurveData this improves
  performance for large curves (or zoomed in ones) significantly while keeping the illusion the
  whole curve and its filling is still being drawn for the viewer.
*/
QPointF QCPCurve::outsideCoordsToPixels(double key, double value, int region) const
{
  int margin = 10;
  QRect axisRect = mKeyAxis->axisRect() | mValueAxis->axisRect();
  QPointF result = coordsToPixels(key, value);
  switch (region)
  {
    case 2: result.setX(axisRect.left()-margin); break; // left
    case 8: result.setX(axisRect.right()+margin); break; // right
    case 4: result.setY(axisRect.top()-margin); break; // top
    case 6: result.setY(axisRect.bottom()+margin); break; // bottom
    case 1: result.setX(axisRect.left()-margin);
            result.setY(axisRect.top()-margin); break; // top left
    case 7: result.setX(axisRect.right()+margin);
            result.setY(axisRect.top()-margin); break; // top right
    case 9: result.setX(axisRect.right()+margin);
            result.setY(axisRect.bottom()+margin); break; // bottom right
    case 3: result.setX(axisRect.left()-margin);
            result.setY(axisRect.bottom()+margin); break; // bottom left
  }
  return result;
}

/* inherits documentation from base class */
QCPRange QCPCurve::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current;
  
  QCPCurveDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().key;
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current < 0) || (inSignDomain == sdPositive && current > 0))
    {
      if (current < range.lower || !haveLower)
      {
        range.lower = current;
        haveLower = true;
      }
      if (current > range.upper || !haveUpper)
      {
        range.upper = current;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

/* inherits documentation from base class */
QCPRange QCPCurve::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current;
  
  QCPCurveDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().value;
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current < 0) || (inSignDomain == sdPositive && current > 0))
    {
      if (current < range.lower || !haveLower)
      {
        range.lower = current;
        haveLower = true;
      }
      if (current > range.upper || !haveUpper)
      {
        range.upper = current;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

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

#include "plottable-bars.h"

#include "../painter.h"
#include "../core.h"
#include "../axis.h"

// ================================================================================
// =================== QCPBarData
// ================================================================================

/*! \class QCPBarData
  \brief Holds the data of one single data point (one bar) for QCPBars.
  
  The stored data is:
  \li \a key: coordinate on the key axis of this bar
  \li \a value: height coordinate on the value axis of this bar
  
  \see QCPBarDataaMap
*/

/*!
  Constructs a bar data point with key and value set to zero.
*/
QCPBarData::QCPBarData() :
  key(0),
  value(0)
{
}

/*!
  Constructs a bar data point with the specified \a key and \a value.
*/
QCPBarData::QCPBarData(double key, double value) :
  key(key),
  value(value)
{
}


// ================================================================================
// =================== QCPBars
// ================================================================================

/*! \class QCPBars
  \brief A plottable representing a bar chart in a plot.

  To plot data, assign it with the \ref setData or \ref addData functions.
  
  \section appearance Changing the appearance
  
  The appearance of the bars is determined by the pen and the brush (\ref setPen, \ref setBrush).
  
  Bar charts are stackable. This means, Two QCPBars plottables can be placed on top of each other
  (see \ref QCPBars::moveAbove). Then, when two bars are at the same key position, they will appear
  stacked.
  
  \section usage Usage
  
  Like all data representing objects in QCustomPlot, the QCPBars is a plottable
  (QCPAbstractPlottable). So the plottable-interface of QCustomPlot applies
  (QCustomPlot::plottable, QCustomPlot::addPlottable, QCustomPlot::removePlottable, etc.)
  
  Usually, you first create an instance:
  \code
  QCPBars *newBars = new QCPBars(customPlot->xAxis, customPlot->yAxis);\endcode
  add it to the customPlot with QCustomPlot::addPlottable:
  \code
  customPlot->addPlottable(newBars);\endcode
  and then modify the properties of the newly created plottable, e.g.:
  \code
  newBars->setName("Country population");
  newBars->setData(xData, yData);\endcode
*/

/*! \fn QCPBars *QCPBars::barBelow() const
  Returns the bars plottable that is directly below this bars plottable.
  If there is no such plottable, returns 0.
  
  \see barAbove, moveBelow, moveAbove
*/

/*! \fn QCPBars *QCPBars::barAbove() const
  Returns the bars plottable that is directly above this bars plottable.
  If there is no such plottable, returns 0.
  
  \see barBelow, moveBelow, moveAbove
*/

/*!
  Constructs a bar chart which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance and not have
  the same orientation. If either of these restrictions is violated, a corresponding message is
  printed to the debug output (qDebug), the construction is not aborted, though.
  
  The constructed QCPBars can be added to the plot with QCustomPlot::addPlottable, QCustomPlot
  then takes ownership of the bar chart.
*/
QCPBars::QCPBars(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis),
  mBarBelow(0),
  mBarAbove(0)
{
  mData = new QCPBarDataMap;
  mPen.setColor(Qt::blue);
  mPen.setStyle(Qt::SolidLine);
  mBrush.setColor(QColor(40, 50, 255, 30));
  mBrush.setStyle(Qt::SolidPattern);
  mSelectedPen = mPen;
  mSelectedPen.setWidthF(2.5);
  mSelectedPen.setColor(QColor(80, 80, 255)); // lighter than Qt::blue of mPen
  mSelectedBrush = mBrush;
  
  mWidth = 0.75;
}

QCPBars::~QCPBars()
{
  if (mBarBelow || mBarAbove)
    connectBars(mBarBelow, mBarAbove); // take this bar out of any stacking
  delete mData;
}

/*!
  Sets the width of the bars in plot (key) coordinates.
*/
void QCPBars::setWidth(double width)
{
  mWidth = width;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the plottable
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
*/
void QCPBars::setData(QCPBarDataMap *data, bool copy)
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
  
  Replaces the current data with the provided points in \a key and \a value tuples. The
  provided vectors should have equal length. Else, the number of added points will be the size of
  the smallest vector.
*/
void QCPBars::setData(const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  QCPBarData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Moves this bars plottable below \a bars. In other words, the bars of this plottable will appear
  below the bars of \a bars. The move target \a bars must use the same key and value axis as this
  plottable.
  
  Inserting into and removing from existing bar stacking is handled gracefully. If \a bars already
  has a bars object below itself, this bars object is inserted between the two. If this bars object
  is already between two other bars, the two other bars will be stacked on top of each other after
  the operation.
  
  To remove this bars plottable from any stacking, set \a bars to 0.
  
  \see moveBelow, barAbove, barBelow
*/
void QCPBars::moveBelow(QCPBars *bars)
{
  if (bars == this) return;
  if (bars->keyAxis() != mKeyAxis || bars->valueAxis() != mValueAxis)
  {
    qDebug() << Q_FUNC_INFO << "passed QCPBars* doesn't have same key and value axis as this QCPBars";
    return;
  }
  // remove from stacking:
  connectBars(mBarBelow, mBarAbove); // Note: also works if one (or both) of them is 0
  // if new bar given, insert this bar below it:
  if (bars)
  {
    if (bars->mBarBelow)
      connectBars(bars->mBarBelow, this);
    connectBars(this, bars);
  }
}

/*!
  Moves this bars plottable above \a bars. In other words, the bars of this plottable will appear
  above the bars of \a bars. The move target \a bars must use the same key and value axis as this
  plottable.
  
  Inserting into and removing from existing bar stacking is handled gracefully. If \a bars already
  has a bars object below itself, this bars object is inserted between the two. If this bars object
  is already between two other bars, the two other bars will be stacked on top of each other after
  the operation.
  
  To remove this bars plottable from any stacking, set \a bars to 0.
  
  \see moveBelow, barBelow, barAbove
*/
void QCPBars::moveAbove(QCPBars *bars)
{
  if (bars == this) return;
  if (bars && (bars->keyAxis() != mKeyAxis || bars->valueAxis() != mValueAxis))
  {
    qDebug() << Q_FUNC_INFO << "passed QCPBars* doesn't have same key and value axis as this QCPBars";
    return;
  }
  // remove from stacking:
  connectBars(mBarBelow, mBarAbove); // Note: also works if one (or both) of them is 0
  // if new bar given, insert this bar above it:
  if (bars)
  {
    if (bars->mBarAbove)
      connectBars(this, bars->mBarAbove);
    connectBars(bars, this);
  }
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  \see removeData
*/
void QCPBars::addData(const QCPBarDataMap &dataMap)
{
  mData->unite(dataMap);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  \see removeData
*/
void QCPBars::addData(const QCPBarData &data)
{
  mData->insertMulti(data.key, data);
}

/*! \overload
  Adds the provided single data point as \a key and \a value tuple to the current data
  \see removeData
*/
void QCPBars::addData(double key, double value)
{
  QCPBarData newData;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.key, newData);
}

/*! \overload
  Adds the provided data points as \a key and \a value tuples to the current data.
  \see removeData
*/
void QCPBars::addData(const QVector<double> &keys, const QVector<double> &values)
{
  int n = keys.size();
  n = qMin(n, values.size());
  QCPBarData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = keys[i];
    newData.value = values[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Removes all data points with key smaller than \a key.
  \see addData, clearData
*/
void QCPBars::removeDataBefore(double key)
{
  QCPBarDataMap::iterator it = mData->begin();
  while (it != mData->end() && it.key() < key)
    it = mData->erase(it);
}

/*!
  Removes all data points with key greater than \a key.
  \see addData, clearData
*/
void QCPBars::removeDataAfter(double key)
{
  if (mData->isEmpty()) return;
  QCPBarDataMap::iterator it = mData->upperBound(key);
  while (it != mData->end())
    it = mData->erase(it);
}

/*!
  Removes all data points with key between \a fromKey and \a toKey. if \a fromKey is
  greater or equal to \a toKey, the function does nothing. To remove a single data point with known
  key, use \ref removeData(double key).
  
  \see addData, clearData
*/
void QCPBars::removeData(double fromKey, double toKey)
{
  if (fromKey >= toKey || mData->isEmpty()) return;
  QCPBarDataMap::iterator it = mData->upperBound(fromKey);
  QCPBarDataMap::iterator itEnd = mData->upperBound(toKey);
  while (it != itEnd)
    it = mData->erase(it);
}

/*! \overload
  
  Removes a single data point at \a key. If the position is not known with absolute precision,
  consider using \ref removeData(double fromKey, double toKey) with a small fuzziness interval
  around the suspected position, depeding on the precision with which the key is known.
  
  \see addData, clearData
*/
void QCPBars::removeData(double key)
{
  mData->remove(key);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPBars::clearData()
{
  mData->clear();
}

/* inherits documentation from base class */
double QCPBars::selectTest(const QPointF &pos) const
{
  QCPBarDataMap::ConstIterator it;
  double posKey, posValue;
  pixelsToCoords(pos, posKey, posValue);
  for (it = mData->constBegin(); it != mData->constEnd(); ++it)
  {
    double baseValue = getBaseValue(it.key(), it.value().value >=0);
    QCPRange keyRange(it.key()-mWidth*0.5, it.key()+mWidth*0.5);
    QCPRange valueRange(baseValue, baseValue+it.value().value);
    if (keyRange.contains(posKey) && valueRange.contains(posValue))
      return mParentPlot->selectionTolerance()*0.99;
  }
  return -1;
}

/* inherits documentation from base class */
void QCPBars::draw(QCPPainter *painter)
{
  if (mData->isEmpty()) return;
  
  QCPBarDataMap::const_iterator it;
  for (it = mData->constBegin(); it != mData->constEnd(); ++it)
  {
    if (it.key()+mWidth*0.5 < mKeyAxis->range().lower || it.key()-mWidth*0.5 > mKeyAxis->range().upper)
      continue;
    QPolygonF barPolygon = getBarPolygon(it.key(), it.value().value);
    // draw bar fill:
    if (mainBrush().style() != Qt::NoBrush && mainBrush().color().alpha() != 0)
    {
      applyFillAntialiasingHint(painter);
      painter->setPen(Qt::NoPen);
      painter->setBrush(mainBrush());
      painter->drawPolygon(barPolygon);
    }
    // draw bar line:
    if (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
    {
      applyDefaultAntialiasingHint(painter);
      painter->setPen(mainPen());
      painter->setBrush(Qt::NoBrush);
      painter->drawPolyline(barPolygon);
    }
  }
}

/* inherits documentation from base class */
void QCPBars::drawLegendIcon(QCPPainter *painter, const QRect &rect) const
{
  // draw filled rect:
  applyDefaultAntialiasingHint(painter);
  painter->setBrush(mBrush);
  painter->setPen(mPen);
  QRectF r = QRectF(0, 0, rect.width()*0.67, rect.height()*0.67);
  r.moveCenter(rect.center());
  painter->drawRect(r);
}

/*! \internal
  
  Returns the polygon of a single bar with \a key and \a value. The Polygon is open at the bottom
  and shifted according to the bar stacking (see \ref moveAbove).
*/
QPolygonF QCPBars::getBarPolygon(double key, double value) const
{
  QPolygonF result;
  double baseValue = getBaseValue(key, value >= 0);
  result << coordsToPixels(key-mWidth*0.5, baseValue);
  result << coordsToPixels(key-mWidth*0.5, baseValue+value);
  result << coordsToPixels(key+mWidth*0.5, baseValue+value);
  result << coordsToPixels(key+mWidth*0.5, baseValue);
  return result;
}

/*! \internal
  
  This function is called to find at which value to start drawing the base of a bar at \a key, when
  it is stacked on top of another QCPBars (e.g. with \ref moveAbove).
  
  positive and negative bars are separated per stack (positive are stacked above 0-value upwards,
  negative are stacked below 0-value downwards). This can be indicated with \a positive. So if the
  bar for which we need the base value is negative, set \a positive to false.
*/
double QCPBars::getBaseValue(double key, bool positive) const
{
  if (mBarBelow)
  {
    double max = 0;
    // find bars of mBarBelow that are approximately at key and find largest one:
    QCPBarDataMap::const_iterator it = mBarBelow->mData->lowerBound(key-mWidth*0.1);
    QCPBarDataMap::const_iterator itEnd = mBarBelow->mData->upperBound(key+mWidth*0.1);
    while (it != itEnd)
    {
      if ((positive && it.value().value > max) ||
          (!positive && it.value().value < max))
        max = it.value().value;
      ++it;
    }
    // recurse down the bar-stack to find the total height:
    return max + mBarBelow->getBaseValue(key, positive);
  } else
    return 0;
}

/*! \internal

  Connects \a below and \a above to each other via their mBarAbove/mBarBelow properties.
  The bar(s) currently below lower and upper will become disconnected to lower/upper.
  
  If lower is zero, upper will be disconnected at the bottom.
  If upper is zero, lower will be disconnected at the top.
*/
void QCPBars::connectBars(QCPBars *lower, QCPBars *upper)
{
  if (!lower && !upper) return;
  
  if (!lower) // disconnect upper at bottom
  {
    // disconnect old bar below upper:
    if (upper->mBarBelow && upper->mBarBelow->mBarAbove == upper)
      upper->mBarBelow->mBarAbove = 0;
    upper->mBarBelow = 0;
  } else if (!upper) // disconnect lower at top
  {
    // disconnect old bar above lower:
    if (lower->mBarAbove && lower->mBarAbove->mBarBelow == lower)
      lower->mBarAbove->mBarBelow = 0;
    lower->mBarAbove = 0;
  } else // connect lower and upper
  {
    // disconnect old bar above lower:
    if (lower->mBarAbove && lower->mBarAbove->mBarBelow == lower)
      lower->mBarAbove->mBarBelow = 0;
    // disconnect old bar below upper:
    if (upper->mBarBelow && upper->mBarBelow->mBarAbove == upper)
      upper->mBarBelow->mBarAbove = 0;
    lower->mBarAbove = upper;
    upper->mBarBelow = lower;
  }
}

/* inherits documentation from base class */
QCPRange QCPBars::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current;
  double barWidthHalf = mWidth*0.5;
  QCPBarDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().key;
    if (inSignDomain == sdBoth || (inSignDomain == sdNegative && current+barWidthHalf < 0) || (inSignDomain == sdPositive && current-barWidthHalf > 0))
    {
      if (current-barWidthHalf < range.lower || !haveLower)
      {
        range.lower = current-barWidthHalf;
        haveLower = true;
      }
      if (current+barWidthHalf > range.upper || !haveUpper)
      {
        range.upper = current+barWidthHalf;
        haveUpper = true;
      }
    }
    ++it;
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

/* inherits documentation from base class */
QCPRange QCPBars::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  QCPRange range;
  bool haveLower = true; // set to true, because 0 should always be visible in bar charts
  bool haveUpper = true; // set to true, because 0 should always be visible in bar charts
  
  double current;
  
  QCPBarDataMap::const_iterator it = mData->constBegin();
  while (it != mData->constEnd())
  {
    current = it.value().value + getBaseValue(it.value().key, it.value().value >= 0);
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
  
  validRange = range.lower < range.upper;
  return range;
}

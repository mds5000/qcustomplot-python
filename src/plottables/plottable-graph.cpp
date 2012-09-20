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

#include "plottable-graph.h"

#include "../painter.h"
#include "../core.h"
#include "../axis.h"

// ================================================================================
// =================== QCPData
// ================================================================================

/*! \class QCPData
  \brief Holds the data of one single data point for QCPGraph.
  
  The stored data is:
  \li \a key: coordinate on the key axis of this data point
  \li \a value: coordinate on the value axis of this data point
  \li \a keyErrorMinus: negative error in the key dimension (for error bars)
  \li \a keyErrorPlus: positive error in the key dimension (for error bars)
  \li \a valueErrorMinus: negative error in the value dimension (for error bars)
  \li \a valueErrorPlus: positive error in the value dimension (for error bars)
  
  \see QCPDataMap
*/

/*!
  Constructs a data point with key, value and all errors set to zero.
*/
QCPData::QCPData() :
  key(0),
  value(0),
  keyErrorPlus(0),
  keyErrorMinus(0),
  valueErrorPlus(0),
  valueErrorMinus(0)
{
}

/*!
  Constructs a data point with the specified \a key and \a value. All errors are set to zero.
*/
QCPData::QCPData(double key, double value) :
  key(key),
  value(value),
  keyErrorPlus(0),
  keyErrorMinus(0),
  valueErrorPlus(0),
  valueErrorMinus(0)
{
}


// ================================================================================
// =================== QCPGraph
// ================================================================================

/*! \class QCPGraph
  \brief A plottable representing a graph in a plot.

  Usually QCustomPlot creates it internally via QCustomPlot::addGraph and the resulting instance is
  accessed via QCustomPlot::graph.

  To plot data, assign it with the \ref setData or \ref addData functions.
  
  \section appearance Changing the appearance
  
  The appearance of the graph is mainly determined by the line style, scatter style, brush and pen
  of the graph (\ref setLineStyle, \ref setScatterStyle, \ref setBrush, \ref setPen).
  
  \subsection filling Filling under or between graphs
  
  QCPGraph knows two types of fills: Normal graph fills towards the zero-value-line parallel to
  the key axis of the graph, and fills between two graphs, called channel fills. To enable a fill,
  just set a brush with \ref setBrush which is neither Qt::NoBrush nor fully transparent.
  
  By default, a normal fill towards the zero-value-line will be drawn. To set up a channel fill
  between this graph and another one, call \ref setChannelFillGraph with the other graph as
  parameter.

  \see QCustomPlot::addGraph, QCustomPlot::graph, QCPLegend::addGraph
*/

/*!
  Constructs a graph which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance and not have
  the same orientation. If either of these restrictions is violated, a corresponding message is
  printed to the debug output (qDebug), the construction is not aborted, though.
  
  The constructed QCPGraph can be added to the plot with QCustomPlot::addPlottable, QCustomPlot
  then takes ownership of the graph.
  
  To directly create a graph inside a plot, you can also use the simpler QCustomPlot::addGraph function.
*/
QCPGraph::QCPGraph(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis)
{
  mData = new QCPDataMap;
  
  setPen(QPen(Qt::blue));
  setErrorPen(QPen(Qt::black));
  setBrush(Qt::NoBrush);
  setSelectedPen(QPen(QColor(80, 80, 255), 2.5));
  setSelectedBrush(Qt::NoBrush);
  
  setLineStyle(lsLine);
  setScatterStyle(QCP::ssNone);
  setScatterSize(6);
  setErrorType(etNone);
  setErrorBarSize(6);
  setErrorBarSkipSymbol(true);
  setChannelFillGraph(0);
}

QCPGraph::~QCPGraph()
{
  if (mParentPlot)
  {
    // if another graph has a channel fill towards this graph, set it to zero
    for (int i=0; i<mParentPlot->graphCount(); ++i)
    {
      if (mParentPlot->graph(i)->channelFillGraph() == this)
        mParentPlot->graph(i)->setChannelFillGraph(0);
    }
  }
  delete mData;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the graph
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
*/
void QCPGraph::setData(QCPDataMap *data, bool copy)
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
  
  Replaces the current data with the provided points in \a key and \a value pairs. The provided
  vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setData(const QVector<double> &key, const QVector<double> &value)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  symmetrical value error of the data points are set to the values in \a valueError.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
  
  For asymmetrical errors (plus different from minus), see the overloaded version of this function.
*/
void QCPGraph::setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueError)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueError.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.valueErrorMinus = valueError[i];
    newData.valueErrorPlus = valueError[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  \overload
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  negative value error of the data points are set to the values in \a valueErrorMinus, the positive
  value error to \a valueErrorPlus.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueErrorMinus.size());
  n = qMin(n, valueErrorPlus.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.valueErrorMinus = valueErrorMinus[i];
    newData.valueErrorPlus = valueErrorPlus[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  symmetrical key error of the data points are set to the values in \a keyError.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
  
  For asymmetrical errors (plus different from minus), see the overloaded version of this function.
*/
void QCPGraph::setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, keyError.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyError[i];
    newData.keyErrorPlus = keyError[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  \overload
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  negative key error of the data points are set to the values in \a keyErrorMinus, the positive
  key error to \a keyErrorPlus.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, keyErrorMinus.size());
  n = qMin(n, keyErrorPlus.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyErrorMinus[i];
    newData.keyErrorPlus = keyErrorPlus[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  symmetrical key and value errors of the data points are set to the values in \a keyError and \a valueError.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
  
  For asymmetrical errors (plus different from minus), see the overloaded version of this function.
*/
void QCPGraph::setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError, const QVector<double> &valueError)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueError.size());
  n = qMin(n, keyError.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyError[i];
    newData.keyErrorPlus = keyError[i];
    newData.valueErrorMinus = valueError[i];
    newData.valueErrorPlus = valueError[i];
    mData->insertMulti(key[i], newData);
  }
}

/*!
  \overload
  Replaces the current data with the provided points in \a key and \a value pairs. Additionally the
  negative key and value errors of the data points are set to the values in \a keyErrorMinus and \a valueErrorMinus. The positive
  key and value errors are set to the values in \a keyErrorPlus \a valueErrorPlus.
  For error bars to show appropriately, see \ref setErrorType.
  The provided vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus)
{
  mData->clear();
  int n = key.size();
  n = qMin(n, value.size());
  n = qMin(n, valueErrorMinus.size());
  n = qMin(n, valueErrorPlus.size());
  n = qMin(n, keyErrorMinus.size());
  n = qMin(n, keyErrorPlus.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = key[i];
    newData.value = value[i];
    newData.keyErrorMinus = keyErrorMinus[i];
    newData.keyErrorPlus = keyErrorPlus[i];
    newData.valueErrorMinus = valueErrorMinus[i];
    newData.valueErrorPlus = valueErrorPlus[i];
    mData->insertMulti(key[i], newData);
  }
}


/*!
  Sets how the single data points are connected in the plot or how they are represented visually
  apart from the scatter symbol. For scatter-only plots, set \a ls to \ref lsNone and \ref
  setScatterStyle to the desired scatter style.
  
  \see setScatterStyle
*/
void QCPGraph::setLineStyle(LineStyle ls)
{
  mLineStyle = ls;
}

/*! 
  Sets the visual appearance of single data points in the plot. If set to \ref QCP::ssNone, no scatter points
  are drawn (e.g. for line-only-plots with appropriate line style).
  
  \see ScatterStyle, setLineStyle
*/
void QCPGraph::setScatterStyle(QCP::ScatterStyle ss)
{
  mScatterStyle = ss;
}

/*! 
  This defines how big (in pixels) single scatters are drawn, if scatter style (\ref
  setScatterStyle) isn't \ref QCP::ssNone, \ref QCP::ssDot or \ref QCP::ssPixmap. Floating point values are
  allowed for fine grained control over optical appearance with antialiased painting.
  
  \see ScatterStyle
*/
void QCPGraph::setScatterSize(double size)
{
  mScatterSize = size;
}

/*! 
  If the scatter style (\ref setScatterStyle) is set to ssPixmap, this function defines the QPixmap
  that will be drawn centered on the data point coordinate.
  
  \see ScatterStyle
*/
void QCPGraph::setScatterPixmap(const QPixmap &pixmap)
{
  mScatterPixmap = pixmap;
}

/*!
  Sets which kind of error bars (Key Error, Value Error or both) should be drawn on each data
  point. If you set \a errorType to something other than \ref etNone, make sure to actually pass
  error data via the specific setData functions along with the data points (e.g. \ref
  setDataValueError, \ref setDataKeyError, \ref setDataBothError).

  \see ErrorType
*/
void QCPGraph::setErrorType(ErrorType errorType)
{
  mErrorType = errorType;
}

/*!
  Sets the pen with which the error bars will be drawn.
  \see setErrorBarSize, setErrorType
*/
void QCPGraph::setErrorPen(const QPen &pen)
{
  mErrorPen = pen;
}

/*! 
  Sets the width of the handles at both ends of an error bar in pixels.
*/
void QCPGraph::setErrorBarSize(double size)
{
  mErrorBarSize = size;
}

/*! 
  If \a enabled is set to true, the error bar will not be drawn as a solid line under the scatter symbol but
  leave some free space around the symbol.
  
  This feature uses the current scatter size (\ref setScatterSize) to determine the size of the
  area to leave blank. So when drawing Pixmaps as scatter points (\ref QCP::ssPixmap), the scatter size
  must be set manually to a value corresponding to the size of the Pixmap, if the error bars should
  leave gaps to its boundaries.
*/
void QCPGraph::setErrorBarSkipSymbol(bool enabled)
{
  mErrorBarSkipSymbol = enabled;
}

/*! 
  Sets the target graph for filling the area between this graph and \a targetGraph with the current
  brush (\ref setBrush).
  
  When \a targetGraph is set to 0, a normal graph fill will be produced. This means, when the brush
  is not Qt::NoBrush or fully transparent, a fill all the way to the zero-value-line parallel to
  the key axis of this graph will be drawn. To disable any filling, set the brush to Qt::NoBrush.
  \see setBrush
*/
void QCPGraph::setChannelFillGraph(QCPGraph *targetGraph)
{
  // prevent setting channel target to this graph itself:
  if (targetGraph == this)
  {
    qDebug() << Q_FUNC_INFO << "targetGraph is this graph itself";
    mChannelFillGraph = 0;
    return;
  }
  // prevent setting channel target to a graph not in the plot:
  if (targetGraph && targetGraph->mParentPlot != mParentPlot)
  {
    qDebug() << Q_FUNC_INFO << "targetGraph not in same plot";
    mChannelFillGraph = 0;
    return;
  }
  
  mChannelFillGraph = targetGraph;
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  \see removeData
*/
void QCPGraph::addData(const QCPDataMap &dataMap)
{
  mData->unite(dataMap);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  \see removeData
*/
void QCPGraph::addData(const QCPData &data)
{
  mData->insertMulti(data.key, data);
}

/*! \overload
  Adds the provided single data point as \a key and \a value pair to the current data.
  \see removeData
*/
void QCPGraph::addData(double key, double value)
{
  QCPData newData;
  newData.key = key;
  newData.value = value;
  mData->insertMulti(newData.key, newData);
}

/*! \overload
  Adds the provided data points as \a key and \a value pairs to the current data.
  \see removeData
*/
void QCPGraph::addData(const QVector<double> &keys, const QVector<double> &values)
{
  int n = qMin(keys.size(), values.size());
  QCPData newData;
  for (int i=0; i<n; ++i)
  {
    newData.key = keys[i];
    newData.value = values[i];
    mData->insertMulti(newData.key, newData);
  }
}

/*!
  Removes all data points with keys smaller than \a key.
  \see addData, clearData
*/
void QCPGraph::removeDataBefore(double key)
{
  QCPDataMap::iterator it = mData->begin();
  while (it != mData->end() && it.key() < key)
    it = mData->erase(it);
}

/*!
  Removes all data points with keys greater than \a key.
  \see addData, clearData
*/
void QCPGraph::removeDataAfter(double key)
{
  if (mData->isEmpty()) return;
  QCPDataMap::iterator it = mData->upperBound(key);
  while (it != mData->end())
    it = mData->erase(it);
}

/*!
  Removes all data points with keys between \a fromKey and \a toKey.
  if \a fromKey is greater or equal to \a toKey, the function does nothing. To remove
  a single data point with known key, use \ref removeData(double key).
  
  \see addData, clearData
*/
void QCPGraph::removeData(double fromKey, double toKey)
{
  if (fromKey >= toKey || mData->isEmpty()) return;
  QCPDataMap::iterator it = mData->upperBound(fromKey);
  QCPDataMap::iterator itEnd = mData->upperBound(toKey);
  while (it != itEnd)
    it = mData->erase(it);
}

/*! \overload
  
  Removes a single data point at \a key. If the position is not known with absolute precision,
  consider using \ref removeData(double fromKey, double toKey) with a small fuzziness interval around
  the suspected position, depeding on the precision with which the key is known.

  \see addData, clearData
*/
void QCPGraph::removeData(double key)
{
  mData->remove(key);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPGraph::clearData()
{
  mData->clear();
}

/* inherits documentation from base class */
double QCPGraph::selectTest(const QPointF &pos) const
{
  if (mData->isEmpty() || !mVisible)
    return -1;
  
  return pointDistance(pos);
}

/*! \overload
  
  Allows to define whether error bars are taken into consideration when determining the new axis
  range.
*/
void QCPGraph::rescaleAxes(bool onlyEnlarge, bool includeErrorBars) const
{
  rescaleKeyAxis(onlyEnlarge, includeErrorBars);
  rescaleValueAxis(onlyEnlarge, includeErrorBars);
}

/*! \overload
  
  Allows to define whether error bars (of kind \ref QCPGraph::etKey) are taken into consideration
  when determining the new axis range.
*/
void QCPGraph::rescaleKeyAxis(bool onlyEnlarge, bool includeErrorBars) const
{
  // this code is a copy of QCPAbstractPlottable::rescaleKeyAxis with the only change
  // that getKeyRange is passed the includeErrorBars value.
  if (mData->isEmpty()) return;

  SignDomain signDomain = sdBoth;
  if (mKeyAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mKeyAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getKeyRange(validRange, signDomain, includeErrorBars);
  
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mKeyAxis->range().lower < newRange.lower)
        newRange.lower = mKeyAxis->range().lower;
      if (mKeyAxis->range().upper > newRange.upper)
        newRange.upper = mKeyAxis->range().upper;
    }
    mKeyAxis->setRange(newRange);
  }
}

/*! \overload
  
  Allows to define whether error bars (of kind \ref QCPGraph::etValue) are taken into consideration
  when determining the new axis range.
*/
void QCPGraph::rescaleValueAxis(bool onlyEnlarge, bool includeErrorBars) const
{
  // this code is a copy of QCPAbstractPlottable::rescaleValueAxis with the only change
  // is that getValueRange is passed the includeErrorBars value.
  if (mData->isEmpty()) return;

  SignDomain signDomain = sdBoth;
  if (mValueAxis->scaleType() == QCPAxis::stLogarithmic)
    signDomain = (mValueAxis->range().upper < 0 ? sdNegative : sdPositive);
  
  bool validRange;
  QCPRange newRange = getValueRange(validRange, signDomain, includeErrorBars);
  
  if (validRange)
  {
    if (onlyEnlarge)
    {
      if (mValueAxis->range().lower < newRange.lower)
        newRange.lower = mValueAxis->range().lower;
      if (mValueAxis->range().upper > newRange.upper)
        newRange.upper = mValueAxis->range().upper;
    }
    mValueAxis->setRange(newRange);
  }
}

/* inherits documentation from base class */
void QCPGraph::draw(QCPPainter *painter)
{
  if (mKeyAxis->range().size() <= 0 || mData->isEmpty()) return;
  if (mLineStyle == lsNone && mScatterStyle == QCP::ssNone) return;
  
  // allocate line and (if necessary) point vectors:
  QVector<QPointF> *lineData = new QVector<QPointF>;
  QVector<QCPData> *pointData = 0;
  if (mScatterStyle != QCP::ssNone)
    pointData = new QVector<QCPData>;
  
  // fill vectors with data appropriate to plot style:
  getPlotData(lineData, pointData);

  // draw fill of graph:
  drawFill(painter, lineData);
  
  // draw line:
  if (mLineStyle == lsImpulse)
    drawImpulsePlot(painter, lineData);
  else if (mLineStyle != lsNone)
    drawLinePlot(painter, lineData); // also step plots can be drawn as a line plot
  
  // draw scatters:
  if (pointData)
    drawScatterPlot(painter, pointData);
  
  // free allocated line and point vectors:
  delete lineData;
  if (pointData)
    delete pointData;
}

/* inherits documentation from base class */
void QCPGraph::drawLegendIcon(QCPPainter *painter, const QRect &rect) const
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

/*! 
  \internal
  This function branches out to the line style specific "get(...)PlotData" functions, according to the
  line style of the graph.
  \param lineData will be filled with raw points that will be drawn with the according draw functions, e.g. \ref drawLinePlot and \ref drawImpulsePlot.
  These aren't necessarily the original data points, since for step plots for example, additional points are needed for drawing lines that make up steps.
  If the line style of the graph is \ref lsNone, the \a lineData vector will be left untouched.
  \param pointData will be filled with the original data points so \ref drawScatterPlot can draw the scatter symbols accordingly. If no scatters need to be
  drawn, i.e. scatter style is \ref QCP::ssNone, pass 0 as \a pointData, and this step will be skipped.
  
  \see getScatterPlotData, getLinePlotData, getStepLeftPlotData, getStepRightPlotData, getStepCenterPlotData, getImpulsePlotData
*/
void QCPGraph::getPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  switch(mLineStyle)
  {
    case lsNone: getScatterPlotData(pointData); break;
    case lsLine: getLinePlotData(lineData, pointData); break;
    case lsStepLeft: getStepLeftPlotData(lineData, pointData); break;
    case lsStepRight: getStepRightPlotData(lineData, pointData); break;
    case lsStepCenter: getStepCenterPlotData(lineData, pointData); break;
    case lsImpulse: getImpulsePlotData(lineData, pointData); break;
  }
}

/*! 
  \internal
  If line style is \ref lsNone and scatter style is not \ref QCP::ssNone, this function serves at providing the
  visible data points in \a pointData, so the \ref drawScatterPlot function can draw the scatter points
  accordingly.
  
  If line style is not \ref lsNone, this function is not called and the data for the scatter points
  are (if needed) calculated inside the corresponding other "get(...)PlotData" functions.
  \see drawScatterPlot
*/
void QCPGraph::getScatterPlotData(QVector<QCPData> *pointData) const
{
  if (!pointData) return;
  
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (pointData)
    pointData->resize(dataCount);

  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    while (it != upperEnd)
    {
      (*pointData)[i] = it.value();
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    while (it != upperEnd)
    {
      (*pointData)[i] = it.value();
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a normal linearly connected plot in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getLinePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  { 
    // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
    lineData->reserve(dataCount+2);
    lineData->resize(dataCount);
  }
  if (pointData)
    pointData->resize(dataCount);

  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    while (it != upperEnd)
    {
      if (pointData)
        (*pointData)[i] = it.value();
      (*lineData)[i].setX(mValueAxis->coordToPixel(it.value().value));
      (*lineData)[i].setY(mKeyAxis->coordToPixel(it.key()));
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    while (it != upperEnd)
    {
      if (pointData)
        (*pointData)[i] = it.value();
      (*lineData)[i].setX(mKeyAxis->coordToPixel(it.key()));
      (*lineData)[i].setY(mValueAxis->coordToPixel(it.value().value));
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a step plot with left oriented steps in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getStepLeftPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
    // multiplied by 2 because step plot needs two polyline points per one actual data point
    lineData->reserve(dataCount*2+2);
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a step plot with right oriented steps in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getStepRightPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
    // multiplied by 2 because step plot needs two polyline points per one actual data point
    lineData->reserve(dataCount*2+2);
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double value;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      value = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(value);
      (*lineData)[i].setY(lastKey);
      ++i;
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(value);
      (*lineData)[i].setY(lastKey);
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double value;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      value = mValueAxis->coordToPixel(it.value().value);
      (*lineData)[i].setX(lastKey);
      (*lineData)[i].setY(value);
      ++i;
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(lastKey);
      (*lineData)[i].setY(value);
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Places the raw data points needed for a step plot with centered steps in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawLinePlot
*/
void QCPGraph::getStepCenterPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // added 2 to reserve memory for lower/upper fill base points that might be needed for base fill
    // multiplied by 2 because step plot needs two polyline points per one actual data point
    lineData->reserve(dataCount*2+2);
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    if (pointData)
    {
      (*pointData)[ipoint] = it.value();
      ++ipoint;
    }
    (*lineData)[i].setX(lastValue);
    (*lineData)[i].setY(lastKey);
    ++it;
    ++i;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = (mKeyAxis->coordToPixel(it.key())-lastKey)*0.5 + lastKey;
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(lastValue);
      (*lineData)[i].setY(key);
      ++it;
      ++i;
    }
    (*lineData)[i].setX(lastValue);
    (*lineData)[i].setY(lastKey);
  } else // key axis is horizontal
  {
    double lastKey = mKeyAxis->coordToPixel(it.key());
    double lastValue = mValueAxis->coordToPixel(it.value().value);
    double key;
    if (pointData)
    {
      (*pointData)[ipoint] = it.value();
      ++ipoint;
    }
    (*lineData)[i].setX(lastKey);
    (*lineData)[i].setY(lastValue);
    ++it;
    ++i;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = (mKeyAxis->coordToPixel(it.key())-lastKey)*0.5 + lastKey;
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++i;
      lastValue = mValueAxis->coordToPixel(it.value().value);
      lastKey = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(lastValue);
      ++it;
      ++i;
    }
    (*lineData)[i].setX(lastKey);
    (*lineData)[i].setY(lastValue);
  }
}

/*! 
  \internal
  Places the raw data points needed for an impulse plot in \a lineData.

  As for all plot data retrieval functions, \a pointData just contains all unaltered data (scatter)
  points that are visible, for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. scatter style \ref QCP::ssNone), pass 0 as \a pointData, and the function will skip
  filling the vector.
  \see drawImpulsePlot
*/
void QCPGraph::getImpulsePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const
{
  // get visible data range:
  QCPDataMap::const_iterator lower, upper;
  int dataCount;
  getVisibleDataBounds(lower, upper, dataCount);
  // prepare vectors:
  if (lineData)
  {
    // no need to reserve 2 extra points, because there is no fill for impulse plot
    lineData->resize(dataCount*2);
  }
  if (pointData)
    pointData->resize(dataCount);
  
  // position data points:
  QCPDataMap::const_iterator it = lower;
  QCPDataMap::const_iterator upperEnd = upper+1;
  int i = 0;
  int ipoint = 0;
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    double zeroPointX = mValueAxis->coordToPixel(0);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(zeroPointX);
      (*lineData)[i].setY(key);
      ++i;
      (*lineData)[i].setX(mValueAxis->coordToPixel(it.value().value));
      (*lineData)[i].setY(key);
      ++i;
      ++it;
    }
  } else // key axis is horizontal
  {
    double zeroPointY = mValueAxis->coordToPixel(0);
    double key;
    while (it != upperEnd)
    {
      if (pointData)
      {
        (*pointData)[ipoint] = it.value();
        ++ipoint;
      }
      key = mKeyAxis->coordToPixel(it.key());
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(zeroPointY);
      ++i;
      (*lineData)[i].setX(key);
      (*lineData)[i].setY(mValueAxis->coordToPixel(it.value().value));
      ++i;
      ++it;
    }
  }
}

/*! 
  \internal
  Draws the fill of the graph with the specified brush. If the fill is a normal "base" fill, i.e.
  under the graph toward the zero-value-line, only the \a lineData is required (and two extra points
  at the zero-value-line, which are added by \ref addFillBasePoints and removed by \ref removeFillBasePoints
  after the fill drawing is done).
  
  If the fill is a channel fill between this graph and another graph (mChannelFillGraph), the more complex
  polygon is calculated with the \ref getChannelFillPolygon function.
  \see drawLinePlot
*/
void QCPGraph::drawFill(QCPPainter *painter, QVector<QPointF> *lineData) const
{
  if (mLineStyle == lsImpulse) return; // fill doesn't make sense for impulse plot
  if (mainBrush().style() == Qt::NoBrush || mainBrush().color().alpha() == 0) return;
  
  applyFillAntialiasingHint(painter);
  if (!mChannelFillGraph)
  {
    // draw base fill under graph, fill goes all the way to the zero-value-line:
    addFillBasePoints(lineData);
    painter->setPen(Qt::NoPen);
    painter->setBrush(mainBrush());
    painter->drawPolygon(QPolygonF(*lineData));
    removeFillBasePoints(lineData);
  } else
  {
    // draw channel fill between this graph and mChannelFillGraph:
    painter->setPen(Qt::NoPen);
    painter->setBrush(mainBrush());
    painter->drawPolygon(getChannelFillPolygon(lineData));
  }
}

/*! \internal
  
  Draws scatter symbols at every data point passed in \a pointData. scatter symbols are independent of
  the line style and are always drawn if scatter style is not \ref QCP::ssNone. Hence, the \a pointData vector
  is outputted by all "get(...)PlotData" functions, together with the (line style dependent) line data.
  \see drawLinePlot, drawImpulsePlot
*/
void QCPGraph::drawScatterPlot(QCPPainter *painter, QVector<QCPData> *pointData) const
{
  // draw error bars:
  if (mErrorType != etNone)
  {
    applyErrorBarsAntialiasingHint(painter);
    painter->setPen(mErrorPen);
    if (mKeyAxis->orientation() == Qt::Vertical)
    {
      for (int i=0; i<pointData->size(); ++i)
        drawError(painter, mValueAxis->coordToPixel(pointData->at(i).value), mKeyAxis->coordToPixel(pointData->at(i).key), pointData->at(i));
    } else
    {
      for (int i=0; i<pointData->size(); ++i)
        drawError(painter, mKeyAxis->coordToPixel(pointData->at(i).key), mValueAxis->coordToPixel(pointData->at(i).value), pointData->at(i));
    }
  }
  
  // draw scatter point symbols:
  applyScattersAntialiasingHint(painter);
  painter->setPen(mainPen());
  painter->setBrush(mainBrush());
  painter->setScatterPixmap(mScatterPixmap);
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    for (int i=0; i<pointData->size(); ++i)
      painter->drawScatter(mValueAxis->coordToPixel(pointData->at(i).value), mKeyAxis->coordToPixel(pointData->at(i).key), mScatterSize, mScatterStyle);
  } else
  {
    for (int i=0; i<pointData->size(); ++i)
      painter->drawScatter(mKeyAxis->coordToPixel(pointData->at(i).key), mValueAxis->coordToPixel(pointData->at(i).value), mScatterSize, mScatterStyle);
  }
}

/*! 
  \internal
  Draws line graphs from the provided data. It connects all points in \a lineData, which
  was created by one of the "get(...)PlotData" functions for line styles that require simple line
  connections between the point vector they create. These are for example \ref getLinePlotData, \ref
  getStepLeftPlotData, \ref getStepRightPlotData and \ref getStepCenterPlotData.
  \see drawScatterPlot, drawImpulsePlot
*/
void QCPGraph::drawLinePlot(QCPPainter *painter, QVector<QPointF> *lineData) const
{
  // draw line of graph:
  if (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
  {
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mainPen());
    painter->setBrush(Qt::NoBrush);
    
    /* Draws polyline in batches, currently not used:
    int p = 0;
    while (p < lineData->size())
    {
      int batch = qMin(25, lineData->size()-p);
      if (p != 0)
      {
        ++batch;
        --p; // to draw the connection lines between two batches
      }
      painter->drawPolyline(lineData->constData()+p, batch);
      p += batch;
    }
    */
    
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
}

/*! 
  \internal
  Draws impulses graphs from the provided data, i.e. it connects all line pairs in \a lineData, which was
  created by \ref getImpulsePlotData.
  \see drawScatterPlot, drawLinePlot
*/
void QCPGraph::drawImpulsePlot(QCPPainter *painter, QVector<QPointF> *lineData) const
{
  // draw impulses:
  if (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
  {
    applyDefaultAntialiasingHint(painter);
    QPen pen = mainPen();
    pen.setCapStyle(Qt::FlatCap); // so impulse line doesn't reach beyond zero-line
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawLines(*lineData);
  }
}

/*! 
  \internal
  called by the scatter drawing function (\ref drawScatterPlot) to draw the error bars on one data
  point. \a x and \a y pixel positions of the data point are passed since they are already known in
  pixel coordinates in the drawing function, so we save some extra coordToPixel transforms here. \a
  data is therefore only used for the errors, not key and value.
*/
void QCPGraph::drawError(QCPPainter *painter, double x, double y, const QCPData &data) const
{
  double a, b; // positions of error bar bounds in pixels
  double barWidthHalf = mErrorBarSize*0.5;
  double skipSymbolMargin = mScatterSize*1.25; // pixels left blank per side, when mErrorBarSkipSymbol is true

  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    // draw key error vertically and value error horizontally
    if (mErrorType == etKey || mErrorType == etBoth)
    {
      a = mKeyAxis->coordToPixel(data.key-data.keyErrorMinus);
      b = mKeyAxis->coordToPixel(data.key+data.keyErrorPlus);
      if (mKeyAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (a-y > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(x, a, x, y+skipSymbolMargin));
        if (y-b > skipSymbolMargin) 
          painter->drawLine(QLineF(x, y-skipSymbolMargin, x, b));
      } else
        painter->drawLine(QLineF(x, a, x, b));
      // draw handles:
      painter->drawLine(QLineF(x-barWidthHalf, a, x+barWidthHalf, a));
      painter->drawLine(QLineF(x-barWidthHalf, b, x+barWidthHalf, b));
    }
    if (mErrorType == etValue || mErrorType == etBoth)
    {
      a = mValueAxis->coordToPixel(data.value-data.valueErrorMinus);
      b = mValueAxis->coordToPixel(data.value+data.valueErrorPlus);
      if (mValueAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (x-a > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(a, y, x-skipSymbolMargin, y));
        if (b-x > skipSymbolMargin)
          painter->drawLine(QLineF(x+skipSymbolMargin, y, b, y));
      } else
        painter->drawLine(QLineF(a, y, b, y));
      // draw handles:
      painter->drawLine(QLineF(a, y-barWidthHalf, a, y+barWidthHalf));
      painter->drawLine(QLineF(b, y-barWidthHalf, b, y+barWidthHalf));
    }
  } else // mKeyAxis->orientation() is Qt::Horizontal
  {
    // draw value error vertically and key error horizontally
    if (mErrorType == etKey || mErrorType == etBoth)
    {
      a = mKeyAxis->coordToPixel(data.key-data.keyErrorMinus);
      b = mKeyAxis->coordToPixel(data.key+data.keyErrorPlus);
      if (mKeyAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (x-a > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(a, y, x-skipSymbolMargin, y));
        if (b-x > skipSymbolMargin)
          painter->drawLine(QLineF(x+skipSymbolMargin, y, b, y));
      } else
        painter->drawLine(QLineF(a, y, b, y));
      // draw handles:
      painter->drawLine(QLineF(a, y-barWidthHalf, a, y+barWidthHalf));
      painter->drawLine(QLineF(b, y-barWidthHalf, b, y+barWidthHalf));
    }
    if (mErrorType == etValue || mErrorType == etBoth)
    {
      a = mValueAxis->coordToPixel(data.value-data.valueErrorMinus);
      b = mValueAxis->coordToPixel(data.value+data.valueErrorPlus);
      if (mValueAxis->rangeReversed())
        qSwap(a,b);
      // draw spine:
      if (mErrorBarSkipSymbol)
      {
        if (a-y > skipSymbolMargin) // don't draw spine if error is so small it's within skipSymbolmargin
          painter->drawLine(QLineF(x, a, x, y+skipSymbolMargin));
        if (y-b > skipSymbolMargin)
          painter->drawLine(QLineF(x, y-skipSymbolMargin, x, b));
      } else
        painter->drawLine(QLineF(x, a, x, b));
      // draw handles:
      painter->drawLine(QLineF(x-barWidthHalf, a, x+barWidthHalf, a));
      painter->drawLine(QLineF(x-barWidthHalf, b, x+barWidthHalf, b));
    }
  }
}

/*! 
  \internal
  called by the specific plot data generating functions "get(...)PlotData" to determine
  which data range is visible, so only that needs to be processed.
  
  \param[out] lower returns an iterator to the lowest data point that needs to be taken into account
  when plotting. Note that in order to get a clean plot all the way to the edge of the axes, \a lower
  may still be outside the visible range.
  \param[out] upper returns an iterator to the highest data point. Same as before, \a upper may also
  lie outside of the visible range.
  \param[out] count number of data points that need plotting, i.e. points between \a lower and \a upper,
  including them. This is useful for allocating the array of QPointFs in the specific drawing functions.
*/
void QCPGraph::getVisibleDataBounds(QCPDataMap::const_iterator &lower, QCPDataMap::const_iterator &upper, int &count) const
{
  // get visible data range as QMap iterators
  QCPDataMap::const_iterator lbound = mData->lowerBound(mKeyAxis->range().lower);
  QCPDataMap::const_iterator ubound = mData->upperBound(mKeyAxis->range().upper)-1;
  bool lowoutlier = lbound != mData->constBegin(); // indicates whether there exist points below axis range
  bool highoutlier = ubound+1 != mData->constEnd(); // indicates whether there exist points above axis range
  lower = (lowoutlier ? lbound-1 : lbound); // data pointrange that will be actually drawn
  upper = (highoutlier ? ubound+1 : ubound); // data pointrange that will be actually drawn
  
  // count number of points in range lower to upper (including them), so we can allocate array for them in draw functions:
  QCPDataMap::const_iterator it = lower;
  count = 1;
  while (it != upper)
  {
    ++it;
    ++count;
  }
}

/*! 
  \internal
  The line data vector generated by e.g. getLinePlotData contains only the line
  that connects the data points. If the graph needs to be filled, two additional points
  need to be added at the value-zero-line in the lower and upper key positions, the graph
  reaches. This function calculates these points and adds them to the end of \a lineData.
  Since the fill is typically drawn before the line stroke, these added points need to
  be removed again after the fill is done, with the removeFillBasePoints function.
  
  The expanding of \a lineData by two points will not cause unnecessary memory reallocations,
  because the data vector generation functions (getLinePlotData etc.) reserve two extra points
  when they allocate memory for \a lineData.
  \see removeFillBasePoints, lowerFillBasePoint, upperFillBasePoint
*/
void QCPGraph::addFillBasePoints(QVector<QPointF> *lineData) const
{
  // append points that close the polygon fill at the key axis:
  if (mKeyAxis->orientation() == Qt::Vertical)
  {
    *lineData << upperFillBasePoint(lineData->last().y());
    *lineData << lowerFillBasePoint(lineData->first().y());
  } else
  {
    *lineData << upperFillBasePoint(lineData->last().x());
    *lineData << lowerFillBasePoint(lineData->first().x());
  }
}

/*! 
  \internal
  removes the two points from \a lineData that were added by addFillBasePoints.
  \see addFillBasePoints, lowerFillBasePoint, upperFillBasePoint
*/
void QCPGraph::removeFillBasePoints(QVector<QPointF> *lineData) const
{
  lineData->remove(lineData->size()-2, 2);
}

/*! 
  \internal
  called by addFillBasePoints to conveniently assign the point which closes the fill
  polygon on the lower side of the zero-value-line parallel to the key axis.
  The logarithmic axis scale case is a bit special, since the zero-value-line in pixel coordinates
  is in positive or negative infinity. So this case is handled separately by just closing the
  fill polygon on the axis which lies in the direction towards the zero value.
  
  \param lowerKey pixel position of the lower key of the point. Depending on whether the key axis
  is horizontal or vertical, \a lowerKey will end up as the x or y value of the returned point,
  respectively.
  \see upperFillBasePoint, addFillBasePoints
*/
QPointF QCPGraph::lowerFillBasePoint(double lowerKey) const
{
  QPointF point;
  if (mValueAxis->scaleType() == QCPAxis::stLinear)
  {
    if (mKeyAxis->axisType() == QCPAxis::atLeft)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(lowerKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atRight)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(lowerKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop)
    {
      point.setX(lowerKey);
      point.setY(mValueAxis->coordToPixel(0));
    } else if (mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(lowerKey);
      point.setY(mValueAxis->coordToPixel(0));
    }
  } else // mValueAxis->mScaleType == QCPAxis::stLogarithmic
  {
    // In logarithmic scaling we can't just draw to value zero so we just fill all the way
    // to the axis which is in the direction towards zero
    if (mKeyAxis->orientation() == Qt::Vertical)
    {
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setX(mKeyAxis->axisRect().right());
      else
        point.setX(mKeyAxis->axisRect().left());
      point.setY(lowerKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop || mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(lowerKey);
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setY(mKeyAxis->axisRect().top());
      else
        point.setY(mKeyAxis->axisRect().bottom());
    }
  }
  return point;
}

/*! 
  \internal
  called by addFillBasePoints to conveniently assign the point which closes the fill
  polygon on the upper side of the zero-value-line parallel to the key axis. The logarithmic axis
  scale case is a bit special, since the zero-value-line in pixel coordinates is in positive or
  negative infinity. So this case is handled separately by just closing the fill polygon on the
  axis which lies in the direction towards the zero value.
  
  \param upperKey pixel position of the upper key of the point. Depending on whether the key axis
  is horizontal or vertical, \a upperKey will end up as the x or y value of the returned point,
  respectively.
  \see lowerFillBasePoint, addFillBasePoints
*/
QPointF QCPGraph::upperFillBasePoint(double upperKey) const
{
  QPointF point;
  if (mValueAxis->scaleType() == QCPAxis::stLinear)
  {
    if (mKeyAxis->axisType() == QCPAxis::atLeft)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(upperKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atRight)
    {
      point.setX(mValueAxis->coordToPixel(0));
      point.setY(upperKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop)
    {
      point.setX(upperKey);
      point.setY(mValueAxis->coordToPixel(0));
    } else if (mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(upperKey);
      point.setY(mValueAxis->coordToPixel(0));
    }
  } else // mValueAxis->mScaleType == QCPAxis::stLogarithmic
  {
    // In logarithmic scaling we can't just draw to value 0 so we just fill all the way
    // to the axis which is in the direction towards 0
    if (mKeyAxis->orientation() == Qt::Vertical)
    {
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setX(mKeyAxis->axisRect().right());
      else
        point.setX(mKeyAxis->axisRect().left());
      point.setY(upperKey);
    } else if (mKeyAxis->axisType() == QCPAxis::atTop || mKeyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(upperKey);
      if ((mValueAxis->range().upper < 0 && !mValueAxis->rangeReversed()) ||
          (mValueAxis->range().upper > 0 && mValueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setY(mKeyAxis->axisRect().top());
      else
        point.setY(mKeyAxis->axisRect().bottom());
    }
  }
  return point;
}

/*! \internal
  
  Generates the polygon needed for drawing channel fills between this graph (data passed via \a
  lineData) and the graph specified by mChannelFillGraph (data generated by calling its \ref
  getPlotData function). May return an empty polygon if the key ranges have no overlap or fill
  target graph and this graph don't have same orientation (i.e. both key axes horizontal or both
  key axes vertical). For increased performance (due to implicit sharing), keep the returned QPolygonF
  const.
*/
const QPolygonF QCPGraph::getChannelFillPolygon(const QVector<QPointF> *lineData) const
{
  if (mChannelFillGraph->mKeyAxis->orientation() != mKeyAxis->orientation())
    return QPolygonF(); // don't have same axis orientation, can't fill that (Note: if keyAxis fits, valueAxis will fit too, because it's always orthogonal to keyAxis)
  
  if (lineData->isEmpty()) return QPolygonF();
  QVector<QPointF> otherData;
  mChannelFillGraph->getPlotData(&otherData, 0);
  if (otherData.isEmpty()) return QPolygonF();
  QVector<QPointF> thisData;
  thisData.reserve(lineData->size()+otherData.size()); // because we will join both vectors at end of this function
  for (int i=0; i<lineData->size(); ++i) // don't use the vector<<(vector),  it squeezes internally, which ruins the performance tuning with reserve()
    thisData << lineData->at(i);
  
  // pointers to be able to swap them, depending which data range needs cropping:
  QVector<QPointF> *staticData = &thisData;
  QVector<QPointF> *croppedData = &otherData;
  
  // crop both vectors to ranges in which the keys overlap (which coord is key, depends on axisType):
  if (mKeyAxis->orientation() == Qt::Horizontal)
  {
    // x is key
    // if an axis range is reversed, the data point keys will be descending. Reverse them, since following algorithm assumes ascending keys:
    if (staticData->first().x() > staticData->last().x())
    {
      int size = staticData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*staticData)[i], (*staticData)[size-1-i]);
    }
    if (croppedData->first().x() > croppedData->last().x())
    {
      int size = croppedData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*croppedData)[i], (*croppedData)[size-1-i]);
    }
    // crop lower bound:
    if (staticData->first().x() < croppedData->first().x()) // other one must be cropped
      qSwap(staticData, croppedData);
    int lowBound = findIndexBelowX(croppedData, staticData->first().x());
    if (lowBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(0, lowBound);
    // set lowest point of cropped data to fit exactly key position of first static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    double slope;
    if (croppedData->at(1).x()-croppedData->at(0).x() != 0)
      slope = (croppedData->at(1).y()-croppedData->at(0).y())/(croppedData->at(1).x()-croppedData->at(0).x());
    else
      slope = 0;
    (*croppedData)[0].setY(croppedData->at(0).y()+slope*(staticData->first().x()-croppedData->at(0).x()));
    (*croppedData)[0].setX(staticData->first().x());
    
    // crop upper bound:
    if (staticData->last().x() > croppedData->last().x()) // other one must be cropped
      qSwap(staticData, croppedData);
    int highBound = findIndexAboveX(croppedData, staticData->last().x());
    if (highBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
    // set highest point of cropped data to fit exactly key position of last static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    int li = croppedData->size()-1; // last index
    if (croppedData->at(li).x()-croppedData->at(li-1).x() != 0)
      slope = (croppedData->at(li).y()-croppedData->at(li-1).y())/(croppedData->at(li).x()-croppedData->at(li-1).x());
    else
      slope = 0;
    (*croppedData)[li].setY(croppedData->at(li-1).y()+slope*(staticData->last().x()-croppedData->at(li-1).x()));
    (*croppedData)[li].setX(staticData->last().x());
  } else // mKeyAxis->orientation() == Qt::Vertical
  {
    // y is key
    // similar to "x is key" but switched x,y. Further, lower/upper meaning is inverted compared to x,
    // because in pixel coordinates, y increases from top to bottom, not bottom to top like data coordinate.
    // if an axis range is reversed, the data point keys will be descending. Reverse them, since following algorithm assumes ascending keys:
    if (staticData->first().y() < staticData->last().y())
    {
      int size = staticData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*staticData)[i], (*staticData)[size-1-i]);
    }
    if (croppedData->first().y() < croppedData->last().y())
    {
      int size = croppedData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*croppedData)[i], (*croppedData)[size-1-i]);
    }
    // crop lower bound:
    if (staticData->first().y() > croppedData->first().y()) // other one must be cropped
      qSwap(staticData, croppedData);
    int lowBound = findIndexAboveY(croppedData, staticData->first().y());
    if (lowBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(0, lowBound);
    // set lowest point of cropped data to fit exactly key position of first static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    double slope;
    if (croppedData->at(1).y()-croppedData->at(0).y() != 0) // avoid division by zero in step plots
      slope = (croppedData->at(1).x()-croppedData->at(0).x())/(croppedData->at(1).y()-croppedData->at(0).y());
    else
      slope = 0;
    (*croppedData)[0].setX(croppedData->at(0).x()+slope*(staticData->first().y()-croppedData->at(0).y()));
    (*croppedData)[0].setY(staticData->first().y());
    
    // crop upper bound:
    if (staticData->last().y() < croppedData->last().y()) // other one must be cropped
      qSwap(staticData, croppedData);
    int highBound = findIndexBelowY(croppedData, staticData->last().y());
    if (highBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
    // set highest point of cropped data to fit exactly key position of last static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    int li = croppedData->size()-1; // last index
    if (croppedData->at(li).y()-croppedData->at(li-1).y() != 0) // avoid division by zero in step plots
      slope = (croppedData->at(li).x()-croppedData->at(li-1).x())/(croppedData->at(li).y()-croppedData->at(li-1).y());
    else
      slope = 0;
    (*croppedData)[li].setX(croppedData->at(li-1).x()+slope*(staticData->last().y()-croppedData->at(li-1).y()));
    (*croppedData)[li].setY(staticData->last().y());
  }
  
  // return joined:
  for (int i=otherData.size()-1; i>=0; --i) // insert reversed, otherwise the polygon will be twisted
    thisData << otherData.at(i);
  return QPolygonF(thisData);
}

/*! \internal
  
  Finds the smallest index of \a data, whose points x value is just above \a x.
  Assumes x values in \a data points are ordered ascending, as is the case
  when plotting with horizontal key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexAboveX(const QVector<QPointF> *data, double x) const
{
  for (int i=data->size()-1; i>=0; --i)
  {
    if (data->at(i).x() < x)
    {
      if (i<data->size()-1)
        return i+1;
      else
        return data->size()-1;
    }
  }
  return -1;
}

/*! \internal
  
  Finds the greatest index of \a data, whose points x value is just below \a x.
  Assumes x values in \a data points are ordered ascending, as is the case
  when plotting with horizontal key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexBelowX(const QVector<QPointF> *data, double x) const
{
  for (int i=0; i<data->size(); ++i)
  {
    if (data->at(i).x() > x)
    {
      if (i>0)
        return i-1;
      else
        return 0;
    }
  }
  return -1;
}

/*! \internal
  
  Finds the smallest index of \a data, whose points y value is just above \a y.
  Assumes y values in \a data points are ordered descending, as is the case
  when plotting with vertical key axis.
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexAboveY(const QVector<QPointF> *data, double y) const
{
  for (int i=0; i<data->size(); ++i)
  {
    if (data->at(i).y() < y)
    {
      if (i>0)
        return i-1;
      else
        return 0;
    }
  }
  return -1;
}

/*! \internal 
  
  Calculates the (minimum) distance (in pixels) the graph's representation has from the given \a
  pixelPoint in pixels. This is used to determine whether the graph was clicked or not, e.g. in
  \ref selectTest.
*/
double QCPGraph::pointDistance(const QPointF &pixelPoint) const
{
  if (mData->isEmpty())
  {
    qDebug() << Q_FUNC_INFO << "requested point distance on graph" << mName << "without data";
    return 500;
  }
  if (mData->size() == 1)
  {
    QPointF dataPoint = coordsToPixels(mData->constBegin().key(), mData->constBegin().value().value);
    return QVector2D(dataPoint-pixelPoint).length();
  }
  
  if (mLineStyle == lsNone && mScatterStyle == QCP::ssNone)
    return 500;
  
  // calculate minimum distances to graph representation:
  if (mLineStyle == lsNone)
  {
    // no line displayed, only calculate distance to scatter points:
    QVector<QCPData> *pointData = new QVector<QCPData>;
    getScatterPlotData(pointData);
    double minDistSqr = std::numeric_limits<double>::max();
    QPointF ptA;
    QPointF ptB = coordsToPixels(pointData->at(0).key, pointData->at(0).value); // getScatterPlotData returns in plot coordinates, so transform to pixels
    for (int i=1; i<pointData->size(); ++i)
    {
      ptA = ptB;
      ptB = coordsToPixels(pointData->at(i).key, pointData->at(i).value);
      double currentDistSqr = distSqrToLine(ptA, ptB, pixelPoint);
      if (currentDistSqr < minDistSqr)
        minDistSqr = currentDistSqr;
    }
    delete pointData;
    return sqrt(minDistSqr);
  } else
  {
    // line displayed calculate distance to line segments:
    QVector<QPointF> *lineData = new QVector<QPointF>;
    getPlotData(lineData, 0); // unlike with getScatterPlotData we get pixel coordinates here
    double minDistSqr = std::numeric_limits<double>::max();
    if (mLineStyle == lsImpulse)
    {
      // impulse plot differs from other line styles in that the lineData points are only pairwise connected:
      for (int i=0; i<lineData->size()-1; i+=2) // iterate pairs
      {
        double currentDistSqr = distSqrToLine(lineData->at(i), lineData->at(i+1), pixelPoint);
        if (currentDistSqr < minDistSqr)
          minDistSqr = currentDistSqr;
      }
    } else 
    {
      // all other line plots (line and step) connect points directly:
      for (int i=0; i<lineData->size()-1; ++i)
      {
        double currentDistSqr = distSqrToLine(lineData->at(i), lineData->at(i+1), pixelPoint);
        if (currentDistSqr < minDistSqr)
          minDistSqr = currentDistSqr;
      }
    }
    delete lineData;
    return sqrt(minDistSqr);
  }
}

/*! \internal
  
  Finds the greatest index of \a data, whose points y value is just below \a y.
  Assumes y values in \a data points are ordered descending, as is the case
  when plotting with vertical key axis (since keys are ordered ascending).
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexBelowY(const QVector<QPointF> *data, double y) const
{
  for (int i=data->size()-1; i>=0; --i)
  {
    if (data->at(i).y() > y)
    {
      if (i<data->size()-1)
        return i+1;
      else
        return data->size()-1;
    }
  }
  return -1;
}

/* inherits documentation from base class */
QCPRange QCPGraph::getKeyRange(bool &validRange, SignDomain inSignDomain) const
{
  // just call the specialized version which takes an additional argument whether error bars
  // should also be taken into consideration for range calculation. We set this to true here.
  return getKeyRange(validRange, inSignDomain, true);
}

/* inherits documentation from base class */
QCPRange QCPGraph::getValueRange(bool &validRange, SignDomain inSignDomain) const
{
  // just call the specialized version which takes an additional argument whether error bars
  // should also be taken into consideration for range calculation. We set this to true here.
  return getValueRange(validRange, inSignDomain, true);
}

/*! \overload
  Allows to specify whether the error bars should be included in the range calculation.
  
  \see getKeyRange(bool &validRange, SignDomain inSignDomain)
*/
QCPRange QCPGraph::getKeyRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current, currentErrorMinus, currentErrorPlus;
  
  if (inSignDomain == sdBoth) // range may be anywhere
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().key;
      currentErrorMinus = (includeErrors ? it.value().keyErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().keyErrorPlus : 0);
      if (current-currentErrorMinus < range.lower || !haveLower)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if (current+currentErrorPlus > range.upper || !haveUpper)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      ++it;
    }
  } else if (inSignDomain == sdNegative) // range may only be in the negative sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().key;
      currentErrorMinus = (includeErrors ? it.value().keyErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().keyErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus < 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus < 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to geht that point.
      {
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  } else if (inSignDomain == sdPositive) // range may only be in the positive sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().key;
      currentErrorMinus = (includeErrors ? it.value().keyErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().keyErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus > 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus > 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to get that point.
      {
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

/*! \overload
  Allows to specify whether the error bars should be included in the range calculation.
  
  \see getValueRange(bool &validRange, SignDomain inSignDomain)
*/
QCPRange QCPGraph::getValueRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const
{
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  
  double current, currentErrorMinus, currentErrorPlus;
  
  if (inSignDomain == sdBoth) // range may be anywhere
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().value;
      currentErrorMinus = (includeErrors ? it.value().valueErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().valueErrorPlus : 0);
      if (current-currentErrorMinus < range.lower || !haveLower)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if (current+currentErrorPlus > range.upper || !haveUpper)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      ++it;
    }
  } else if (inSignDomain == sdNegative) // range may only be in the negative sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().value;
      currentErrorMinus = (includeErrors ? it.value().valueErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().valueErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus < 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus < 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to get that point.
      {
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  } else if (inSignDomain == sdPositive) // range may only be in the positive sign domain
  {
    QCPDataMap::const_iterator it = mData->constBegin();
    while (it != mData->constEnd())
    {
      current = it.value().value;
      currentErrorMinus = (includeErrors ? it.value().valueErrorMinus : 0);
      currentErrorPlus = (includeErrors ? it.value().valueErrorPlus : 0);
      if ((current-currentErrorMinus < range.lower || !haveLower) && current-currentErrorMinus > 0)
      {
        range.lower = current-currentErrorMinus;
        haveLower = true;
      }
      if ((current+currentErrorPlus > range.upper || !haveUpper) && current+currentErrorPlus > 0)
      {
        range.upper = current+currentErrorPlus;
        haveUpper = true;
      }
      if (includeErrors) // in case point is in valid sign domain but errobars stretch beyond it, we still want to geht that point.
      {
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  }
  
  validRange = haveLower && haveUpper;
  return range;
}

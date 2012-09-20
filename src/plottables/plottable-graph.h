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

#ifndef QCP_PLOTTABLE_GRAPH_H
#define QCP_PLOTTABLE_GRAPH_H

#include "../global.h"
#include "../range.h"
#include "../plottable.h"

class QCPPainter;
class QCPAxis;

class QCP_LIB_DECL QCPData
{
public:
  QCPData();
  QCPData(double key, double value);
  double key, value;
  double keyErrorPlus, keyErrorMinus;
  double valueErrorPlus, valueErrorMinus;
};
Q_DECLARE_TYPEINFO(QCPData, Q_MOVABLE_TYPE);

/*! \typedef QCPDataMap
  Container for storing QCPData items in a sorted fashion. The key of the map
  is the key member of the QCPData instance.
  
  This is the container in which QCPGraph holds its data.
  \see QCPData, QCPGraph::setData
*/
typedef QMap<double, QCPData> QCPDataMap;
typedef QMapIterator<double, QCPData> QCPDataMapIterator;
typedef QMutableMapIterator<double, QCPData> QCPDataMutableMapIterator;


class QCP_LIB_DECL QCPGraph : public QCPAbstractPlottable
{
  Q_OBJECT
public:
  /*!
    Defines how the graph's line is represented visually in the plot. The line is drawn with the
    current pen of the graph (\ref setPen).
    \see setLineStyle
  */
  enum LineStyle { lsNone        ///< data points are not connected with any lines (e.g. data only represented
                                 ///< with symbols according to the scatter style, see \ref setScatterStyle)
                  ,lsLine        ///< data points are connected by a straight line
                  ,lsStepLeft    ///< line is drawn as steps where the step height is the value of the left data point
                  ,lsStepRight   ///< line is drawn as steps where the step height is the value of the right data point
                  ,lsStepCenter  ///< line is drawn as steps where the step is in between two data points
                  ,lsImpulse     ///< each data point is represented by a line parallel to the value axis, which reaches from the data point to the zero-value-line
                 };
  Q_ENUMS(LineStyle)
  /*!
    Defines what kind of error bars are drawn for each data point
  */
  enum ErrorType { etNone   ///< No error bars are shown
                  ,etKey    ///< Error bars for the key dimension of the data point are shown
                  ,etValue  ///< Error bars for the value dimension of the data point are shown
                  ,etBoth   ///< Error bars for both key and value dimensions of the data point are shown
                 };
  Q_ENUMS(ErrorType)
  
  explicit QCPGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPGraph();
  
  // getters:
  const QCPDataMap *data() const { return mData; }
  LineStyle lineStyle() const { return mLineStyle; }
  QCP::ScatterStyle scatterStyle() const { return mScatterStyle; }
  double scatterSize() const { return mScatterSize; }
  const QPixmap scatterPixmap() const { return mScatterPixmap; }
  ErrorType errorType() const { return mErrorType; }
  QPen errorPen() const { return mErrorPen; }
  double errorBarSize() const { return mErrorBarSize; }
  bool errorBarSkipSymbol() const { return mErrorBarSkipSymbol; }
  QCPGraph *channelFillGraph() const { return mChannelFillGraph; }
  
  // setters:
  void setData(QCPDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError);
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus);
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueError);
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError, const QVector<double> &valueError);
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setLineStyle(LineStyle ls);
  void setScatterStyle(QCP::ScatterStyle ss);
  void setScatterSize(double size);
  void setScatterPixmap(const QPixmap &pixmap);
  void setErrorType(ErrorType errorType);
  void setErrorPen(const QPen &pen);
  void setErrorBarSize(double size);
  void setErrorBarSkipSymbol(bool enabled);
  void setChannelFillGraph(QCPGraph *targetGraph);
  
  // non-property methods:
  void addData(const QCPDataMap &dataMap);
  void addData(const QCPData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  using QCPAbstractPlottable::rescaleAxes;
  using QCPAbstractPlottable::rescaleKeyAxis;
  using QCPAbstractPlottable::rescaleValueAxis;
  virtual void rescaleAxes(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  virtual void rescaleKeyAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  virtual void rescaleValueAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  
protected:
  QCPDataMap *mData;
  QPen mErrorPen;
  LineStyle mLineStyle;
  QCP::ScatterStyle mScatterStyle;
  double mScatterSize;
  QPixmap mScatterPixmap;
  ErrorType mErrorType;
  double mErrorBarSize;
  bool mErrorBarSkipSymbol;
  QCPGraph *mChannelFillGraph;

  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;

  // functions to generate plot data points in pixel coordinates:
  void getPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  // plot style specific functions to generate plot data, used by getPlotData:
  void getScatterPlotData(QVector<QCPData> *pointData) const;
  void getLinePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getStepLeftPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getStepRightPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getStepCenterPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getImpulsePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  
  // helper functions for drawing:
  virtual void drawFill(QCPPainter *painter, QVector<QPointF> *lineData) const;
  virtual void drawScatterPlot(QCPPainter *painter, QVector<QCPData> *pointData) const;
  virtual void drawLinePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;
  virtual void drawImpulsePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;
  void drawError(QCPPainter *painter, double x, double y, const QCPData &data) const;
  
  // helper functions:
  void getVisibleDataBounds(QCPDataMap::const_iterator &lower, QCPDataMap::const_iterator &upper, int &count) const;
  void addFillBasePoints(QVector<QPointF> *lineData) const;
  void removeFillBasePoints(QVector<QPointF> *lineData) const;
  QPointF lowerFillBasePoint(double lowerKey) const;
  QPointF upperFillBasePoint(double upperKey) const;
  const QPolygonF getChannelFillPolygon(const QVector<QPointF> *lineData) const;
  int findIndexBelowX(const QVector<QPointF> *data, double x) const;
  int findIndexAboveX(const QVector<QPointF> *data, double x) const;
  int findIndexBelowY(const QVector<QPointF> *data, double y) const;
  int findIndexAboveY(const QVector<QPointF> *data, double y) const;
  double pointDistance(const QPointF &pixelPoint) const;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface
  virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface
  
  friend class QCustomPlot;
  friend class QCPLegend;
};

#endif // QCP_PLOTTABLE_GRAPH_H

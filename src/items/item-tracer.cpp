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

#include "item-tracer.h"

#include "../painter.h"
#include "../core.h"
#include "../plottables/plottable-graph.h"

// ================================================================================
// =================== QCPItemTracer
// ================================================================================

/*! \class QCPItemTracer
  \brief Item that sticks to QCPGraph data points

  \image html QCPItemTracer.png "Tracer example. Blue dotted circles are anchors, solid blue discs are positions."

  The tracer can be connected with a QCPGraph via \ref setGraph. Then it will automatically adopt
  the coordinate axes of the graph and update its \a position to be on the graph's data. This means
  the key stays controllable via \ref setGraphKey, but the value will follow the graph data. If a
  QCPGraph is connected, note that setting the coordinates directly via \a position will have no
  effect, i.e. be overriden in the next redraw (this is when the coodinate update happens).
  
  If the specified key in \ref setGraphKey is outside the key bounds of the graph, the tracer will
  stay at the respective end of the graph.
  
  With \ref setInterpolating you may specify whether the tracer may only stay exactly on data
  points or whether it interpolates data points linearly, if given a key that lies between two data
  points of the graph.
  
  The tracer has different visual styles, see \ref setStyle. It is also possible to make the tracer
  have no own visual appearance (set the style to \ref tsNone), and just connect other item
  positions to the tracer \a position (used as an anchor) via \ref
  QCPItemPosition::setParentAnchor.
  
  \note The tracer position is only automatically updated upon redraws. This means when, for
  example, the data of the graph changes and you immediately afterwards (without a redraw) read the
  \a position coordinates of the tracer, they will not reflect the updated data of the graph. In
  this case you should call \ref updatePosition manually, prior to reading the tracer coordinates.
*/

/*!
  Creates a tracer item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemTracer::QCPItemTracer(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  position(createPosition("position")),
  mGraph(0)
{
  position->setCoords(0, 0);

  setBrush(Qt::NoBrush);
  setSelectedBrush(Qt::NoBrush);
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue, 2));
  setStyle(tsCrosshair);
  setSize(6);
  setInterpolating(false);
  setGraphKey(0);
}

QCPItemTracer::~QCPItemTracer()
{
}

/*!
  Sets the pen that will be used to draw the line of the tracer
  
  \see setSelectedPen, setBrush
*/
void QCPItemTracer::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw the line of the tracer when selected
  
  \see setPen, setSelected
*/
void QCPItemTracer::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  Sets the brush that will be used to draw any fills of the tracer
  
  \see setSelectedBrush, setPen
*/
void QCPItemTracer::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  Sets the brush that will be used to draw any fills of the tracer, when selected.
  
  \see setBrush, setSelected
*/
void QCPItemTracer::setSelectedBrush(const QBrush &brush)
{
  mSelectedBrush = brush;
}

/*!
  Sets the size of the tracer in pixels, if the style supports setting a size (e.g. \ref tsSquare
  does, \ref tsCrosshair does not).
*/
void QCPItemTracer::setSize(double size)
{
  mSize = size;
}

/*!
  Sets the style/visual appearance of the tracer.
  
  If you only want to use the tracer \a position as an anchor for other items, set \a style to
  \ref tsNone.
*/
void QCPItemTracer::setStyle(QCPItemTracer::TracerStyle style)
{
  mStyle = style;
}

/*!
  Sets the QCPGraph this tracer sticks to. The tracer \a position will be set to type
  QCPItemPosition::ptPlotCoords and the axes will be set to the axes of \a graph.
  
  To free the tracer from any graph, set \a graph to 0. The tracer \a position can then be placed
  freely like any other item position. This is the state the tracer will assume when its graph gets
  deleted while still attached to it.
  
  \see setGraphKey
*/
void QCPItemTracer::setGraph(QCPGraph *graph)
{
  if (graph)
  {
    if (graph->parentPlot() == mParentPlot)
    {
      position->setType(QCPItemPosition::ptPlotCoords);
      position->setAxes(graph->keyAxis(), graph->valueAxis());
      mGraph = graph;
      updatePosition();
    } else
      qDebug() << Q_FUNC_INFO << "graph isn't in same QCustomPlot instance as this item";
  } else
  {
    mGraph = 0;
  }
}

/*!
  Sets the key of the graph's data point the tracer will be positioned at. This is the only free
  coordinate of a tracer when attached to a graph.
  
  Depending on \ref setInterpolating, the tracer will be either positioned on the data point
  closest to \a key, or will stay exactly at \a key and interpolate the value linearly.
  
  \see setGraph, setInterpolating
*/
void QCPItemTracer::setGraphKey(double key)
{
  mGraphKey = key;
}

/*!
  Sets whether the value of the graph's data points shall be interpolated, when positioning the
  tracer.
  
  If \a enabled is set to false and a key is given with \ref setGraphKey, the tracer is placed on
  the data point of the graph which is closest to the key, but which is not necessarily exactly
  there. If \a enabled is true, the tracer will be positioned exactly at the specified key, and
  the appropriate value will be interpolated from the graph's data points linearly.
  
  \see setGraph, setGraphKey
*/
void QCPItemTracer::setInterpolating(bool enabled)
{
  mInterpolating = enabled;
}

/* inherits documentation from base class */
double QCPItemTracer::selectTest(const QPointF &pos) const
{
  if (!mVisible || mStyle == tsNone)
    return -1;

  QPointF center(position->pixelPoint());
  double w = mSize/2.0;
  QRect clip = clipRect();
  switch (mStyle)
  {
    case tsNone: return -1;
    case tsPlus:
    {
      if (clipRect().intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
        return qSqrt(qMin(distSqrToLine(center+QPointF(-w, 0), center+QPointF(w, 0), pos),
                          distSqrToLine(center+QPointF(0, -w), center+QPointF(0, w), pos)));
      break;
    }
    case tsCrosshair:
    {
      return qSqrt(qMin(distSqrToLine(QPointF(clip.left(), center.y()), QPointF(clip.right(), center.y()), pos),
                        distSqrToLine(QPointF(center.x(), clip.top()), QPointF(center.x(), clip.bottom()), pos)));
      break;
    }
    case tsCircle:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
      {
        // distance to border:
        double centerDist = QVector2D(center-pos).length();
        double circleLine = w;
        double result = qAbs(centerDist-circleLine);
        // filled ellipse, allow click inside to count as hit:
        if (result > mParentPlot->selectionTolerance()*0.99 && mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0)
        {
          if (centerDist <= circleLine)
            result = mParentPlot->selectionTolerance()*0.99;
        }
        return result;
      }
      break;
    }
    case tsSquare:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
      {
        QRectF rect = QRectF(center-QPointF(w, w), center+QPointF(w, w));
        bool filledRect = mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0;
        return rectSelectTest(rect, pos, filledRect);
      }
      break;
    }
  }
  return -1;
}

/* inherits documentation from base class */
void QCPItemTracer::draw(QCPPainter *painter)
{
  updatePosition();
  if (mStyle == tsNone)
    return;

  painter->setPen(mainPen());
  painter->setBrush(mainBrush());
  QPointF center(position->pixelPoint());
  double w = mSize/2.0;
  QRect clip = clipRect();
  switch (mStyle)
  {
    case tsNone: return;
    case tsPlus:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
      {
        painter->drawLine(QLineF(center+QPointF(-w, 0), center+QPointF(w, 0)));
        painter->drawLine(QLineF(center+QPointF(0, -w), center+QPointF(0, w)));
      }
      break;
    }
    case tsCrosshair:
    {
      if (center.y() > clip.top() && center.y() < clip.bottom())
        painter->drawLine(QLineF(clip.left(), center.y(), clip.right(), center.y()));
      if (center.x() > clip.left() && center.x() < clip.right())
        painter->drawLine(QLineF(center.x(), clip.top(), center.x(), clip.bottom()));
      break;
    }
    case tsCircle:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
        painter->drawEllipse(center, w, w);
      break;
    }
    case tsSquare:
    {
      if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
        painter->drawRect(QRectF(center-QPointF(w, w), center+QPointF(w, w)));
      break;
    }
  }
}

/*!
  If the tracer is connected with a graph (\ref setGraph), this function updates the tracer's \a
  position to reside on the graph data, depending on the configured key (\ref setGraphKey).
  
  It is called automatically on every redraw and normally doesn't need to be called manually. One
  exception is when you want to read the tracer coordinates via \a position and are not sure that
  the graph's data (or the tracer key with \ref setGraphKey) hasn't changed since the last redraw.
  In that situation, call this function before accessing \a position, to make sure you don't get
  out-of-date coordinates.
  
  If there is no graph set on this tracer, this function does nothing.
*/
void QCPItemTracer::updatePosition()
{
  if (mGraph)
  {
    if (mParentPlot->hasPlottable(mGraph))
    {
      if (mGraph->data()->size() > 1)
      {
        QCPDataMap::const_iterator first = mGraph->data()->constBegin();
        QCPDataMap::const_iterator last = mGraph->data()->constEnd()-1;
        if (mGraphKey < first.key())
          position->setCoords(first.key(), first.value().value);
        else if (mGraphKey > last.key())
          position->setCoords(last.key(), last.value().value);
        else
        {
          QCPDataMap::const_iterator it = first;
          it = mGraph->data()->lowerBound(mGraphKey);
          if (it != first) // mGraphKey is somewhere between iterators
          {
            QCPDataMap::const_iterator prevIt = it-1;
            if (mInterpolating)
            {
              // interpolate between iterators around mGraphKey:
              double slope = (it.value().value-prevIt.value().value)/(it.key()-prevIt.key());
              position->setCoords(mGraphKey, (mGraphKey-prevIt.key())*slope+prevIt.value().value);
            } else
            {
              // find iterator with key closest to mGraphKey:
              if (mGraphKey < (prevIt.key()+it.key())*0.5)
                it = prevIt;
              position->setCoords(it.key(), it.value().value);
            }
          } else // mGraphKey is exactly on first iterator
            position->setCoords(it.key(), it.value().value);
        }
      } else if (mGraph->data()->size() == 1)
      {
        QCPDataMap::const_iterator it = mGraph->data()->constBegin();
        position->setCoords(it.key(), it.value().value);
      } else
        qDebug() << Q_FUNC_INFO << "graph has no data";
    } else
      qDebug() << Q_FUNC_INFO << "graph not contained in QCustomPlot instance (anymore)";
  }
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the item is not selected
  and mSelectedPen when it is.
*/
QPen QCPItemTracer::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}

/*! \internal

  Returns the brush that should be used for drawing fills of the item. Returns mBrush when the item
  is not selected and mSelectedBrush when it is.
*/
QBrush QCPItemTracer::mainBrush() const
{
  return mSelected ? mSelectedBrush : mBrush;
}

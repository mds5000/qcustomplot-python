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

#include "axis.h"

#include "painter.h"
#include "core.h"

// ================================================================================
// =================== QCPGrid
// ================================================================================

/*! \class QCPGrid
  \brief Responsible for drawing the grid of a QCPAxis.
  
  This class is tightly bound to QCPAxis. Every axis owns a grid instance internally and uses it to
  draw the grid. Normally, you don't need to interact with the QCPGrid instance, because QCPAxis
  reproduces the grid interface in its own interface.
  
  The axis and grid drawing was split into two classes to allow them to be placed on different
  layers (both QCPAxis and QCPGrid inherit from QCPLayerable). So it is possible to have the grid
  at the background and the axes in the foreground, and any plottables/items in between. This
  described situation is the default setup, see QCPLayer documentation.  
*/

/*!
  Creates a QCPGrid instance and sets default values.
  
  You shouldn't instantiate grids on their own, since every QCPAxis brings its own QCPGrid
  internally
*/
QCPGrid::QCPGrid(QCPAxis *parentAxis) :
  QCPLayerable(parentAxis->parentPlot()),
  mParentAxis(parentAxis),
  mSectionBrushEven(Qt::NoBrush),
  mSectionBrushOdd(Qt::NoBrush)
{
  setPen(QPen(QColor(200,200,200), 0, Qt::DotLine));
  setSubGridPen(QPen(QColor(220,220,220), 0, Qt::DotLine));
  setZeroLinePen(QPen(QColor(200,200,200), 0, Qt::SolidLine));
  setSubGridVisible(false);
  setAntialiased(false);
  setAntialiasedSubGrid(false);
  setAntialiasedZeroLine(false);
  
  //DBG:
  //setSectionBrushes(QBrush(Qt::lightGray), QBrush(Qt::gray));
}

QCPGrid::~QCPGrid()
{
}

/*!
  Sets whether grid lines at sub tick marks are drawn.
  
  \see setSubGridPen
*/
void QCPGrid::setSubGridVisible(bool visible)
{
  mSubGridVisible = visible;
}

/*!
  Sets whether sub grid lines are drawn antialiased.
*/
void QCPGrid::setAntialiasedSubGrid(bool enabled)
{
  mAntialiasedSubGrid = enabled;
}

/*!
  Sets whether zero lines are drawn antialiased.
*/
void QCPGrid::setAntialiasedZeroLine(bool enabled)
{
  mAntialiasedZeroLine = enabled;
}

/*!
  Sets the pen with which (major) grid lines are drawn.
*/
void QCPGrid::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen with which sub grid lines are drawn.
*/
void QCPGrid::setSubGridPen(const QPen &pen)
{
  mSubGridPen = pen;
}

/*!
  Sets the pen with which zero lines are drawn.
  
  Zero lines are lines at coordinate 0 which may be drawn with a different pen than other grid
  lines. To disable zero lines and just draw normal grid lines at zero, set \a pen to Qt::NoPen.
*/
void QCPGrid::setZeroLinePen(const QPen &pen)
{
  mZeroLinePen = pen;
}

/*!
  Sets the brushes that will be used to draw tick section backgrounds of the axes alternatingly. To
  disable alternating background brushes for axis tick sections, set the brushes to Qt::NoBrush
*/
void QCPGrid::setSectionBrushes(const QBrush &brushEven, const QBrush &brushOdd)
{
  mSectionBrushEven = brushEven;
  mSectionBrushOdd = brushOdd;
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing the major grid lines.

  This is the antialiasing state the painter passed to the \ref draw method is in by default.
  
  This function takes into account the local setting of the antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased
*/
void QCPGrid::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aeGrid);
}

/*! \internal
  
  Draws grid lines and sub grid lines at the positions of (sub) ticks of the parent axis, spanning
  over the complete axis rect. Also draws the zero line, if appropriate (\ref setZeroLinePen).
  
  Called by QCustomPlot::draw to draw the grid of an axis.
*/
void QCPGrid::draw(QCPPainter *painter)
{
  if (!mParentAxis->visible()) return; // also don't draw grid when parent axis isn't visible
  
  if (mSectionBrushEven != Qt::NoBrush || mSectionBrushOdd != Qt::NoBrush)
    drawSections(painter);
  if (mSubGridVisible)
    drawSubGridLines(painter);
  drawGridLines(painter);
}

/*! \internal
  
  Draws tick sections of the axis with alternating brushes set via \ref setSectionBrushes.
  
  Called by QCustomPlot::draw to draw the sections of an axis.
*/
void QCPGrid::drawSections(QCPPainter *painter) const
{
  int lowTick = mParentAxis->mLowestVisibleTick;
  int highTick = mParentAxis->mHighestVisibleTick;
  double t1, t2; // helper variable, result of coordinate-to-pixel transforms
  if (mParentAxis->orientation() == Qt::Horizontal)
  {
    // TODO: make this properly work. Doesn't work when only one tick in range and probably when logarithmic scale.
    // idea for no tick in visible range: if (lowtick > hightick), draw full axis fillrect with color depending on
    // side (left or right of ticks) and whether there are ticks to left/right in tickvector.
    applyDefaultAntialiasingHint(painter);
    painter->setPen(Qt::NoPen);
    for (int i=lowTick; i <= highTick-1; ++i)
    {
      int sectionId = 0;
      if (mParentAxis->autoTicks())
      {
        double sectionHalf = (mParentAxis->mTickVector.at(i)+mParentAxis->mTickVector.at(i+1))/2.0;
        sectionId = qFloor(sectionHalf/mParentAxis->mTickStep);
      }
      {
        sectionId = i;
      }
      t1 = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i)); // x1
      t2 = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i+1)); // x2
      if (i == lowTick) // draw first section extra
      {
        painter->setBrush((sectionId-1) % 2 == 0 ? mSectionBrushEven : mSectionBrushOdd);
        painter->drawRect(QRectF(mParentAxis->mAxisRect.left(), mParentAxis->mAxisRect.top(), t1-mParentAxis->mAxisRect.left(), mParentAxis->mAxisRect.height()));
      }
      painter->setBrush(sectionId % 2 == 0 ? mSectionBrushEven : mSectionBrushOdd);
      painter->drawRect(QRectF(t1, mParentAxis->mAxisRect.top(), t2-t1, mParentAxis->mAxisRect.height()));
      if (i == highTick-1) // draw last section extra
      {
        painter->setBrush((sectionId+1) % 2 == 0 ? mSectionBrushEven : mSectionBrushOdd);
        painter->drawRect(QRectF(t2, mParentAxis->mAxisRect.top(), mParentAxis->mAxisRect.right()-t2+1, mParentAxis->mAxisRect.height()));
      }
    }
  } else
  {
    // TODO: same for Qt::Vertical
  }
}

/*! \internal
  
  Draws the main grid lines and possibly a zero line with the specified painter.
  
  This is a helper function called by \ref draw.
*/
void QCPGrid::drawGridLines(QCPPainter *painter) const
{
  int lowTick = mParentAxis->mLowestVisibleTick;
  int highTick = mParentAxis->mHighestVisibleTick;
  double t; // helper variable, result of coordinate-to-pixel transforms
  if (mParentAxis->orientation() == Qt::Horizontal)
  {
    // draw zeroline:
    int zeroLineIndex = -1;
    if (mZeroLinePen.style() != Qt::NoPen && mParentAxis->mRange.lower < 0 && mParentAxis->mRange.upper > 0)
    {
      applyAntialiasingHint(painter, mAntialiasedZeroLine, QCP::aeZeroLine);
      painter->setPen(mZeroLinePen);
      double epsilon = mParentAxis->range().size()*1E-6; // for comparing double to zero
      for (int i=lowTick; i <= highTick; ++i)
      {
        if (qAbs(mParentAxis->mTickVector.at(i)) < epsilon)
        {
          zeroLineIndex = i;
          t = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i)); // x
          painter->drawLine(QLineF(t, mParentAxis->mAxisRect.bottom(), t, mParentAxis->mAxisRect.top()));
          break;
        }
      }
    }
    // draw grid lines:
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mPen);
    for (int i=lowTick; i <= highTick; ++i)
    {
      if (i == zeroLineIndex) continue; // don't draw a gridline on top of the zeroline
      t = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i)); // x
      painter->drawLine(QLineF(t, mParentAxis->mAxisRect.bottom(), t, mParentAxis->mAxisRect.top()));
    }
  } else
  {
    // draw zeroline:
    int zeroLineIndex = -1;
    if (mZeroLinePen.style() != Qt::NoPen && mParentAxis->mRange.lower < 0 && mParentAxis->mRange.upper > 0)
    {
      applyAntialiasingHint(painter, mAntialiasedZeroLine, QCP::aeZeroLine);
      painter->setPen(mZeroLinePen);
      double epsilon = mParentAxis->mRange.size()*1E-6; // for comparing double to zero
      for (int i=lowTick; i <= highTick; ++i)
      {
        if (qAbs(mParentAxis->mTickVector.at(i)) < epsilon)
        {
          zeroLineIndex = i;
          t = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i)); // y
          painter->drawLine(QLineF(mParentAxis->mAxisRect.left(), t, mParentAxis->mAxisRect.right(), t));
          break;
        }
      }
    }
    // draw grid lines:
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mPen);
    for (int i=lowTick; i <= highTick; ++i)
    {
      if (i == zeroLineIndex) continue; // don't draw a gridline on top of the zeroline
      t = mParentAxis->coordToPixel(mParentAxis->mTickVector.at(i)); // y
      painter->drawLine(QLineF(mParentAxis->mAxisRect.left(), t, mParentAxis->mAxisRect.right(), t));
    }
  }
}

/*! \internal
  
  Draws the sub grid lines with the specified painter.
  
  This is a helper function called by \ref draw.
*/
void QCPGrid::drawSubGridLines(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiasedSubGrid, QCP::aeSubGrid);
  double t; // helper variable, result of coordinate-to-pixel transforms
  painter->setPen(mSubGridPen);
  if (mParentAxis->orientation() == Qt::Horizontal)
  {
    for (int i=0; i<mParentAxis->mSubTickVector.size(); ++i)
    {
      t = mParentAxis->coordToPixel(mParentAxis->mSubTickVector.at(i)); // x
      painter->drawLine(QLineF(t, mParentAxis->mAxisRect.bottom(), t, mParentAxis->mAxisRect.top()));
    }
  } else
  {
    for (int i=0; i<mParentAxis->mSubTickVector.size(); ++i)
    {
      t = mParentAxis->coordToPixel(mParentAxis->mSubTickVector.at(i)); // y
      painter->drawLine(QLineF(mParentAxis->mAxisRect.left(), t, mParentAxis->mAxisRect.right(), t));
    }
  }
}


// ================================================================================
// =================== QCPAxis
// ================================================================================

/*! \class QCPAxis
  \brief Manages a single axis inside a QCustomPlot.

  Usually doesn't need to be instantiated externally. Access %QCustomPlot's axes via
  QCustomPlot::xAxis (bottom), QCustomPlot::yAxis (left), QCustomPlot::xAxis2 (top) and
  QCustomPlot::yAxis2 (right).
*/

/* start of documentation of inline functions */

/*! \fn Qt::Orientation QCPAxis::orientation() const
  
  Returns the orientation of the axis. The axis orientation (horizontal or vertical) is deduced
  from the axis type (left, top, right or bottom).
*/

/* end of documentation of inline functions */
/* start of documentation of signals */

/*! \fn void QCPAxis::ticksRequest()
  
  This signal is emitted when \ref setAutoTicks is false and the axis is about to generate tick
  labels and replot itself.
  
  Modifying the tick positions can be done with \ref setTickVector. If you also want to control the
  tick labels, set \ref setAutoTickLabels to false and also provide the labels with \ref
  setTickVectorLabels.
  
  If you only want static ticks you probably don't need this signal, since you can just set the
  tick vector (and possibly tick label vector) once. However, if you want to provide ticks (and
  maybe labels) dynamically, e.g. depending on the current axis range, connect a slot to this
  signal and set the vector/vectors there.
*/

/*! \fn void QCPAxis::rangeChanged(const QCPRange &newRange)

  This signal is emitted when the range of this axis has changed. You can connect it to the \ref
  setRange slot of another axis to communicate the new range to the other axis, in order for it to
  be synchronized.
*/

/*! \fn void QCPAxis::selectionChanged(QCPAxis::SelectableParts selection)
  
  This signal is emitted when the selection state of this axis has changed, either by user interaction
  or by a direct call to \ref setSelected.
*/

/* end of documentation of signals */

/*!
  Constructs an Axis instance of Type \a type inside \a parentPlot.
*/
QCPAxis::QCPAxis(QCustomPlot *parentPlot, AxisType type) :
  QCPLayerable(parentPlot)
{
  mLabelCache.setMaxCost(16); // cache at most 16 (tick) labels
  mLowestVisibleTick = 0;
  mHighestVisibleTick = -1;
  mGrid = new QCPGrid(this);
  setAxisType(type);
  setAxisRect(parentPlot->axisRect()); 
  setScaleType(stLinear);
  setScaleLogBase(10);
  
  setAntialiased(false);
  setRange(0, 5);
  setRangeReversed(false);
  
  setTicks(true);
  setTickStep(1);
  setAutoTickCount(6);
  setAutoTicks(true);
  setAutoTickLabels(true);
  setAutoTickStep(true);
  setTickLabelFont(parentPlot->font());
  setTickLabelColor(Qt::black);
  setTickLength(5);
  setTickPen(QPen(Qt::black, 0, Qt::SolidLine, Qt::SquareCap));
  setTickLabels(true);
  setTickLabelType(ltNumber);
  setTickLabelRotation(0);
  setDateTimeFormat("hh:mm:ss\ndd.MM.yy");
  setNumberFormat("gbd");
  setNumberPrecision(6);
  setLabel("");
  setLabelFont(parentPlot->font());
  setLabelColor(Qt::black);
  
  setAutoSubTicks(true);
  setSubTickCount(4);
  setSubTickLength(2);
  setSubTickPen(QPen(Qt::black, 0, Qt::SolidLine, Qt::SquareCap));
  setBasePen(QPen(Qt::black, 0, Qt::SolidLine, Qt::SquareCap));
  
  setSelected(spNone);
  setSelectable(spAxis | spTickLabels | spAxisLabel);
  QFont selTickLabelFont = tickLabelFont();
  selTickLabelFont.setBold(true);
  setSelectedTickLabelFont(selTickLabelFont);
  QFont selLabelFont = labelFont();
  selLabelFont.setBold(true);
  setSelectedLabelFont(selLabelFont);
  setSelectedTickLabelColor(Qt::blue);
  setSelectedLabelColor(Qt::blue);
  QPen blueThickPen(Qt::blue, 2);
  setSelectedBasePen(blueThickPen);
  setSelectedTickPen(blueThickPen);
  setSelectedSubTickPen(blueThickPen);
  
  setPadding(0);
  if (type == atTop)
  {
    setTickLabelPadding(3);
    setLabelPadding(6);
  } else if (type == atRight)
  {
    setTickLabelPadding(7);
    setLabelPadding(12);
  } else if (type == atBottom)
  {
    setTickLabelPadding(3);
    setLabelPadding(3);
  } else if (type == atLeft)
  {
    setTickLabelPadding(5);
    setLabelPadding(10);
  }
}

QCPAxis::~QCPAxis()
{
  delete mGrid;
}

/* No documentation as it is a property getter */
QString QCPAxis::numberFormat() const
{
  QString result;
  result.append(mNumberFormatChar);
  if (mNumberBeautifulPowers)
  {
    result.append("b");
    if (mNumberMultiplyCross)
      result.append("c");
  }
  return result;
}

/*! \internal
  
  Sets the axis type. This determines the \ref orientation and together with the current axis rect
  (see \ref setAxisRect), the position of the axis. Depending on \a type, ticks, tick labels, and
  label are drawn on corresponding sides of the axis base line.
*/
void QCPAxis::setAxisType(AxisType type)
{
  mAxisType = type;
  mOrientation = (type == atBottom || type == atTop) ? Qt::Horizontal : Qt::Vertical;
}

/*! \internal
  
  Sets the axis rect. The axis uses this rect to position itself within the plot,
  together with the information of its type (\ref setAxisType). Theoretically it's possible to give
  a plot's axes different axis rects (e.g. for gaps between them), however, they are currently all
  synchronized by the QCustomPlot::setAxisRect function.
*/
void QCPAxis::setAxisRect(const QRect &rect)
{
  mAxisRect = rect;
}

/*!
  Sets whether the axis uses a linear scale or a logarithmic scale. If \a type is set to \ref
  stLogarithmic, the logarithm base can be set with \ref setScaleLogBase. In logarithmic axis
  scaling, major tick marks appear at all powers of the logarithm base. Properties like tick step
  (\ref setTickStep) don't apply in logarithmic scaling. If you wish a decimal base but less major
  ticks, consider choosing a logarithm base of 100, 1000 or even higher.
  
  If \a type is \ref stLogarithmic and the number format (\ref setNumberFormat) uses the 'b' option
  (beautifully typeset decimal powers), the display usually is "1 [multiplication sign] 10
  [superscript] n", which looks unnatural for logarithmic scaling (the "1 [multiplication sign]"
  part). To only display the decimal power, set the number precision to zero with
  \ref setNumberPrecision.
*/
void QCPAxis::setScaleType(ScaleType type)
{
  mScaleType = type;
  if (mScaleType == stLogarithmic)
    mRange = mRange.sanitizedForLogScale();
}

/*!
  If \ref setScaleType is set to \ref stLogarithmic, \a base will be the logarithm base of the
  scaling. In logarithmic axis scaling, major tick marks appear at all powers of \a base.
  
  Properties like tick step (\ref setTickStep) don't apply in logarithmic scaling. If you wish a decimal base but
  less major ticks, consider choosing \a base 100, 1000 or even higher.
*/
void QCPAxis::setScaleLogBase(double base)
{
  if (base > 1)
  {
    mScaleLogBase = base;
    mScaleLogBaseLogInv = 1.0/qLn(mScaleLogBase); // buffer for faster baseLog() calculation
  } else
    qDebug() << Q_FUNC_INFO << "Invalid logarithmic scale base (must be greater 1):" << base;
}

/*!
  Sets the range of the axis.
  
  This slot may be connected with the \ref rangeChanged signal of another axis so this axis
  is always synchronized with the other axis range, when it changes.
  
  To invert the direction of an axis range, use \ref setRangeReversed.
*/
void QCPAxis::setRange(const QCPRange &range)
{
  if (range.lower == mRange.lower && range.upper == mRange.upper)
    return;
  
  if (!QCPRange::validRange(range)) return;
  if (mScaleType == stLogarithmic)
  {
    mRange = range.sanitizedForLogScale();
  } else
  {
    mRange = range.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  Sets whether the user can (de-)select the parts in \a selectable by clicking on the QCustomPlot surface.
  (When \ref QCustomPlot::setInteractions contains iSelectAxes.)
  
  However, even when \a selectable is set to a value not allowing the selection of a specific part,
  it is still possible to set the selection of this part manually, by calling \ref setSelected
  directly.
  
  \see SelectablePart, setSelected
*/
void QCPAxis::setSelectable(const SelectableParts &selectable)
{
  mSelectable = selectable;
}

/*!
  Sets the selected state of the respective axis parts described by \ref SelectablePart. When a part
  is selected, it uses a different pen/font.
  
  The entire selection mechanism for axes is handled automatically when \ref
  QCustomPlot::setInteractions contains iSelectAxes. You only need to call this function when you
  wish to change the selection state manually.
  
  This function can change the selection state of a part even when \ref setSelectable was set to a
  value that actually excludes the part.
  
  emits the \ref selectionChanged signal when \a selected is different from the previous selection state.
  
  \see SelectablePart, setSelectable, selectTest, setSelectedBasePen, setSelectedTickPen, setSelectedSubTickPen,
  setSelectedTickLabelFont, setSelectedLabelFont, setSelectedTickLabelColor, setSelectedLabelColor
*/
void QCPAxis::setSelected(const SelectableParts &selected)
{
  if (mSelected != selected)
  {
    if (mSelected.testFlag(spTickLabels) != selected.testFlag(spTickLabels))
      mLabelCache.clear();
    mSelected = selected;
    emit selectionChanged(mSelected);
  }
}

/*!
  \overload
  Sets the lower and upper bound of the axis range.
  
  To invert the direction of an axis range, use \ref setRangeReversed.
  
  There is also a slot to set a range, see \ref setRange(const QCPRange &range).
*/
void QCPAxis::setRange(double lower, double upper)
{
  if (lower == mRange.lower && upper == mRange.upper)
    return;
  
  if (!QCPRange::validRange(lower, upper)) return;
  mRange.lower = lower;
  mRange.upper = upper;
  if (mScaleType == stLogarithmic)
  {
    mRange = mRange.sanitizedForLogScale();
  } else
  {
    mRange = mRange.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  \overload
  Sets the range of the axis.

  \param position the \a position coordinate indicates together with the \a alignment parameter, where
  the new range will be positioned.
  \param size defines the size (upper-lower) of the new axis range.
  \param alignment determines how \a position is to be interpreted.\n
  If \a alignment is Qt::AlignLeft, \a position will be the lower bound of the range.\n
  If \a alignment is Qt::AlignRight, \a position will be the upper bound of the range.\n
  If \a alignment is Qt::AlignCenter, the new range will be centered around \a position.\n
  Any other values for \a alignment will default to Qt::AlignCenter.
*/
void QCPAxis::setRange(double position, double size, Qt::AlignmentFlag alignment)
{
  if (alignment == Qt::AlignLeft)
    setRange(position, position+size);
  else if (alignment == Qt::AlignRight)
    setRange(position-size, position);
  else // alignment == Qt::AlignCenter
    setRange(position-size/2.0, position+size/2.0);
}

/*!
  Sets the lower bound of the axis range, independently of the upper bound.
  \see setRange
*/
void QCPAxis::setRangeLower(double lower)
{
  if (mRange.lower == lower)
    return;
  
  mRange.lower = lower;
  if (mScaleType == stLogarithmic)
  {
    mRange = mRange.sanitizedForLogScale();
  } else
  {
    mRange = mRange.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  Sets the upper bound of the axis range, independently of the lower bound.
  \see setRange
*/
void QCPAxis::setRangeUpper(double upper)
{
  if (mRange.upper == upper)
    return;
  
  mRange.upper = upper;
  if (mScaleType == stLogarithmic)
  {
    mRange = mRange.sanitizedForLogScale();
  } else
  {
    mRange = mRange.sanitizedForLinScale();
  }
  emit rangeChanged(mRange);
}

/*!
  Sets whether the axis range (direction) is displayed reversed. Normally, the values on horizontal
  axes increase left to right, on vertical axes bottom to top. When \a reversed is set to true, the
  direction of increasing values is inverted. Note that the range and data interface stays the same
  for reversed axes, e.g. the \a lower part of the \ref setRange interface will still reference the
  mathematically smaller number than the \a upper part.
*/
void QCPAxis::setRangeReversed(bool reversed)
{
  mRangeReversed = reversed;
}

/*!
  Sets whether the grid of this axis is drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAxis::setAntialiasedGrid(bool enabled)
{
  mGrid->setAntialiased(enabled);
}

/*!
  Sets whether the sub grid of this axis is drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAxis::setAntialiasedSubGrid(bool enabled)
{
  mGrid->setAntialiasedSubGrid(enabled);
}

/*!
  Sets whether the zero line of this axis is drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAxis::setAntialiasedZeroLine(bool enabled)
{
  mGrid->setAntialiasedZeroLine(enabled);
}

/*!
  Sets whether the grid lines are visible.
  \see setSubGrid, setGridPen, setZeroLinePen
*/
void QCPAxis::setGrid(bool show)
{
  mGrid->setVisible(show);
}

/*!
  Sets whether the sub grid lines are visible.
  \see setGrid, setSubGridPen, setZeroLinePen
*/
void QCPAxis::setSubGrid(bool show)
{
  mGrid->setSubGridVisible(show);
}

/*!
  Sets whether the tick positions should be calculated automatically (either from an automatically
  generated tick step or a tick step provided manually via \ref setTickStep, see \ref setAutoTickStep).
  
  If \a on is set to false, you must provide the tick positions manually via \ref setTickVector.
  For these manual ticks you may let QCPAxis generate the appropriate labels automatically
  by setting/leaving \ref setAutoTickLabels true. If you also wish to control the displayed labels
  manually, set \ref setAutoTickLabels to false and provide the label strings with \ref setTickVectorLabels.
  
  If you need dynamically calculated tick vectors (and possibly tick label vectors), set the
  vectors in a slot connected to the \ref ticksRequest signal.
*/
void QCPAxis::setAutoTicks(bool on)
{
  mAutoTicks = on;
}

/*!
  When \ref setAutoTickStep is true, \a approximateCount determines how many ticks should be generated
  in the visible range approximately.
  
  Only values greater than zero are accepted as \a approximateCount.
*/
void QCPAxis::setAutoTickCount(int approximateCount)
{
  if (approximateCount > 0)
    mAutoTickCount = approximateCount;
  else
    qDebug() << Q_FUNC_INFO << "approximateCount must be greater than zero:" << approximateCount;
}

/*!
  Sets whether the tick labels are generated automatically depending on the tick label type
  (\ref ltNumber or \ref ltDateTime).
  
  If \a on is set to false, you should provide the tick labels via \ref setTickVectorLabels. This
  is usually used in a combination with \ref setAutoTicks set to false for complete control over
  tick positions and labels, e.g. when the ticks should be at multiples of pi and show "2pi", "3pi"
  etc. as tick labels.
  
  If you need dynamically calculated tick vectors (and possibly tick label vectors), set the
  vectors in a slot connected to the \ref ticksRequest signal.
*/
void QCPAxis::setAutoTickLabels(bool on)
{
  mAutoTickLabels = on;
}

/*!
  Sets whether the tick step, i.e. the interval between two (major) ticks, is calculated
  automatically. If \a on is set to true, the axis finds a tick step that is reasonable for human
  readable plots. This means the tick step mantissa is chosen such that it's either a multiple of
  two or ends in 0.5. The number of ticks the algorithm aims for within the visible range can be
  set with \ref setAutoTickCount. It's not guaranteed that this number of ticks is met exactly, but
  approximately within a tolerance of two or three.
  
  If \a on is set to false, you may set the tick step manually with \ref setTickStep.
*/
void QCPAxis::setAutoTickStep(bool on)
{
  mAutoTickStep = on;
}

/*!
  Sets whether the number of sub ticks in one tick interval is determined automatically.
  This works, as long as the tick step mantissa is a multiple of 0.5 (which it is, when
  \ref setAutoTickStep is enabled).\n
  When \a on is set to false, you may set the sub tick count with \ref setSubTickCount manually.
*/
void QCPAxis::setAutoSubTicks(bool on)
{
  mAutoSubTicks = on;
}

/*!
  Sets whether tick marks are displayed. Setting \a show to false does not imply, that tick labels
  are invisible, too. To achieve that, see \ref setTickLabels.
*/
void QCPAxis::setTicks(bool show)
{
  mTicks = show;
}

/*!
  Sets whether tick labels are displayed.
*/
void QCPAxis::setTickLabels(bool show)
{
  mTickLabels = show;
}

/*!
  Sets the distance between the axis base line (or any tick marks pointing outward) and the tick labels.
  \see setLabelPadding, setPadding
*/
void QCPAxis::setTickLabelPadding(int padding)
{
  mTickLabelPadding = padding;
}

/*!
  Sets whether the tick labels display numbers or dates/times.
  
  If \a type is set to \ref ltNumber, the format specifications of \ref setNumberFormat apply.
  
  If \a type is set to \ref ltDateTime, the format specifications of \ref setDateTimeFormat apply.
  
  In QCustomPlot, date/time coordinates are double numbers representing the seconds since
  1970-01-01T00:00:00 UTC. This format can be retrieved from QDateTime objects with the
  QDateTime::toTime_t() function. Since this only gives a resolution of one second, there is also
  the QDateTime::toMSecsSinceEpoch() function which returns the timespan described above in
  milliseconds. Divide its return value by 1000.0 to get a value with the format needed for
  date/time plotting, this time with a resolution of one millisecond.
  
  Using the toMSecsSinceEpoch function allows dates that go back to 2nd January 4713 B.C. (i.e. a
  negative number), unlike the toTime_t approach which works with unsigned integers and thus only
  goes back to 1st January 1970.
*/
void QCPAxis::setTickLabelType(LabelType type)
{
  mTickLabelType = type;
}

/*!
  Sets the font of the tick labels, i.e. the numbers drawn next to tick marks.
  
  \see setTickLabelColor
*/
void QCPAxis::setTickLabelFont(const QFont &font)
{
  if (font != mTickLabelFont)
  {
    mTickLabelFont = font;
    mLabelCache.clear();
  }
}

/*!
  Sets the color of the tick labels, i.e. the numbers drawn next to tick marks.
  
  \see setTickLabelFont
*/
void QCPAxis::setTickLabelColor(const QColor &color)
{
  if (color != mTickLabelColor)
  {
    mTickLabelColor = color;
    mLabelCache.clear();
  }
}

/*!
  Sets the rotation of the tick labels, i.e. the numbers drawn next to tick marks. If \a degrees
  is zero, the labels are drawn normally. Else, the tick labels are drawn rotated by \a degrees
  clockwise. The specified angle is bound to values from -90 to 90 degrees.
*/
void QCPAxis::setTickLabelRotation(double degrees)
{
  if (!qFuzzyIsNull(degrees-mTickLabelRotation))
  {
    mTickLabelRotation = qBound(-90.0, degrees, 90.0);
    mLabelCache.clear();
  }
}

/*!
  Sets the format in which dates and times are displayed as tick labels, if \ref setTickLabelType is \ref ltDateTime.
  for details about the \a format string, see the documentation of QDateTime::toString().
  Newlines can be inserted with "\n".
*/
void QCPAxis::setDateTimeFormat(const QString &format)
{
  mDateTimeFormat = format;
}

/*!
  Sets the number format for the numbers drawn as tick labels (if tick label type is \ref
  ltNumber). This \a formatCode is an extended version of the format code used e.g. by
  QString::number() and QLocale::toString(). For reference about that, see the "Argument Formats"
  section in the detailed description of the QString class. \a formatCode is a string of one, two
  or three characters. The first character is identical to the normal format code used by Qt. In
  short, this means: 'e'/'E' scientific format, 'f' fixed format, 'g'/'G' scientific or fixed,
  whichever is shorter.
  
  The second and third characters are optional and specific to QCustomPlot:\n
  If the first char was 'e' or 'g', numbers are/might be displayed in the scientific format, e.g.
  "5.5e9", which is ugly in a plot. So when the second char of \a formatCode is set to 'b' (for
  "beautiful"), those exponential numbers are formatted in a more natural way, i.e. "5.5
  [multiplication sign] 10 [superscript] 9". By default, the multiplication sign is a centered dot.
  If instead a cross should be shown (as is usual in the USA), the third char of \a formatCode can
  be set to 'c'. The inserted multiplication signs are the UTF-8 characters 215 (0xD7) for the
  cross and 183 (0xB7) for the dot.
  
  If the scale type (\ref setScaleType) is \ref stLogarithmic and the \a formatCode uses the 'b'
  option (beautifully typeset decimal powers), the display usually is "1 [multiplication sign] 10
  [superscript] n", which looks unnatural for logarithmic scaling (the "1 [multiplication sign]"
  part). To only display the decimal power, set the number precision to zero with \ref
  setNumberPrecision.
  
  Examples for \a formatCode:
  \li \c g normal format code behaviour. If number is small, fixed format is used, if number is large,
  normal scientific format is used
  \li \c gb If number is small, fixed format is used, if number is large, scientific format is used with
  beautifully typeset decimal powers and a dot as multiplication sign
  \li \c ebc All numbers are in scientific format with beautifully typeset decimal power and a cross as
  multiplication sign
  \li \c fb illegal format code, since fixed format doesn't support (or need) beautifully typeset decimal
  powers. Format code will be reduced to 'f'.
  \li \c hello illegal format code, since first char is not 'e', 'E', 'f', 'g' or 'G'. Current format
  code will not be changed.
*/
void QCPAxis::setNumberFormat(const QString &formatCode)
{
  if (formatCode.length() < 1) return;
  
  // interpret first char as number format char:
  QString allowedFormatChars = "eEfgG";
  if (allowedFormatChars.contains(formatCode.at(0)))
  {
    mNumberFormatChar = formatCode.at(0).toAscii();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Invalid number format code (first char not in 'eEfgG'):" << formatCode;
    return;
  }
  if (formatCode.length() < 2)
  {
    mNumberBeautifulPowers = false;
    mNumberMultiplyCross = false;
    return;
  }
  
  // interpret second char as indicator for beautiful decimal powers:
  if (formatCode.at(1) == 'b' && (mNumberFormatChar == 'e' || mNumberFormatChar == 'g'))
  {
    mNumberBeautifulPowers = true;
  } else
  {
    qDebug() << Q_FUNC_INFO << "Invalid number format code (second char not 'b' or first char neither 'e' nor 'g'):" << formatCode;
    return;
  }
  if (formatCode.length() < 3)
  {
    mNumberMultiplyCross = false;
    return;
  }
  
  // interpret third char as indicator for dot or cross multiplication symbol:
  if (formatCode.at(2) == 'c')
  {
    mNumberMultiplyCross = true;
  } else if (formatCode.at(2) == 'd')
  {
    mNumberMultiplyCross = false;
  } else
  {
    qDebug() << Q_FUNC_INFO << "Invalid number format code (third char neither 'c' nor 'd'):" << formatCode;
    return;
  }
}

/*!
  Sets the precision of the numbers drawn as tick labels. See QLocale::toString(double i, char f,
  int prec) for details. The effect of precisions are most notably for number Formats starting with
  'e', see \ref setNumberFormat

  If the scale type (\ref setScaleType) is \ref stLogarithmic and the number format (\ref
  setNumberFormat) uses the 'b' format code (beautifully typeset decimal powers), the display
  usually is "1 [multiplication sign] 10 [superscript] n", which looks unnatural for logarithmic
  scaling (the "1 [multiplication sign]" part). To only display the decimal power, set \a precision
  to zero.
*/
void QCPAxis::setNumberPrecision(int precision)
{
  mNumberPrecision = precision;
}

/*!
  If \ref setAutoTickStep is set to false, use this function to set the tick step manually.
  The tick step is the interval between (major) ticks, in plot coordinates.
  \see setSubTickCount
*/
void QCPAxis::setTickStep(double step)
{
  mTickStep = step;
}

/*!
  If you want full control over what ticks (and possibly labels) the axes show, this function is
  used to set the coordinates at which ticks will appear.\ref setAutoTicks must be disabled, else
  the provided tick vector will be overwritten with automatically generated tick coordinates. The
  labels of the ticks can either be generated automatically when \ref setAutoTickLabels is left
  enabled, or be set manually with \ref setTickVectorLabels, when \ref setAutoTickLabels is
  disabled.
  
  \a vec is a vector containing the positions of the ticks.

  \see setTickVectorLabels
*/
void QCPAxis::setTickVector(const QVector<double> &vec)
{
  mTickVector = vec;
}

/*!
  If you want full control over what ticks and labels the axes show, this function is used to set a
  number of QStrings that will be displayed at the tick positions which you need to provide with
  \ref setTickVector. These two vectors should have the same size. (Note that you need to disable
  \ref setAutoTicks and \ref setAutoTickLabels first.)
  
  \a vec is a vector containing the labels of the ticks.
  
  \see setTickVector
*/
void QCPAxis::setTickVectorLabels(const QVector<QString> &vec)
{
  mTickVectorLabels = vec;
}

/*!
  Sets the length of the ticks in pixels. \a inside is the length the ticks will reach inside the
  plot and \a outside is the length they will reach outside the plot. If \a outside is greater than
  zero, the tick labels will increase their distance to the axis accordingly, so they won't collide
  with the ticks.
  \see setSubTickLength
*/
void QCPAxis::setTickLength(int inside, int outside)
{
  mTickLengthIn = inside;
  mTickLengthOut = outside;
}

/*!
  Sets the number of sub ticks in one (major) tick step. A sub tick count of three for example,
  divides the tick intervals in four sub intervals.
  
  By default, the number of sub ticks is chosen automatically in a reasonable manner as long as
  the mantissa of the tick step is a multiple of 0.5 (which it is, when \ref setAutoTickStep is enabled).
  If you want to disable automatic sub ticks and use this function to set the count manually, see
  \ref setAutoSubTicks.
*/
void QCPAxis::setSubTickCount(int count)
{
  mSubTickCount = count;
}

/*!
  Sets the length of the subticks in pixels. \a inside is the length the subticks will reach inside the
  plot and \a outside is the length they will reach outside the plot. If \a outside is greater than
  zero, the tick labels will increase their distance to the axis accordingly, so they won't collide
  with the ticks.
  \see setTickLength
*/
void QCPAxis::setSubTickLength(int inside, int outside)
{
  mSubTickLengthIn = inside;
  mSubTickLengthOut = outside;
}

/*!
  Sets the pen, the axis base line is drawn with.
  
  \see setTickPen, setSubTickPen
*/
void QCPAxis::setBasePen(const QPen &pen)
{
  mBasePen = pen;
}

/*!
  Sets the pen, grid lines are drawn with.
  \see setSubGridPen, setZeroLinePen
*/
void QCPAxis::setGridPen(const QPen &pen)
{
  mGrid->setPen(pen);
}

/*!
  Sets the pen, the sub grid lines are drawn with.
  (By default, subgrid drawing needs to be enabled first with \ref setSubGrid.)
  \see setGridPen, setZeroLinePen
*/
void QCPAxis::setSubGridPen(const QPen &pen)
{
  mGrid->setSubGridPen(pen);
}

/*!
  Sets the pen with which a single grid-like line will be drawn at value position zero. The line
  will be drawn instead of a grid line at that position, and not on top. To disable the drawing of
  a zero-line, set \a pen to Qt::NoPen. Then, if \ref setGrid is enabled, a grid line will be
  drawn instead.
  \see setGrid, setGridPen
*/
void QCPAxis::setZeroLinePen(const QPen &pen)
{
  mGrid->setZeroLinePen(pen);
}

/*!
  Sets the pen, tick marks will be drawn with.
  \see setTickLength, setBasePen
*/
void QCPAxis::setTickPen(const QPen &pen)
{
  mTickPen = pen;
}

/*!
  Sets the pen, subtick marks will be drawn with.
  \see setSubTickCount, setSubTickLength, setBasePen
*/
void QCPAxis::setSubTickPen(const QPen &pen)
{
  mSubTickPen = pen;
}

/*!
  Sets the font of the axis label.
  
  \see setLabelColor
*/
void QCPAxis::setLabelFont(const QFont &font)
{
  mLabelFont = font;
}

/*!
  Sets the color of the axis label.
  
  \see setLabelFont
*/
void QCPAxis::setLabelColor(const QColor &color)
{
  mLabelColor = color;
}

/*!
  Sets the axis label that will be shown below/above or next to the axis, depending on its orientation.
*/
void QCPAxis::setLabel(const QString &str)
{
  mLabel = str;
}

/*!
  Sets the distance between the tick labels and the axis label.
  \see setTickLabelPadding, setPadding
*/
void QCPAxis::setLabelPadding(int padding)
{
  mLabelPadding = padding;
}

/*!
  Sets the padding of the axis.

  When \ref QCustomPlot::setAutoMargin is enabled, the padding is the additional distance to the
  respective widget border, that is left blank. If \a padding is zero (default), the auto margin
  mechanism will find a margin that the axis label (or tick label, if no axis label is set) barely
  fits inside the QCustomPlot widget. To give the label closest to the border some freedom,
  increase \a padding.
  
  The axis padding has no meaning if \ref QCustomPlot::setAutoMargin is disabled.
  
  \see setLabelPadding, setTickLabelPadding
*/
void QCPAxis::setPadding(int padding)
{
  mPadding = padding;
}

/*!
  Sets the font that is used for tick labels when they are selected.
  
  \see setTickLabelFont, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedTickLabelFont(const QFont &font)
{
  if (font != mSelectedTickLabelFont)
  {
    mSelectedTickLabelFont = font;
    mLabelCache.clear();
  }
}

/*!
  Sets the font that is used for the axis label when it is selected.
  
  \see setLabelFont, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedLabelFont(const QFont &font)
{
  mSelectedLabelFont = font;
}

/*!
  Sets the color that is used for tick labels when they are selected.
  
  \see setTickLabelColor, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedTickLabelColor(const QColor &color)
{
  if (color != mSelectedTickLabelColor)
  {
    mSelectedTickLabelColor = color;
    mLabelCache.clear();
  }
}

/*!
  Sets the color that is used for the axis label when it is selected.
  
  \see setLabelColor, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedLabelColor(const QColor &color)
{
  mSelectedLabelColor = color;
}

/*!
  Sets the pen that is used to draw the axis base line when selected.
  
  \see setBasePen, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedBasePen(const QPen &pen)
{
  mSelectedBasePen = pen;
}

/*!
  Sets the pen that is used to draw the (major) ticks when selected.
  
  \see setTickPen, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedTickPen(const QPen &pen)
{
  mSelectedTickPen = pen;
}

/*!
  Sets the pen that is used to draw the subticks when selected.
  
  \see setSubTickPen, setSelectable, setSelected, QCustomPlot::setInteractions
*/
void QCPAxis::setSelectedSubTickPen(const QPen &pen)
{
  mSelectedSubTickPen = pen;
}

/*!
  If the scale type (\ref setScaleType) is \ref stLinear, \a diff is added to the lower and upper
  bounds of the range. The range is simply moved by \a diff.
  
  If the scale type is \ref stLogarithmic, the range bounds are multiplied by \a diff. This
  corresponds to an apparent "linear" move in logarithmic scaling by a distance of log(diff).
*/
void QCPAxis::moveRange(double diff)
{
  if (mScaleType == stLinear)
  {
    mRange.lower += diff;
    mRange.upper += diff;
  } else // mScaleType == stLogarithmic
  {
    mRange.lower *= diff;
    mRange.upper *= diff;
  }
  emit rangeChanged(mRange);
}

/*!
  Scales the range of this axis by \a factor around the coordinate \a center. For example, if \a
  factor is 2.0, \a center is 1.0, then the axis range will double its size, and the point at
  coordinate 1.0 won't have changed its position in the QCustomPlot widget (i.e. coordinates
  around 1.0 will have moved symmetrically closer to 1.0).
*/
void QCPAxis::scaleRange(double factor, double center)
{
  
  if (mScaleType == stLinear)
  {
    QCPRange newRange;
    newRange.lower = (mRange.lower-center)*factor + center;
    newRange.upper = (mRange.upper-center)*factor + center;
    if (QCPRange::validRange(newRange))
      mRange = newRange.sanitizedForLinScale();
  } else // mScaleType == stLogarithmic
  {
    if ((mRange.upper < 0 && center < 0) || (mRange.upper > 0 && center > 0)) // make sure center has same sign as range
    {
      QCPRange newRange;
      newRange.lower = pow(mRange.lower/center, factor)*center;
      newRange.upper = pow(mRange.upper/center, factor)*center;
      if (QCPRange::validRange(newRange))
        mRange = newRange.sanitizedForLogScale();
    } else
      qDebug() << Q_FUNC_INFO << "center of scaling operation doesn't lie in same logarithmic sign domain as range:" << center;
  }
  emit rangeChanged(mRange);
}

/*!
  Sets the range of this axis to have a certain scale \a ratio to \a otherAxis. For example, if \a
  ratio is 1, this axis is the \a yAxis and \a otherAxis is \a xAxis, graphs plotted with those
  axes will appear in a 1:1 ratio, independent of the aspect ratio the axis rect has. This is an
  operation that changes the range of this axis once, it doesn't fix the scale ratio indefinitely.
  Consequently calling this function in the constructor won't have the desired effect, since the
  widget's dimensions aren't defined yet, and a resizeEvent will follow.
*/
void QCPAxis::setScaleRatio(const QCPAxis *otherAxis, double ratio)
{
  int otherPixelSize, ownPixelSize;
  
  if (otherAxis->orientation() == Qt::Horizontal)
    otherPixelSize = otherAxis->mAxisRect.width();
  else
    otherPixelSize = otherAxis->mAxisRect.height();
  
  if (orientation() == Qt::Horizontal)
    ownPixelSize = mAxisRect.width();
  else
    ownPixelSize = mAxisRect.height();
  
  double newRangeSize = ratio*otherAxis->mRange.size()*ownPixelSize/(double)otherPixelSize;
  setRange(range().center(), newRangeSize, Qt::AlignCenter);
}

/*!
  Transforms \a value (in pixel coordinates of the QCustomPlot widget) to axis coordinates.
*/
double QCPAxis::pixelToCoord(double value) const
{
  if (orientation() == Qt::Horizontal)
  {
    if (mScaleType == stLinear)
    {
      if (!mRangeReversed)
        return (value-mAxisRect.left())/(double)mAxisRect.width()*mRange.size()+mRange.lower;
      else
        return -(value-mAxisRect.left())/(double)mAxisRect.width()*mRange.size()+mRange.upper;
    } else // mScaleType == stLogarithmic
    {
      if (!mRangeReversed)
        return pow(mRange.upper/mRange.lower, (value-mAxisRect.left())/(double)mAxisRect.width())*mRange.lower;
      else
        return pow(mRange.upper/mRange.lower, (mAxisRect.left()-value)/(double)mAxisRect.width())*mRange.upper;
    }
  } else // orientation() == Qt::Vertical
  {
    if (mScaleType == stLinear)
    {
      if (!mRangeReversed)
        return (mAxisRect.bottom()-value)/(double)mAxisRect.height()*mRange.size()+mRange.lower;
      else
        return -(mAxisRect.bottom()-value)/(double)mAxisRect.height()*mRange.size()+mRange.upper;
    } else // mScaleType == stLogarithmic
    {
      if (!mRangeReversed)
        return pow(mRange.upper/mRange.lower, (mAxisRect.bottom()-value)/(double)mAxisRect.height())*mRange.lower;
      else
        return pow(mRange.upper/mRange.lower, (value-mAxisRect.bottom())/(double)mAxisRect.height())*mRange.upper;
    }
  }
}

/*!
  Transforms \a value (in coordinates of the axis) to pixel coordinates of the QCustomPlot widget.
*/
double QCPAxis::coordToPixel(double value) const
{
  if (orientation() == Qt::Horizontal)
  {
    if (mScaleType == stLinear)
    {
      if (!mRangeReversed)
        return (value-mRange.lower)/mRange.size()*mAxisRect.width()+mAxisRect.left();
      else
        return (mRange.upper-value)/mRange.size()*mAxisRect.width()+mAxisRect.left();
    } else // mScaleType == stLogarithmic
    {
      if (value >= 0 && mRange.upper < 0) // invalid value for logarithmic scale, just draw it outside visible range
        return !mRangeReversed ? mAxisRect.right()+200 : mAxisRect.left()-200;
      else if (value <= 0 && mRange.upper > 0) // invalid value for logarithmic scale, just draw it outside visible range
        return !mRangeReversed ? mAxisRect.left()-200 : mAxisRect.right()+200;
      else
      {
        if (!mRangeReversed)
          return baseLog(value/mRange.lower)/baseLog(mRange.upper/mRange.lower)*mAxisRect.width()+mAxisRect.left();
        else
          return baseLog(mRange.upper/value)/baseLog(mRange.upper/mRange.lower)*mAxisRect.width()+mAxisRect.left();
      }
    }
  } else // orientation() == Qt::Vertical
  {
    if (mScaleType == stLinear)
    {
      if (!mRangeReversed)
        return mAxisRect.bottom()-(value-mRange.lower)/mRange.size()*mAxisRect.height();
      else
        return mAxisRect.bottom()-(mRange.upper-value)/mRange.size()*mAxisRect.height();
    } else // mScaleType == stLogarithmic
    {     
      if (value >= 0 && mRange.upper < 0) // invalid value for logarithmic scale, just draw it outside visible range
        return !mRangeReversed ? mAxisRect.top()-200 : mAxisRect.bottom()+200;
      else if (value <= 0 && mRange.upper > 0) // invalid value for logarithmic scale, just draw it outside visible range
        return !mRangeReversed ? mAxisRect.bottom()+200 : mAxisRect.top()-200;
      else
      {
        if (!mRangeReversed)
          return mAxisRect.bottom()-baseLog(value/mRange.lower)/baseLog(mRange.upper/mRange.lower)*mAxisRect.height();
        else
          return mAxisRect.bottom()-baseLog(mRange.upper/value)/baseLog(mRange.upper/mRange.lower)*mAxisRect.height();
      }
    }
  }
}

/*!
  Returns the part of the axis that is hit by \a pos (in pixels). The return value of this function
  is independent of the user-selectable parts defined with \ref setSelectable. Further, this
  function does not change the current selection state of the axis.
  
  If the axis is not visible (\ref setVisible), this function always returns \ref spNone.
  
  \see setSelected, setSelectable, QCustomPlot::setInteractions
*/
QCPAxis::SelectablePart QCPAxis::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return spNone;
  
  if (mAxisSelectionBox.contains(pos.toPoint()))
    return spAxis;
  else if (mTickLabelsSelectionBox.contains(pos.toPoint()))
    return spTickLabels;
  else if (mLabelSelectionBox.contains(pos.toPoint()))
    return spAxisLabel;
  else
    return spNone;
}

/*! \internal
  
  This function is called before the grid and axis is drawn, in order to prepare the tick vector,
  sub tick vector and tick label vector. If \ref setAutoTicks is set to true, appropriate tick
  values are determined automatically via \ref generateAutoTicks. If it's set to false, the signal
  ticksRequest is emitted, which can be used to provide external tick positions. Then the sub tick
  vectors and tick label vectors are created.
*/
void QCPAxis::setupTickVectors()
{
  if ((!mTicks && !mTickLabels && !mGrid->visible()) || mRange.size() <= 0) return;
  
  // fill tick vectors, either by auto generating or by notifying user to fill the vectors himself
  if (mAutoTicks)
  {
    generateAutoTicks();
  } else
  {
    emit ticksRequest();
  }
  
  visibleTickBounds(mLowestVisibleTick, mHighestVisibleTick);
  if (mTickVector.isEmpty())
  {
    mSubTickVector.clear();
    return;
  }
  
  // generate subticks between ticks:
  mSubTickVector.resize((mTickVector.size()-1)*mSubTickCount);
  if (mSubTickCount > 0)
  {
    double subTickStep = 0;
    double subTickPosition = 0;
    int subTickIndex = 0;
    bool done = false;
    for (int i=1; i<mTickVector.size(); ++i)
    {
      subTickStep = (mTickVector.at(i)-mTickVector.at(i-1))/(double)(mSubTickCount+1);
      for (int k=1; k<=mSubTickCount; ++k)
      {
        subTickPosition = mTickVector.at(i-1) + k*subTickStep;
        if (subTickPosition < mRange.lower)
          continue;
        if (subTickPosition > mRange.upper)
        {
          done = true;
          break;
        }
        mSubTickVector[subTickIndex] = subTickPosition;
        subTickIndex++;
      }
      if (done) break;
    }
    mSubTickVector.resize(subTickIndex);
  }

  // generate tick labels according to tick positions:
  mExponentialChar = mParentPlot->locale().exponential();   // will be needed when drawing the numbers generated here, in drawTickLabel()
  mPositiveSignChar = mParentPlot->locale().positiveSign(); // will be needed when drawing the numbers generated here, in drawTickLabel()
  if (mAutoTickLabels)
  {
    int vecsize = mTickVector.size();
    mTickVectorLabels.resize(vecsize);
    if (mTickLabelType == ltNumber)
    {
      for (int i=0; i<vecsize; ++i)
        mTickVectorLabels[i] = mParentPlot->locale().toString(mTickVector.at(i), mNumberFormatChar, mNumberPrecision);
    } else if (mTickLabelType == ltDateTime)
    {
      for (int i=0; i<vecsize; ++i)
      {
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0) // use fromMSecsSinceEpoch function if available, to gain sub-second accuracy on tick labels (e.g. for format "hh:mm:ss:zzz")
        mTickVectorLabels[i] = mParentPlot->locale().toString(QDateTime::fromTime_t(mTickVector.at(i)), mDateTimeFormat);
#else
        mTickVectorLabels[i] = mParentPlot->locale().toString(QDateTime::fromMSecsSinceEpoch(mTickVector.at(i)*1000), mDateTimeFormat);
#endif
      }
    }
  } else // mAutoTickLabels == false
  {
    if (mAutoTicks) // ticks generated automatically, but not ticklabels, so emit ticksRequest here for labels
    {
      emit ticksRequest();
    }
    // make sure provided tick label vector has correct (minimal) length:
    if (mTickVectorLabels.size() < mTickVector.size())
      mTickVectorLabels.resize(mTickVector.size());
  }
}

/*! \internal
  
  If \ref setAutoTicks is set to true, this function is called by \ref setupTickVectors to
  generate reasonable tick positions (and subtick count). The algorithm tries to create
  approximately <tt>mAutoTickCount</tt> ticks (set via \ref setAutoTickCount), taking into account,
  that tick mantissas that are divisable by two or end in .5 are nice to look at and practical in
  linear scales. If the scale is logarithmic, one tick is generated at every power of the current
  logarithm base, set via \ref setScaleLogBase.
*/
void QCPAxis::generateAutoTicks()
{
  if (mScaleType == stLinear)
  {
    if (mAutoTickStep)
    {
      // Generate tick positions according to linear scaling:
      mTickStep = mRange.size()/(double)(mAutoTickCount+1e-10); // mAutoTickCount ticks on average, the small addition is to prevent jitter on exact integers
      double magnitudeFactor = qPow(10.0, qFloor(qLn(mTickStep)/qLn(10.0))); // get magnitude factor e.g. 0.01, 1, 10, 1000 etc.
      double tickStepMantissa = mTickStep/magnitudeFactor;
      if (tickStepMantissa < 5)
      {
        // round digit after decimal point to 0.5
        mTickStep = (int)(tickStepMantissa*2)/2.0*magnitudeFactor;
      } else
      {
        // round to first digit in multiples of 2
        mTickStep = (int)((tickStepMantissa/10.0)*5)/5.0*10*magnitudeFactor;
      }
    }
    if (mAutoSubTicks)
      mSubTickCount = calculateAutoSubTickCount(mTickStep);
    // Generate tick positions according to mTickStep:
    int firstStep = floor(mRange.lower/mTickStep);
    int lastStep = ceil(mRange.upper/mTickStep);
    int tickcount = lastStep-firstStep+1;
    if (tickcount < 0) tickcount = 0;
    mTickVector.resize(tickcount);
    for (int i=0; i<tickcount; ++i)
    {
      mTickVector[i] = (firstStep+i)*mTickStep;
    }
  } else // mScaleType == stLogarithmic
  {
    // Generate tick positions according to logbase scaling:
    if (mRange.lower > 0 && mRange.upper > 0) // positive range
    {
      double lowerMag = basePow((int)floor(baseLog(mRange.lower)));
      double currentMag = lowerMag;
      mTickVector.clear();
      mTickVector.append(currentMag);
      while (currentMag < mRange.upper && currentMag > 0) // currentMag might be zero for ranges ~1e-300, just cancel in that case
      {
        currentMag *= mScaleLogBase;
        mTickVector.append(currentMag);
      }
    } else if (mRange.lower < 0 && mRange.upper < 0) // negative range
    {
      double lowerMag = -basePow((int)ceil(baseLog(-mRange.lower)));
      double currentMag = lowerMag;
      mTickVector.clear();
      mTickVector.append(currentMag);
      while (currentMag < mRange.upper && currentMag < 0) // currentMag might be zero for ranges ~1e-300, just cancel in that case
      {
        currentMag /= mScaleLogBase;
        mTickVector.append(currentMag);
      }
    } else // invalid range for logarithmic scale, because lower and upper have different sign
    {
      mTickVector.clear();
      qDebug() << Q_FUNC_INFO << "Invalid range for logarithmic plot: " << mRange.lower << "-" << mRange.upper;
    }
  }
}

/*! \internal
  
  Called by generateAutoTicks when \ref setAutoSubTicks is set to true. Depending on the \a
  tickStep between two major ticks on the axis, a different number of sub ticks is appropriate. For
  Example taking 4 sub ticks for a \a tickStep of 1 makes more sense than taking 5 sub ticks,
  because this corresponds to a sub tick step of 0.2, instead of the less intuitive 0.16666. Note
  that a subtick count of 4 means dividing the major tick step into 5 sections.
  
  This is implemented by a hand made lookup for integer tick steps as well as fractional tick steps
  with a fractional part of (approximately) 0.5. If a tick step is different (i.e. has no
  fractional part close to 0.5), the currently set sub tick count (\ref setSubTickCount) is
  returned.
*/
int QCPAxis::calculateAutoSubTickCount(double tickStep) const
{
  int result = mSubTickCount; // default to current setting, if no proper value can be found
  
  // get mantissa of tickstep:
  double magnitudeFactor = qPow(10.0, qFloor(qLn(tickStep)/qLn(10.0))); // get magnitude factor e.g. 0.01, 1, 10, 1000 etc.
  double tickStepMantissa = tickStep/magnitudeFactor;
  
  // separate integer and fractional part of mantissa:
  double epsilon = 0.01;
  double intPartf;
  int intPart;
  double fracPart = modf(tickStepMantissa, &intPartf);
  intPart = intPartf;
  
  // handle cases with (almost) integer mantissa:
  if (fracPart < epsilon || 1.0-fracPart < epsilon)
  {
    if (1.0-fracPart < epsilon)
      intPart++;
    switch (intPart)
    {
      case 1: result = 4; break; // 1.0 -> 0.2 substep
      case 2: result = 3; break; // 2.0 -> 0.5 substep
      case 3: result = 2; break; // 3.0 -> 1.0 substep
      case 4: result = 3; break; // 4.0 -> 1.0 substep
      case 5: result = 4; break; // 5.0 -> 1.0 substep
      case 6: result = 2; break; // 6.0 -> 2.0 substep
      case 7: result = 6; break; // 7.0 -> 1.0 substep
      case 8: result = 3; break; // 8.0 -> 2.0 substep
      case 9: result = 2; break; // 9.0 -> 3.0 substep
    }
  } else
  {
    // handle cases with significantly fractional mantissa:
    if (qAbs(fracPart-0.5) < epsilon) // *.5 mantissa
    {
      switch (intPart)
      {
        case 1: result = 2; break; // 1.5 -> 0.5 substep
        case 2: result = 4; break; // 2.5 -> 0.5 substep
        case 3: result = 4; break; // 3.5 -> 0.7 substep
        case 4: result = 2; break; // 4.5 -> 1.5 substep
        case 5: result = 4; break; // 5.5 -> 1.1 substep (won't occur with autoTickStep from here on)
        case 6: result = 4; break; // 6.5 -> 1.3 substep
        case 7: result = 2; break; // 7.5 -> 2.5 substep
        case 8: result = 4; break; // 8.5 -> 1.7 substep
        case 9: result = 4; break; // 9.5 -> 1.9 substep
      }
    }
    // if mantissa fraction isnt 0.0 or 0.5, don't bother finding good sub tick marks, leave default
  }
  
  return result;
}

/*! \internal
  
  The main draw function of an axis, called by QCustomPlot::draw for each axis. Draws axis
  baseline, major ticks, subticks, tick labels and axis label.
  
  The selection boxes (mAxisSelectionBox, mTickLabelsSelectionBox, mLabelSelectionBox) are set
  here, too.
*/
void QCPAxis::draw(QCPPainter *painter)
{
  QPoint origin;
  if (mAxisType == atLeft)
    origin = mAxisRect.bottomLeft();
  else if (mAxisType == atRight)
    origin = mAxisRect.bottomRight();
  else if (mAxisType == atTop)
    origin = mAxisRect.topLeft();
  else if (mAxisType == atBottom)
    origin = mAxisRect.bottomLeft();
  
  double xCor = 0, yCor = 0; // paint system correction, for pixel exact matches (affects baselines and ticks of top/right axes)
  switch (mAxisType)
  {
    case atTop: yCor = -1; break;
    case atRight: xCor = 1; break;
    default: break;
  }
  
  int margin = 0;
  int lowTick = mLowestVisibleTick;
  int highTick = mHighestVisibleTick;
  double t; // helper variable, result of coordinate-to-pixel transforms

  // draw baseline:
  painter->setPen(getBasePen());
  if (orientation() == Qt::Horizontal)
    painter->drawLine(QLineF(origin+QPointF(xCor, yCor), origin+QPointF(mAxisRect.width()+xCor, yCor)));
  else
    painter->drawLine(QLineF(origin+QPointF(xCor, yCor), origin+QPointF(xCor, -mAxisRect.height()+yCor)));
  
  // draw ticks:
  if (mTicks)
  {
    painter->setPen(getTickPen());
    // direction of ticks ("inward" is right for left axis and left for right axis)
    int tickDir = (mAxisType == atBottom || mAxisType == atRight) ? -1 : 1;
    if (orientation() == Qt::Horizontal)
    {
      for (int i=lowTick; i <= highTick; ++i)
      {
        t = coordToPixel(mTickVector.at(i)); // x
        painter->drawLine(QLineF(t+xCor, origin.y()-mTickLengthOut*tickDir+yCor, t+xCor, origin.y()+mTickLengthIn*tickDir+yCor));
      }
    } else
    {
      for (int i=lowTick; i <= highTick; ++i)
      {
        t = coordToPixel(mTickVector.at(i)); // y
        painter->drawLine(QLineF(origin.x()-mTickLengthOut*tickDir+xCor, t+yCor, origin.x()+mTickLengthIn*tickDir+xCor, t+yCor));
      }
    }
  }
  
  // draw subticks:
  if (mTicks && mSubTickCount > 0)
  {
    painter->setPen(getSubTickPen());
    // direction of ticks ("inward" is right for left axis and left for right axis)
    int tickDir = (mAxisType == atBottom || mAxisType == atRight) ? -1 : 1;
    if (orientation() == Qt::Horizontal)
    {
      for (int i=0; i<mSubTickVector.size(); ++i) // no need to check bounds because subticks are always only created inside current mRange
      {
        t = coordToPixel(mSubTickVector.at(i));
        painter->drawLine(QLineF(t+xCor, origin.y()-mSubTickLengthOut*tickDir+yCor, t+xCor, origin.y()+mSubTickLengthIn*tickDir+yCor));
      }
    } else
    {
      for (int i=0; i<mSubTickVector.size(); ++i)
      {
        t = coordToPixel(mSubTickVector.at(i));
        painter->drawLine(QLineF(origin.x()-mSubTickLengthOut*tickDir+xCor, t+yCor, origin.x()+mSubTickLengthIn*tickDir+xCor, t+yCor));
      }
    }
  }
  margin += qMax(0, qMax(mTickLengthOut, mSubTickLengthOut));
  
  // tick labels:
  QSize tickLabelsSize(0, 0); // size of largest tick label, for offset calculation of axis label
  if (mTickLabels)
  {
    margin += mTickLabelPadding;
    painter->setFont(getTickLabelFont());
    painter->setPen(QPen(getTickLabelColor()));
    for (int i=lowTick; i <= highTick; ++i)
    {
      t = coordToPixel(mTickVector.at(i));
      placeTickLabel(painter, t, margin, mTickVectorLabels.at(i), &tickLabelsSize);
    }
  }
  if (orientation() == Qt::Horizontal)
    margin += tickLabelsSize.height();
  else
    margin += tickLabelsSize.width();

  // axis label:
  QRect labelBounds;
  if (!mLabel.isEmpty())
  {
    margin += mLabelPadding;
    painter->setFont(getLabelFont());
    painter->setPen(QPen(getLabelColor()));
    labelBounds = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip, mLabel);
    if (mAxisType == atLeft)
    {
      QTransform oldTransform = painter->transform();
      painter->translate((origin.x()-margin-labelBounds.height()), origin.y());
      painter->rotate(-90);
      painter->drawText(0, 0, mAxisRect.height(), labelBounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
      painter->setTransform(oldTransform);
    }
    else if (mAxisType == atRight)
    {
      QTransform oldTransform = painter->transform();
      painter->translate((origin.x()+margin+labelBounds.height()), origin.y()-mAxisRect.height());
      painter->rotate(90);
      painter->drawText(0, 0, mAxisRect.height(), labelBounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
      painter->setTransform(oldTransform);
    }
    else if (mAxisType == atTop)
      painter->drawText(origin.x(), origin.y()-margin-labelBounds.height(), mAxisRect.width(), labelBounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
    else if (mAxisType == atBottom)
      painter->drawText(origin.x(), origin.y()+margin, mAxisRect.width(), labelBounds.height(), Qt::TextDontClip | Qt::AlignCenter, mLabel);
  }
  
  // set selection boxes:
  int selAxisOutSize = qMax(qMax(mTickLengthOut, mSubTickLengthOut), mParentPlot->selectionTolerance());
  int selAxisInSize = mParentPlot->selectionTolerance();
  int selTickLabelSize = (orientation()==Qt::Horizontal ? tickLabelsSize.height() : tickLabelsSize.width());
  int selTickLabelOffset = qMax(mTickLengthOut, mSubTickLengthOut)+mTickLabelPadding;
  int selLabelSize = labelBounds.height();
  int selLabelOffset = selTickLabelOffset+selTickLabelSize+mLabelPadding;
  if (mAxisType == atLeft)
  {
    mAxisSelectionBox.setCoords(mAxisRect.left()-selAxisOutSize, mAxisRect.top(), mAxisRect.left()+selAxisInSize, mAxisRect.bottom());
    mTickLabelsSelectionBox.setCoords(mAxisRect.left()-selTickLabelOffset-selTickLabelSize, mAxisRect.top(), mAxisRect.left()-selTickLabelOffset, mAxisRect.bottom());
    mLabelSelectionBox.setCoords(mAxisRect.left()-selLabelOffset-selLabelSize, mAxisRect.top(), mAxisRect.left()-selLabelOffset, mAxisRect.bottom());
  } else if (mAxisType == atRight)
  {
    mAxisSelectionBox.setCoords(mAxisRect.right()-selAxisInSize, mAxisRect.top(), mAxisRect.right()+selAxisOutSize, mAxisRect.bottom());
    mTickLabelsSelectionBox.setCoords(mAxisRect.right()+selTickLabelOffset+selTickLabelSize, mAxisRect.top(), mAxisRect.right()+selTickLabelOffset, mAxisRect.bottom());
    mLabelSelectionBox.setCoords(mAxisRect.right()+selLabelOffset+selLabelSize, mAxisRect.top(), mAxisRect.right()+selLabelOffset, mAxisRect.bottom());
  } else if (mAxisType == atTop)
  {
    mAxisSelectionBox.setCoords(mAxisRect.left(), mAxisRect.top()-selAxisOutSize, mAxisRect.right(), mAxisRect.top()+selAxisInSize);
    mTickLabelsSelectionBox.setCoords(mAxisRect.left(), mAxisRect.top()-selTickLabelOffset-selTickLabelSize, mAxisRect.right(), mAxisRect.top()-selTickLabelOffset);
    mLabelSelectionBox.setCoords(mAxisRect.left(), mAxisRect.top()-selLabelOffset-selLabelSize, mAxisRect.right(), mAxisRect.top()-selLabelOffset);
  } else if (mAxisType == atBottom)
  {
    mAxisSelectionBox.setCoords(mAxisRect.left(), mAxisRect.bottom()-selAxisInSize, mAxisRect.right(), mAxisRect.bottom()+selAxisOutSize);
    mTickLabelsSelectionBox.setCoords(mAxisRect.left(), mAxisRect.bottom()+selTickLabelOffset+selTickLabelSize, mAxisRect.right(), mAxisRect.bottom()+selTickLabelOffset);
    mLabelSelectionBox.setCoords(mAxisRect.left(), mAxisRect.bottom()+selLabelOffset+selLabelSize, mAxisRect.right(), mAxisRect.bottom()+selLabelOffset);
  }
  // draw hitboxes for debug purposes:
  //painter->drawRects(QVector<QRect>() << mAxisSelectionBox << mTickLabelsSelectionBox << mLabelSelectionBox);
}

/*! \internal
  
  Draws a single tick label with the provided \a painter, utilizing the internal label cache to
  significantly speed up drawing of labels that were drawn in previous calls. The tick label is
  always bound to an axis, the distance to the axis is controllable via \a distanceToAxis in
  pixels. The pixel position in the axis direction is passed in the \a position parameter. Hence
  for the bottom axis, \a position would indicate the horizontal pixel position (not coordinate),
  at which the label should be drawn.
  
  In order to later draw the axis label in a place that doesn't overlap with the tick labels, the
  largest tick label size is needed. This is acquired by passing a \a tickLabelsSize to all \ref
  drawTickLabel calls during the process of drawing all tick labels of one axis. \a tickLabelSize
  is only expanded, if the drawn label exceeds the value \a tickLabelsSize currently holds.
  
  The label is drawn with the font and pen that are currently set on the \a painter. To draw
  superscripted powers, the font is temporarily made smaller by a fixed factor.
*/
void QCPAxis::placeTickLabel(QCPPainter *painter, double position, int distanceToAxis, const QString &text, QSize *tickLabelsSize)
{
  // warning: if you change anything here, also adapt getMaxTickLabelSize() accordingly!
  QSize finalSize;
  QPointF labelAnchor;
  switch (mAxisType)
  {
    case atLeft:   labelAnchor = QPointF(mAxisRect.left()-distanceToAxis, position); break;
    case atRight:  labelAnchor = QPointF(mAxisRect.right()+distanceToAxis, position); break;
    case atTop:    labelAnchor = QPointF(position, mAxisRect.top()-distanceToAxis); break;
    case atBottom: labelAnchor = QPointF(position, mAxisRect.bottom()+distanceToAxis); break;
  }
  if (parentPlot()->plottingHints().testFlag(QCP::phCacheLabels)) // label caching enabled
  {
    if (!mLabelCache.contains(text))  // no cached label exists, create it
    {
      CachedLabel *newCachedLabel = new CachedLabel;
      TickLabelData labelData = getTickLabelData(painter->font(), text);
      QPointF drawOffset = getTickLabelDrawOffset(labelData);
      newCachedLabel->offset = drawOffset+labelData.rotatedTotalBounds.topLeft();
      newCachedLabel->pixmap = QPixmap(labelData.rotatedTotalBounds.size());
      newCachedLabel->pixmap.fill(Qt::transparent);
      QCPPainter cachePainter(&newCachedLabel->pixmap);
      cachePainter.setPen(painter->pen());
      drawTickLabel(&cachePainter, -labelData.rotatedTotalBounds.topLeft().x(), -labelData.rotatedTotalBounds.topLeft().y(), labelData);
      mLabelCache.insert(text, newCachedLabel, 1);
    }
    // draw cached label:
    const CachedLabel *cachedLabel = mLabelCache.object(text);
    // if label would be partly clipped by widget border on sides, don't draw it:
    if (orientation() == Qt::Horizontal)
    {
      if (labelAnchor.x()+cachedLabel->offset.x()+cachedLabel->pixmap.width() > mParentPlot->mViewport.right() ||
          labelAnchor.x()+cachedLabel->offset.x() < mParentPlot->mViewport.left())
        return;
    } else
    {
      if (labelAnchor.y()+cachedLabel->offset.y()+cachedLabel->pixmap.height() > mParentPlot->mViewport.bottom() ||
          labelAnchor.y()+cachedLabel->offset.y() < mParentPlot->mViewport.top())
        return;
    }
    painter->drawPixmap(labelAnchor+cachedLabel->offset, cachedLabel->pixmap);
    finalSize = cachedLabel->pixmap.size();
  } else // label caching disabled, draw text directly on surface:
  {
    TickLabelData labelData = getTickLabelData(painter->font(), text);
    QPointF finalPosition = labelAnchor + getTickLabelDrawOffset(labelData);
    // if label would be partly clipped by widget border on sides, don't draw it:
    if (orientation() == Qt::Horizontal)
    {
      if (finalPosition.x()+(labelData.rotatedTotalBounds.width()+labelData.rotatedTotalBounds.left()) > mParentPlot->mViewport.right() ||
          finalPosition.x()+labelData.rotatedTotalBounds.left() < mParentPlot->mViewport.left())
        return;
    } else
    {
      if (finalPosition.y()+(labelData.rotatedTotalBounds.height()+labelData.rotatedTotalBounds.top()) > mParentPlot->mViewport.bottom() ||
          finalPosition.y()+labelData.rotatedTotalBounds.top() < mParentPlot->mViewport.top())
        return;
    }
    drawTickLabel(painter, finalPosition.x(), finalPosition.y(), labelData);
    finalSize = labelData.rotatedTotalBounds.size();
  }
  
  // expand passed tickLabelsSize if current tick label is larger:
  if (finalSize.width() > tickLabelsSize->width()) 
    tickLabelsSize->setWidth(finalSize.width());
  if (finalSize.height() > tickLabelsSize->height())
    tickLabelsSize->setHeight(finalSize.height());
}

/*! \internal
  
  This is a \ref placeTickLabel helper function.
  
  Draws the tick label specified in \a labelData with \a painter at the pixel positions \a x and \a
  y. This function is used by \ref placeTickLabel to create cached tick labels or to directly draw
  the labels on the QCustomPlot surface when label caching is disabled (when QCP::phCacheLabels
  plotting hint is not set).
*/
void QCPAxis::drawTickLabel(QCPPainter *painter, double x, double y, const QCPAxis::TickLabelData &labelData) const
{
  // backup painter settings that we're about to change:
  QTransform oldTransform = painter->transform();
  QFont oldFont = painter->font();
  
  // transform painter to position/rotation:
  painter->translate(x, y);
  if (!qFuzzyIsNull(mTickLabelRotation))
    painter->rotate(mTickLabelRotation);
  
  // draw text:
  if (!labelData.expPart.isEmpty()) // indicator that beautiful powers must be used
  {
    painter->setFont(labelData.baseFont);
    painter->drawText(0, 0, 0, 0, Qt::TextDontClip, labelData.basePart);
    painter->setFont(labelData.expFont);
    painter->drawText(labelData.baseBounds.width()+1, 0, labelData.expBounds.width(), labelData.expBounds.height(), Qt::TextDontClip,  labelData.expPart);
  } else
  {
    painter->drawText(0, 0, labelData.totalBounds.width(), labelData.totalBounds.height(), Qt::TextDontClip | Qt::AlignHCenter, labelData.basePart);
  }
  
  // reset painter settings to what it was before:
  painter->setTransform(oldTransform);
  painter->setFont(oldFont);
}

/*! \internal
  
  This is a \ref placeTickLabel helper function.
  
  Transforms the passed \a text and \a font to a tickLabelData structure that can then be further
  processed by \ref getTickLabelDrawOffset and \ref drawTickLabel. Thus it splits the text into base
  and exponent if necessary (see \ref setNumberFormat) and calculates appropriate bounding boxes.
*/
QCPAxis::TickLabelData QCPAxis::getTickLabelData(const QFont &font, const QString &text) const
{
  TickLabelData result;
  
  // determine whether beautiful decimal powers should be used
  bool useBeautifulPowers = false;
  int ePos = -1;
  if (mAutoTickLabels && mNumberBeautifulPowers && mTickLabelType == ltNumber)
  {
    ePos = text.indexOf('e');
    if (ePos > -1)
      useBeautifulPowers = true;
  }
  
  // calculate text bounding rects and do string preparation for beautiful decimal powers:
  result.baseFont = font;
  result.baseFont.setPointSizeF(result.baseFont.pointSizeF()+0.05); // QFontMetrics.boundingRect has a bug for exact point sizes that make the results oscillate due to internal rounding 
  if (useBeautifulPowers)
  {
    // split text into parts of number/symbol that will be drawn normally and part that will be drawn as exponent:
    result.basePart = text.left(ePos);
    // in log scaling, we want to turn "1*10^n" into "10^n", else add multiplication sign and decimal base:
    if (mScaleType == stLogarithmic && result.basePart == "1")
      result.basePart = "10";
    else
      result.basePart += (mNumberMultiplyCross ? QString(QChar(215)) : QString(QChar(183))) + "10";
    result.expPart = text.mid(ePos+1);
    // clip "+" and leading zeros off expPart:
    while (result.expPart.at(1) == '0' && result.expPart.length() > 2) // length > 2 so we leave one zero when numberFormatChar is 'e'
      result.expPart.remove(1, 1);
    if (result.expPart.at(0) == mPositiveSignChar)
      result.expPart.remove(0, 1);
    // prepare smaller font for exponent:
    result.expFont = font;
    result.expFont.setPointSize(result.expFont.pointSize()*0.75);
    // calculate bounding rects of base part, exponent part and total one:
    result.baseBounds = QFontMetrics(result.baseFont).boundingRect(0, 0, 0, 0, Qt::TextDontClip, result.basePart);
    result.expBounds = QFontMetrics(result.expFont).boundingRect(0, 0, 0, 0, Qt::TextDontClip, result.expPart);
    result.totalBounds = result.baseBounds.adjusted(0, 0, result.expBounds.width(), 0);
  } else // useBeautifulPowers == false
  {
    result.basePart = text;
    result.totalBounds = QFontMetrics(result.baseFont).boundingRect(0, 0, 0, 0, Qt::TextDontClip | Qt::AlignHCenter, result.basePart);
  }
  result.totalBounds.moveTopLeft(QPoint(0, 0)); // want bounding box aligned top left at origin, independent of how it was created, to make further processing simpler
  
  // calculate possibly different bounding rect after rotation:
  result.rotatedTotalBounds = result.totalBounds;
  if (!qFuzzyIsNull(mTickLabelRotation))
  {
    QTransform transform;
    transform.rotate(mTickLabelRotation);
    result.rotatedTotalBounds = transform.mapRect(result.rotatedTotalBounds);
  }
  
  return result;
}

/*! \internal
  
  This is a \ref placeTickLabel helper function.
  
  Calculates the offset at which the top left corner of the specified tick label shall be drawn.
  The offset is relative to a point right next to the tick the label belongs to.
  
  This function is thus responsible for e.g. centering tick labels under ticks and positioning them
  appropriately when they are rotated.
*/
QPointF QCPAxis::getTickLabelDrawOffset(const QCPAxis::TickLabelData &labelData) const
{
  /*
    calculate label offset from base point at tick (non-trivial, for best visual appearance): short
    explanation for bottom axis: The anchor, i.e. the point in the label that is placed
    horizontally under the corresponding tick is always on the label side that is closer to the
    axis (e.g. the left side of the text when we're rotating clockwise). On that side, the height
    is halved and the resulting point is defined the anchor. This way, a 90 degree rotated text
    will be centered under the tick (i.e. displaced horizontally by half its height). At the same
    time, a 45 degree rotated text will "point toward" its tick, as is typical for rotated tick
    labels.
  */
  bool doRotation = !qFuzzyIsNull(mTickLabelRotation);
  bool flip = qFuzzyCompare(qAbs(mTickLabelRotation), 90.0); // perfect +/-90 degree flip. Indicates vertical label centering on vertical axes.
  double radians = mTickLabelRotation/180.0*M_PI;
  int x=0, y=0;
  if (mAxisType == atLeft)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = -qCos(radians)*labelData.totalBounds.width();
        y = flip ? -labelData.totalBounds.width()/2.0 : -qSin(radians)*labelData.totalBounds.width()-qCos(radians)*labelData.totalBounds.height()/2.0;
      } else
      {
        x = -qCos(-radians)*labelData.totalBounds.width()-qSin(-radians)*labelData.totalBounds.height();
        y = flip ? +labelData.totalBounds.width()/2.0 : +qSin(-radians)*labelData.totalBounds.width()-qCos(-radians)*labelData.totalBounds.height()/2.0;
      }
    } else
    {
      x = -labelData.totalBounds.width();
      y = -labelData.totalBounds.height()/2.0;
    }
  } else if (mAxisType == atRight)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = +qSin(radians)*labelData.totalBounds.height();
        y = flip ? -labelData.totalBounds.width()/2.0 : -qCos(radians)*labelData.totalBounds.height()/2.0;
      } else
      {
        x = 0;
        y = flip ? +labelData.totalBounds.width()/2.0 : -qCos(-radians)*labelData.totalBounds.height()/2.0;
      }
    } else
    {
      x = 0;
      y = -labelData.totalBounds.height()/2.0;
    }
  } else if (mAxisType == atTop)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = -qCos(radians)*labelData.totalBounds.width()+qSin(radians)*labelData.totalBounds.height()/2.0;
        y = -qSin(radians)*labelData.totalBounds.width()-qCos(radians)*labelData.totalBounds.height();
      } else
      {
        x = -qSin(-radians)*labelData.totalBounds.height()/2.0;
        y = -qCos(-radians)*labelData.totalBounds.height();
      }
    } else
    {
      x = -labelData.totalBounds.width()/2.0;
      y = -labelData.totalBounds.height();
    }
  } else if (mAxisType == atBottom)
  {
    if (doRotation)
    {
      if (mTickLabelRotation > 0)
      {
        x = +qSin(radians)*labelData.totalBounds.height()/2.0;
        y = 0;
      } else
      {
        x = -qCos(-radians)*labelData.totalBounds.width()-qSin(-radians)*labelData.totalBounds.height()/2.0;
        y = +qSin(-radians)*labelData.totalBounds.width();
      }
    } else
    {
      x = -labelData.totalBounds.width()/2.0;
      y = 0;
    }
  }
  
  return QPointF(x, y);
}

/*! \internal
  
  Simulates the steps done by \ref drawTickLabel by calculating bounding boxes of the text label to
  be drawn, depending on number format etc. Since we only want the largest tick label for the
  margin calculation, the passed \a tickLabelsSize isn't overridden with the calculated label size,
  but only expanded, if it's currently set to a smaller width/height.
*/
void QCPAxis::getMaxTickLabelSize(const QFont &font, const QString &text,  QSize *tickLabelsSize) const
{
  // note: this function must return the same tick label sizes as the placeTickLabel function.
  QSize finalSize;
  if (parentPlot()->plottingHints().testFlag(QCP::phCacheLabels) && mLabelCache.contains(text)) // label caching enabled and have cached label
  {
    const CachedLabel *cachedLabel = mLabelCache.object(text);
    finalSize = cachedLabel->pixmap.size();
  } else // label caching disabled or no label with this text cached:
  {
    TickLabelData labelData = getTickLabelData(font, text);
    finalSize = labelData.rotatedTotalBounds.size();
  }
  
  // expand passed tickLabelsSize if current tick label is larger:
  if (finalSize.width() > tickLabelsSize->width()) 
    tickLabelsSize->setWidth(finalSize.width());
  if (finalSize.height() > tickLabelsSize->height())
    tickLabelsSize->setHeight(finalSize.height());
}

/*! \internal
  
  Handles the selection \a event and returns true when the selection event hit any parts of the
  axis. If the selection state of any parts of the axis was changed, the output parameter \a
  modified is set to true.
  
  When \a additiveSelecton is true, any new selections become selected in addition to the recent
  selections. The recent selections are not cleared. Further, clicking on one object multiple times
  in additive selection mode, toggles the selection of that object on and off.
  
  To indicate that an event deselects the axis (i.e. the parts that are deselectable by the user,
  see \ref setSelectable), pass 0 as \a event.
*/
bool QCPAxis::handleAxisSelection(QMouseEvent *event, bool additiveSelection, bool &modified)
{
  bool selectionFound = false;
  if (event)
  {
    SelectablePart selectedAxisPart = selectTest(event->pos());
    if (selectedAxisPart == spNone || !selectable().testFlag(selectedAxisPart))
    {
      // deselect parts that are changeable (selectable):
      SelectableParts newState = selected() & ~selectable();
      if (newState != selected() && !additiveSelection)
      {
        modified = true;
        setSelected(newState);
      }
    } else
    {
      selectionFound = true;
      if (additiveSelection)
      {
        // additive selection, so toggle selected part:
        setSelected(selected() ^ selectedAxisPart);
        modified = true;
      } else
      {
        // not additive selection, so select part and deselect all others that are changeable (selectable):
        SelectableParts newState = (selected() & ~selectable()) | selectedAxisPart;
        if (newState != selected())
        {
          modified = true;
          setSelected(newState);
        }
      }
    }
  } else // event == 0, so deselect all changeable parts
  {
    SelectableParts newState = selected() & ~selectable();
    if (newState != selected())
    {
      modified = true;
      setSelected(newState);
    }
  }
  return selectionFound;
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing axis lines.

  This is the antialiasing state the painter passed to the \ref draw method is in by default.
  
  This function takes into account the local setting of the antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased
*/
void QCPAxis::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aeAxes);
}

/*! \internal
  
  Returns via \a lowIndex and \a highIndex, which ticks in the current tick vector are visible in
  the current range. The return values are indices of the tick vector, not the positions of the
  ticks themselves.
  
  The actual use of this function is when we have an externally provided tick vector, which might
  exceed far beyond the currently displayed range, and would cause unnecessary calculations e.g. of
  subticks.
*/
void QCPAxis::visibleTickBounds(int &lowIndex, int &highIndex) const
{
  lowIndex = 0;
  highIndex = -1;
  // make sure only ticks that are in visible range are returned
  for (int i=0; i < mTickVector.size(); ++i)
  {
    lowIndex = i;
    if (mTickVector.at(i) >= mRange.lower) break;
  }
  for (int i=mTickVector.size()-1; i >= 0; --i)
  {
    highIndex = i;
    if (mTickVector.at(i) <= mRange.upper) break;
  }
}

/*! \internal
  
  A log function with the base mScaleLogBase, used mostly for coordinate transforms in logarithmic
  scales with arbitrary log base. Uses the buffered mScaleLogBaseLogInv for faster calculation.
  This is set to <tt>1.0/qLn(mScaleLogBase)</tt> in \ref setScaleLogBase.
  
  \see basePow, setScaleLogBase, setScaleType
*/
double QCPAxis::baseLog(double value) const
{
  return qLn(value)*mScaleLogBaseLogInv;
}

/*! \internal
  
  A power function with the base mScaleLogBase, used mostly for coordinate transforms in
  logarithmic scales with arbitrary log base.
  
  \see baseLog, setScaleLogBase, setScaleType
*/
double QCPAxis::basePow(double value) const
{
  return qPow(mScaleLogBase, value);
}

/*! \internal
  
  Returns the pen that is used to draw the axis base line. Depending on the selection state, this
  is either mSelectedBasePen or mBasePen.
*/
QPen QCPAxis::getBasePen() const
{
  return mSelected.testFlag(spAxis) ? mSelectedBasePen : mBasePen;
}

/*! \internal
  
  Returns the pen that is used to draw the (major) ticks. Depending on the selection state, this
  is either mSelectedTickPen or mTickPen.
*/
QPen QCPAxis::getTickPen() const
{
  return mSelected.testFlag(spAxis) ? mSelectedTickPen : mTickPen;
}

/*! \internal
  
  Returns the pen that is used to draw the subticks. Depending on the selection state, this
  is either mSelectedSubTickPen or mSubTickPen.
*/
QPen QCPAxis::getSubTickPen() const
{
  return mSelected.testFlag(spAxis) ? mSelectedSubTickPen : mSubTickPen;
}

/*! \internal
  
  Returns the font that is used to draw the tick labels. Depending on the selection state, this
  is either mSelectedTickLabelFont or mTickLabelFont.
*/
QFont QCPAxis::getTickLabelFont() const
{
  return mSelected.testFlag(spTickLabels) ? mSelectedTickLabelFont : mTickLabelFont;
}

/*! \internal
  
  Returns the font that is used to draw the axis label. Depending on the selection state, this
  is either mSelectedLabelFont or mLabelFont.
*/
QFont QCPAxis::getLabelFont() const
{
  return mSelected.testFlag(spAxisLabel) ? mSelectedLabelFont : mLabelFont;
}

/*! \internal
  
  Returns the color that is used to draw the tick labels. Depending on the selection state, this
  is either mSelectedTickLabelColor or mTickLabelColor.
*/
QColor QCPAxis::getTickLabelColor() const
{
  return mSelected.testFlag(spTickLabels) ? mSelectedTickLabelColor : mTickLabelColor;
}

/*! \internal
  
  Returns the color that is used to draw the axis label. Depending on the selection state, this
  is either mSelectedLabelColor or mLabelColor.
*/
QColor QCPAxis::getLabelColor() const
{
  return mSelected.testFlag(spAxisLabel) ? mSelectedLabelColor : mLabelColor;
}

/*! \internal
  
  Simulates the steps of \ref draw by calculating all appearing text bounding boxes. From this
  information, the appropriate margin for this axis is determined, so nothing is drawn beyond the
  widget border in the actual \ref draw function (if \ref QCustomPlot::setAutoMargin is set to
  true).
  
  The margin consists of: tick label padding, tick label size, label padding, label size. The
  return value is the calculated margin for this axis. Thus, an axis with axis type \ref atLeft
  will return an appropriate left margin, \ref atBottom will return an appropriate bottom margin
  and so forth.
  
  \warning if anything is changed in this function, make sure it's synchronized with the actual
  drawing function \ref draw.
*/
int QCPAxis::calculateMargin() const
{
  // run through similar steps as QCPAxis::draw, and caluclate margin needed to fit axis and its labels
  int margin = 0;
  
  if (mVisible)
  {
    int lowTick, highTick;
    visibleTickBounds(lowTick, highTick);
    // get length of tick marks reaching outside axis rect:
    margin += qMax(0, qMax(mTickLengthOut, mSubTickLengthOut));
    // calculate size of tick labels:
    QSize tickLabelsSize(0, 0);
    if (mTickLabels)
    {
      for (int i=lowTick; i <= highTick; ++i)
      {
        getMaxTickLabelSize(mTickLabelFont, mTickVectorLabels.at(i), &tickLabelsSize); // don't use getTickLabelFont() because we don't want margin to possibly change on selection
      }
      if (orientation() == Qt::Horizontal)
        margin += tickLabelsSize.height() + mTickLabelPadding;
      else
        margin += tickLabelsSize.width() + mTickLabelPadding;
    }
    // calculate size of axis label (only height needed, because left/right labels are rotated by 90 degrees):
    if (!mLabel.isEmpty())
    {
      QFontMetrics fontMetrics(mLabelFont); // don't use getLabelFont() because we don't want margin to possibly change on selection
      QRect bounds;
      bounds = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip | Qt::AlignHCenter | Qt::AlignVCenter, mLabel);
      margin += bounds.height() + mLabelPadding;
    }
  }
  margin += mPadding;
  
  if (margin < 15) // need a bit of margin if no axis text is shown at all (i.e. only baseline and tick lines, or no axis at all)
    margin = 15;
  return margin;
}

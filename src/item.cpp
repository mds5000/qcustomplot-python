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

#include "item.h"

#include "painter.h"
#include "core.h"

// ================================================================================
// =================== QCPItemAnchor
// ================================================================================

/*! \class QCPItemAnchor
  \brief An anchor of an item to which positions can be attached to.
  
  An item (QCPAbstractItem) may have one or more anchors. Unlike QCPItemPosition, an anchor doesn't
  control anything on its item, but provides a way to tie other items via their positions to the
  anchor.

  For example, a QCPItemRect is defined by its positions \a topLeft and \a bottomRight.
  Additionally it has various anchors like \a top, \a topRight or \a bottomLeft etc. So you can
  attach the \a start (which is a QCPItemPosition) of a QCPItemLine to one of the anchors by
  calling QCPItemPosition::setParentAnchor on \a start, passing the wanted anchor of the
  QCPItemRect. This way the start of the line will now always follow the respective anchor location
  on the rect item.
  
  Note that QCPItemPosition derives from QCPItemAnchor, so every position can also serve as an
  anchor to other positions.
  
  To learn how to provide anchors in your own item subclasses, see the subclassing section of the
  QCPAbstractItem documentation.
*/

/*!
  Creates a new QCPItemAnchor. You shouldn't create QCPItemAnchor instances directly, even if
  you want to make a new item subclass. Use \ref QCPAbstractItem::createAnchor instead, as
  explained in the subclassing section of the QCPAbstractItem documentation.
*/
QCPItemAnchor::QCPItemAnchor(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name, int anchorId) :
  mParentPlot(parentPlot),
  mParentItem(parentItem),
  mAnchorId(anchorId),
  mName(name)
{
}

QCPItemAnchor::~QCPItemAnchor()
{
  // unregister as parent at children:
  QList<QCPItemPosition*> currentChildren(mChildren.toList());
  for (int i=0; i<currentChildren.size(); ++i)
    currentChildren.at(i)->setParentAnchor(0); // this acts back on this anchor and child removes itself from mChildren
}

/*!
  Returns the final absolute pixel position of the QCPItemAnchor on the QCustomPlot surface.
  
  The pixel information is internally retrieved via QCPAbstractItem::anchorPixelPosition of the
  parent item, QCPItemAnchor is just an intermediary.
*/
QPointF QCPItemAnchor::pixelPoint() const
{
  if (mParentItem)
  {
    if (mAnchorId > -1)
    {
      return mParentItem->anchorPixelPoint(mAnchorId);
    } else
    {
      qDebug() << Q_FUNC_INFO << "no valid anchor id set:" << mAnchorId;
      return QPointF();
    }
  } else
  {
    qDebug() << Q_FUNC_INFO << "no parent item set";
    return QPointF();
  }
}

/*! \internal

  Adds \a pos to the child list of this anchor. This is necessary to notify the children prior to
  destruction of the anchor.
  
  Note that this function does not change the parent setting in \a pos.
*/
void QCPItemAnchor::addChild(QCPItemPosition *pos)
{
  if (!mChildren.contains(pos))
    mChildren.insert(pos);
  else
    qDebug() << Q_FUNC_INFO << "provided pos is child already" << reinterpret_cast<quintptr>(pos);
}

/*! \internal

  Removes \a pos from the child list of this anchor.
  
  Note that this function does not change the parent setting in \a pos.
*/
void QCPItemAnchor::removeChild(QCPItemPosition *pos)
{
  if (!mChildren.remove(pos))
    qDebug() << Q_FUNC_INFO << "provided pos isn't child" << reinterpret_cast<quintptr>(pos);
}


// ================================================================================
// =================== QCPItemPosition
// ================================================================================

/*! \class QCPItemPosition
  \brief Manages the position of an item.
  
  Every item has at least one public QCPItemPosition member pointer which provides ways to position the
  item on the QCustomPlot surface. Some items have multiple positions, for example QCPItemRect has two:
  \a topLeft and \a bottomRight.

  QCPItemPosition has a type (\ref PositionType) that can be set with \ref setType. This type defines
  how coordinates passed to \ref setCoords are to be interpreted, e.g. as absolute pixel coordinates, as
  plot coordinates of certain axes, etc.

  Further, QCPItemPosition may have a parent QCPItemAnchor, see \ref setParentAnchor. (Note that every
  QCPItemPosition inherits from QCPItemAnchor and thus can itself be used as parent anchor for other
  positions.) This way you can tie multiple items together. If the QCPItemPosition has a parent, the
  coordinates set with \ref setCoords are considered to be absolute values in the reference frame of the
  parent anchor, where (0, 0) means directly ontop of the parent anchor. For example, You could attach
  the \a start position of a QCPItemLine to the \a bottom anchor of a QCPItemText to make the starting
  point of the line always be centered under the text label, no matter where the text is moved to, or is
  itself tied to.

  To set the apparent pixel position on the QCustomPlot surface directly, use \ref setPixelPoint. This
  works no matter what type this QCPItemPosition is or what parent-child situation it is in, as \ref
  setPixelPoint transforms the coordinates appropriately, to make the position appear at the specified
  pixel values.
*/

/*!
  Creates a new QCPItemPosition. You shouldn't create QCPItemPosition instances directly, even if
  you want to make a new item subclass. Use \ref QCPAbstractItem::createPosition instead, as
  explained in the subclassing section of the QCPAbstractItem documentation.
*/
QCPItemPosition::QCPItemPosition(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name) :
  QCPItemAnchor(parentPlot, parentItem, name),
  mPositionType(ptAbsolute),
  mKeyAxis(0),
  mValueAxis(0),
  mKey(0),
  mValue(0),
  mParentAnchor(0)
{
}

QCPItemPosition::~QCPItemPosition()
{
  // unregister as parent at children:
  // Note: this is done in ~QCPItemAnchor again, but it's important QCPItemPosition does it itself, because only then
  //       the setParentAnchor(0) call the correct QCPItemPosition::pixelPos function instead of QCPItemAnchor::pixelPos
  QList<QCPItemPosition*> currentChildren(mChildren.toList());
  for (int i=0; i<currentChildren.size(); ++i)
    currentChildren.at(i)->setParentAnchor(0); // this acts back on this anchor and child removes itself from mChildren
  // unregister as child in parent:
  if (mParentAnchor)
    mParentAnchor->removeChild(this);
}

/*!
  Sets the type of the position. The type defines how the coordinates passed to \ref setCoords
  should be handled and how the QCPItemPosition should behave in the plot. Note that the position
  type \ref ptPlotCoords is only available (and sensible) when the position has no parent anchor
  (\ref setParentAnchor).
  
  The possible values for \a type can be separated in two main categories:

  \li The position is regarded as a point in plot coordinates. This corresponds to \ref ptPlotCoords
  and requires two axes that define the plot coordinate system. They can be specified with \ref setAxes.
  By default, the QCustomPlot's x- and yAxis are used.
  
  \li The position is fixed on the QCustomPlot surface, i.e. independent of axis ranges. This
  corresponds to all other types, i.e. \ref ptAbsolute, \ref ptViewportRatio and \ref ptAxisRectRatio. They
  differ only in the way the absolute position is described, see the documentation of PositionType
  for details.
  
  \note If the type is changed, the apparent pixel position on the plot is preserved. This means
  the coordinates as retrieved with coords() and set with \ref setCoords may change in the process.
*/
void QCPItemPosition::setType(QCPItemPosition::PositionType type)
{
  if (mPositionType != type)
  {
    QPointF pixelP = pixelPoint();
    mPositionType = type;
    setPixelPoint(pixelP);
  }
}

/*!
  Sets the parent of this QCPItemPosition to \a parentAnchor. This means the position will now
  follow any position changes of the anchor. The local coordinate system of positions with a parent
  anchor always is absolute with (0, 0) being exactly on top of the parent anchor. (Hence the type
  shouldn't be \ref ptPlotCoords for positions with parent anchors.)
  
  if \a keepPixelPosition is true, the current pixel position of the QCPItemPosition is preserved
  during reparenting. If it's set to false, the coordinates are set to (0, 0), i.e. the position
  will be exactly on top of the parent anchor.
  
  To remove this QCPItemPosition from any parent anchor, set \a parentAnchor to 0.
  
  \note If the QCPItemPosition previously had no parent and the type is \ref ptPlotCoords, the type
  is set to \ref ptAbsolute, to keep the position in a valid state.
*/
bool QCPItemPosition::setParentAnchor(QCPItemAnchor *parentAnchor, bool keepPixelPosition)
{
  // make sure self is not assigned as parent:
  if (parentAnchor == this)
  {
    qDebug() << Q_FUNC_INFO << "can't set self as parent anchor" << reinterpret_cast<quintptr>(parentAnchor);
    return false;
  }
  // make sure no recursive parent-child-relationships are created:
  QCPItemAnchor *currentParent = parentAnchor;
  while (currentParent)
  {
    if (QCPItemPosition *currentParentPos = dynamic_cast<QCPItemPosition*>(currentParent))
    {
      // is a QCPItemPosition, might have further parent, so keep iterating
      if (currentParentPos == this)
      {
        qDebug() << Q_FUNC_INFO << "can't create recursive parent-child-relationship" << reinterpret_cast<quintptr>(parentAnchor);
        return false;
      }
      currentParent = currentParentPos->mParentAnchor;
    } else
    {
      // is a QCPItemAnchor, can't have further parent, so just compare parent items
      if (currentParent->mParentItem == mParentItem)
      {
        qDebug() << Q_FUNC_INFO << "can't create recursive parent-child-relationship" << reinterpret_cast<quintptr>(parentAnchor);
        return false;
      }
      break;
    }
  }
  
  // if previously no parent set and PosType is still ptPlotCoords, set to ptAbsolute:
  if (!mParentAnchor && mPositionType == ptPlotCoords)
    setType(ptAbsolute);
  
  // save pixel position:
  QPointF pixelP;
  if (keepPixelPosition)
    pixelP = pixelPoint();
  // unregister at current parent anchor:
  if (mParentAnchor)
    mParentAnchor->removeChild(this);
  // register at new parent anchor:
  if (parentAnchor)
    parentAnchor->addChild(this);
  mParentAnchor = parentAnchor;
  // restore pixel position under new parent:
  if (keepPixelPosition)
    setPixelPoint(pixelP);
  else
    setCoords(0, 0);
  return true;
}

/*!
  Sets the coordinates of this QCPItemPosition. What the coordinates mean, is defined by the type
  (\ref setType).
  
  For example, if the type is \ref ptAbsolute, \a key and \a value mean the x and y pixel position
  on the QCustomPlot surface where the origin (0, 0) is in the top left corner of the QCustomPlot
  viewport. If the type is \ref ptPlotCoords, \a key and \a value mean a point in the plot
  coordinate system defined by the axes set by \ref setAxes. (By default the QCustomPlot's x- and
  yAxis.)

  \see setPixelPoint
*/
void QCPItemPosition::setCoords(double key, double value)
{
  mKey = key;
  mValue = value;
}

/*! \overload

  Sets the coordinates as a QPointF \a pos where pos.x has the meaning of \a key and pos.y the
  meaning of \a value of the \ref setCoords(double key, double value) function.
*/
void QCPItemPosition::setCoords(const QPointF &pos)
{
  setCoords(pos.x(), pos.y());
}

/*!
  Returns the final absolute pixel position of the QCPItemPosition on the QCustomPlot surface. It
  includes all effects of type (\ref setType) and possible parent anchors (\ref setParentAnchor).

  \see setPixelPoint
*/
QPointF QCPItemPosition::pixelPoint() const
{
  switch (mPositionType)
  {
    case ptAbsolute:
    {
      if (mParentAnchor)
        return QPointF(mKey, mValue) + mParentAnchor->pixelPoint();
      else
        return QPointF(mKey, mValue);
    }
    
    case ptViewportRatio:
    {
      if (mParentAnchor)
      {
        return QPointF(mKey*mParentPlot->viewport().width(),
                       mValue*mParentPlot->viewport().height()) + mParentAnchor->pixelPoint();
      } else
      {
        return QPointF(mKey*mParentPlot->viewport().width(),
                       mValue*mParentPlot->viewport().height()) + mParentPlot->viewport().topLeft();
      }
    }
      
    case ptAxisRectRatio:
    {
      if (mParentAnchor)
      {
        return QPointF(mKey*mParentPlot->axisRect().width(),
                       mValue*mParentPlot->axisRect().height()) + mParentAnchor->pixelPoint();
      } else
      {
        return QPointF(mKey*mParentPlot->axisRect().width(),
                       mValue*mParentPlot->axisRect().height()) + mParentPlot->axisRect().topLeft();
      }
    }
    
    case ptPlotCoords:
    {
      double x, y;
      if (mKeyAxis && mValueAxis)
      {
        // both key and value axis are given, translate key/value to x/y coordinates:
        if (mKeyAxis->orientation() == Qt::Horizontal)
        {
          x = mKeyAxis->coordToPixel(mKey);
          y = mValueAxis->coordToPixel(mValue);
        } else
        {
          y = mKeyAxis->coordToPixel(mKey);
          x = mValueAxis->coordToPixel(mValue);
        }
      } else if (mKeyAxis)
      {
        // only key axis is given, depending on orientation only transform x or y to key coordinate, other stays pixel:
        if (mKeyAxis->orientation() == Qt::Horizontal)
        {
          x = mKeyAxis->coordToPixel(mKey);
          y = mValue;
        } else
        {
          y = mKeyAxis->coordToPixel(mKey);
          x = mValue;
        }
      } else if (mValueAxis)
      {
        // only value axis is given, depending on orientation only transform x or y to value coordinate, other stays pixel:
        if (mValueAxis->orientation() == Qt::Horizontal)
        {
          x = mValueAxis->coordToPixel(mValue);
          y = mKey;
        } else
        {
          y = mValueAxis->coordToPixel(mValue);
          x = mKey;
        }
      } else
      {
        // no axis given, basically the same as if mAnchorType were atNone
        x = mKey;
        y = mValue;
      }
      return QPointF(x, y);
    }
  }
  return QPointF();
}

/*!
  When \ref setType is ptPlotCoords, this function may be used to specify the axes the coordinates set
  with \ref setCoords relate to.
*/
void QCPItemPosition::setAxes(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
  mKeyAxis = keyAxis;
  mValueAxis = valueAxis;
}

/*!
  Sets the apparent pixel position. This works no matter what type this QCPItemPosition is or what
  parent-child situation it is in, as \ref setPixelPoint transforms the coordinates appropriately, to
  make the position appear at the specified pixel values.

  Only if the type is \ref ptAbsolute and no parent anchor is set, this function is identical to \ref
  setCoords.

  \see setCoords
*/
void QCPItemPosition::setPixelPoint(const QPointF &pixelPoint)
{
  switch (mPositionType)
  {
    case ptAbsolute:
    {
      if (mParentAnchor)
        setCoords(pixelPoint-mParentAnchor->pixelPoint());
      else
        setCoords(pixelPoint);
      break;
    }
      
    case ptViewportRatio:
    {
      if (mParentAnchor)
      {
        QPointF p(pixelPoint-mParentAnchor->pixelPoint());
        p.rx() /= (double)mParentPlot->viewport().width();
        p.ry() /= (double)mParentPlot->viewport().height();
        setCoords(p);
      } else
      {
        QPointF p(pixelPoint-mParentPlot->viewport().topLeft());
        p.rx() /= (double)mParentPlot->viewport().width();
        p.ry() /= (double)mParentPlot->viewport().height();
        setCoords(p);
      }
      break;
    }
      
    case ptAxisRectRatio:
    {
      if (mParentAnchor)
      {
        QPointF p(pixelPoint-mParentAnchor->pixelPoint());
        p.rx() /= (double)mParentPlot->axisRect().width();
        p.ry() /= (double)mParentPlot->axisRect().height();
        setCoords(p);
      } else
      {
        QPointF p(pixelPoint-mParentPlot->axisRect().topLeft());
        p.rx() /= (double)mParentPlot->axisRect().width();
        p.ry() /= (double)mParentPlot->axisRect().height();
        setCoords(p);
      }
      break;
    }
      
    case ptPlotCoords:
    {
      double newKey, newValue;
      if (mKeyAxis && mValueAxis)
      {
        // both key and value axis are given, translate point to key/value coordinates:
        if (mKeyAxis->orientation() == Qt::Horizontal)
        {
          newKey = mKeyAxis->pixelToCoord(pixelPoint.x());
          newValue = mValueAxis->pixelToCoord(pixelPoint.y());
        } else
        {
          newKey = mKeyAxis->pixelToCoord(pixelPoint.y());
          newValue = mValueAxis->pixelToCoord(pixelPoint.x());
        }
      } else if (mKeyAxis)
      {
        // only key axis is given, depending on orientation only transform x or y to key coordinate, other stays pixel:
        if (mKeyAxis->orientation() == Qt::Horizontal)
        {
          newKey = mKeyAxis->pixelToCoord(pixelPoint.x());
          newValue = pixelPoint.y();
        } else
        {
          newKey = mKeyAxis->pixelToCoord(pixelPoint.y());
          newValue = pixelPoint.x();
        }
      } else if (mValueAxis)
      {
        // only value axis is given, depending on orientation only transform x or y to value coordinate, other stays pixel:
        if (mValueAxis->orientation() == Qt::Horizontal)
        {
          newKey = pixelPoint.y();
          newValue = mValueAxis->pixelToCoord(pixelPoint.x());
        } else
        {
          newKey = pixelPoint.x();
          newValue = mValueAxis->pixelToCoord(pixelPoint.y());
        }
      } else
      {
        // no axis given, basically the same as if mAnchorType were atNone
        newKey = pixelPoint.x();
        newValue = pixelPoint.y();
      }
      setCoords(newKey, newValue);
      break;
    }
  }
}


// ================================================================================
// =================== QCPAbstractItem
// ================================================================================

/*! \class QCPAbstractItem
  \brief The abstract base class for all items in a plot.
  
  In QCustomPlot, items are supplemental graphical elements that are neither plottables
  (QCPAbstractPlottable) nor axes (QCPAxis). While plottables are always tied to two axes and thus
  plot coordinates, items can also be placed in absolute coordinates independent of any axes. Each
  specific item has at least one QCPItemPosition member which controls the positioning. Some items
  are defined by more than one coordinate and thus have two or more QCPItemPosition members (For
  example, QCPItemRect has \a topLeft and \a bottomRight).
  
  This abstract base class defines a very basic interface like visibility and clipping. Since this
  class is abstract, it can't be instantiated. Use one of the subclasses or create a subclass
  yourself to create new items.
  
  The built-in items are:
  <table>
  <tr><td>QCPItemLine</td><td>A line defined by a start and an end point. May have different ending styles on each side (e.g. arrows).</td></tr>
  <tr><td>QCPItemStraightLine</td><td>A straight line defined by a start and a direction point. Unlike QCPItemLine, the straight line is infinitely long and has no endings.</td></tr>
  <tr><td>QCPItemCurve</td><td>A curve defined by start, end and two intermediate control points. May have different ending styles on each side (e.g. arrows).</td></tr>
  <tr><td>QCPItemRect</td><td>A rectangle</td></tr>
  <tr><td>QCPItemEllipse</td><td>An ellipse</td></tr>
  <tr><td>QCPItemPixmap</td><td>An arbitrary pixmap</td></tr>
  <tr><td>QCPItemText</td><td>A text label</td></tr>
  <tr><td>QCPItemBracket</td><td>A bracket which may be used to reference/highlight certain parts in the plot.</td></tr>
  <tr><td>QCPItemTracer</td><td>An item that can be attached to a QCPGraph and sticks to its data points, given a key coordinate.</td></tr>
  </table>
  
  \section items-using Using items
  
  First you instantiate the item you want to use and add it to the plot:
  \code
  QCPItemLine *line = new QCPItemLine(customPlot);
  customPlot->addItem(line);
  \endcode
  by default, the positions of the item are bound to the x- and y-Axis of the plot. So we can just
  set the plot coordinates where the line should start/end:
  \code
  line->start->setCoords(-0.1, 0.8);
  line->end->setCoords(1.1, 0.2);
  \endcode
  If we wanted the line to be positioned not in plot coordinates but a different coordinate system,
  e.g. absolute pixel positions on the QCustomPlot surface, we would have changed the position type
  like this:
  \code
  line->start->setType(QCPItemPosition::ptAbsolute);
  line->end->setType(QCPItemPosition::ptAbsolute);
  \endcode
  Then we can set the coordinates, this time in pixels:
  \code
  line->start->setCoords(100, 200);
  line->end->setCoords(450, 320);
  \endcode
  
  \section items-subclassing Creating own items
  
  To create an own item, you implement a subclass of QCPAbstractItem. These are the pure
  virtual functions, you must implement:
  \li \ref selectTest
  \li \ref draw
  
  See the documentation of those functions for what they need to do.
  
  \subsection items-positioning Allowing the item to be positioned
  
  As mentioned, item positions are represented by QCPItemPosition members. Let's assume the new item shall
  have only one point as its position (as opposed to two like a rect or multiple like a polygon). You then add
  a public member of type QCPItemPosition like so:
  
  \code QCPItemPosition * const myPosition;\endcode
  
  the const makes sure the pointer itself can't be modified from the user of your new item (the QCPItemPosition
  instance it points to, can be modified, of course).
  The initialization of this pointer is made easy with the \ref createPosition function. Just assign
  the return value of this function to each QCPItemPosition in the constructor of your item. \ref createPosition
  takes a string which is the name of the position, typically this is identical to the variable name.
  For example, the constructor of QCPItemExample could look like this:
  
  \code
  QCPItemExample::QCPItemExample(QCustomPlot *parentPlot) :
    QCPAbstractItem(parentPlot),
    myPosition(createPosition("myPosition"))
  {
    // other constructor code
  }
  \endcode
  
  \subsection items-drawing The draw function
  
  Your implementation of the draw function should check whether the item is visible (\a mVisible)
  and then draw the item. You can retrieve its position in pixel coordinates from the position
  member(s) via \ref QCPItemPosition::pixelPoint.

  To optimize performance you should calculate a bounding rect first (don't forget to take the pen
  width into account), check whether it intersects the \ref clipRect, and only draw the item at all
  if this is the case.
  
  \subsection items-selection The selectTest function
  
  Your implementation of the \ref selectTest function may use the helpers \ref distSqrToLine and
  \ref rectSelectTest. With these, the implementation of the selection test becomes significantly
  simpler for most items.
  
  \subsection anchors Providing anchors
  
  Providing anchors (QCPItemAnchor) starts off like adding a position. First you create a public
  member, e.g.
  
  \code QCPItemAnchor * const bottom;\endcode

  and create it in the constructor with the \ref createAnchor function, assigning it a name and an
  anchor id (an integer enumerating all anchors on the item, you may create an own enum for this).
  Since anchors can be placed anywhere, relative to the item's position(s), your item needs to
  provide the position of every anchor with the reimplementation of the \ref anchorPixelPoint(int
  anchorId) function.
  
  In essence the QCPItemAnchor is merely an intermediary that itself asks your item for the pixel
  position when anything attached to the anchor needs to know the coordinates.
*/

/* start of documentation of inline functions */

/*! \fn QList<QCPItemPosition*> QCPAbstractItem::positions() const
  
  Returns all positions of the item in a list.
  
  \see anchors, position
*/

/*! \fn QList<QCPItemAnchor*> QCPAbstractItem::anchors() const
  
  Returns all anchors of the item in a list. Note that since a position (QCPItemPosition) is always
  also an anchor, the list will also contain the positions of this item.
  
  \see positions, anchor
*/

/* end of documentation of inline functions */
/* start documentation of pure virtual functions */

/*! \fn double QCPAbstractItem::selectTest(const QPointF &pos) const = 0
  
  This function is used to decide whether a click hits an item or not.

  \a pos is a point in pixel coordinates on the QCustomPlot surface. This function returns the
  shortest pixel distance of this point to the item. If the item is either invisible or the
  distance couldn't be determined, -1.0 is returned. \ref setSelectable has no influence on the
  return value of this function.

  If the item is represented not by single lines but by an area like QCPItemRect or QCPItemText, a
  click inside the area returns a constant value greater zero (typically 99% of the
  selectionTolerance of the parent QCustomPlot). If the click lies outside the area, this function
  returns -1.0.
  
  Providing a constant value for area objects allows selecting line objects even when they are
  obscured by such area objects, by clicking close to the lines (i.e. closer than
  0.99*selectionTolerance).
  
  The actual setting of the selection state is not done by this function. This is handled by the
  parent QCustomPlot when the mouseReleaseEvent occurs.
  
  \see setSelected, QCustomPlot::setInteractions
*/

/*! \fn void QCPAbstractItem::draw(QCPPainter *painter) = 0
  \internal
  
  Draws this item with the provided \a painter. Called by \ref QCustomPlot::draw on all its
  visible items.
  
  The cliprect of the provided painter is set to the rect returned by \ref clipRect before this
  function is called. For items this depends on the clipping settings defined by \ref
  setClipToAxisRect, \ref setClipKeyAxis and \ref setClipValueAxis.
*/

/* end documentation of pure virtual functions */
/* start documentation of signals */

/*! \fn void QCPAbstractItem::selectionChanged(bool selected)
  This signal is emitted when the selection state of this item has changed, either by user interaction
  or by a direct call to \ref setSelected.
*/

/* end documentation of signals */

/*!
  Base class constructor which initializes base class members.
*/
QCPAbstractItem::QCPAbstractItem(QCustomPlot *parentPlot) :
  QCPLayerable(parentPlot),
  mClipToAxisRect(true),
  mClipKeyAxis(parentPlot->xAxis),
  mClipValueAxis(parentPlot->yAxis),
  mSelectable(true),
  mSelected(false)
{
}

QCPAbstractItem::~QCPAbstractItem()
{
  // don't delete mPositions because every position is also an anchor and thus in mAnchors
  qDeleteAll(mAnchors);
}

/*!
  Sets whether the item shall be clipped to the axis rect or whether it shall be visible on the
  entire QCustomPlot. The axis rect is defined by the clip axes which can be set via \ref
  setClipAxes or individually with \ref setClipKeyAxis and \ref setClipValueAxis.
*/
void QCPAbstractItem::setClipToAxisRect(bool clip)
{
  mClipToAxisRect = clip;
}

/*!
  Sets both clip axes. Together they define the axis rect that will be used to clip the item
  when \ref setClipToAxisRect is set to true.
  
  \see setClipToAxisRect, setClipKeyAxis, setClipValueAxis
*/
void QCPAbstractItem::setClipAxes(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
  mClipKeyAxis = keyAxis;
  mClipValueAxis = valueAxis;
}

/*!
  Sets the clip key axis. Together with the clip value axis it defines the axis rect that will be
  used to clip the item when \ref setClipToAxisRect is set to true.
  
  \see setClipToAxisRect, setClipAxes, setClipValueAxis
*/
void QCPAbstractItem::setClipKeyAxis(QCPAxis *axis)
{
  mClipKeyAxis = axis;
}

/*!
  Sets the clip value axis. Together with the clip key axis it defines the axis rect that will be
  used to clip the item when \ref setClipToAxisRect is set to true.
  
  \see setClipToAxisRect, setClipAxes, setClipKeyAxis
*/
void QCPAbstractItem::setClipValueAxis(QCPAxis *axis)
{
  mClipValueAxis = axis;
}

/*!
  Sets whether the user can (de-)select this item by clicking on the QCustomPlot surface.
  (When \ref QCustomPlot::setInteractions contains QCustomPlot::iSelectItems.)
  
  However, even when \a selectable was set to false, it is possible to set the selection manually,
  by calling \ref setSelected directly.
  
  \see QCustomPlot::setInteractions, setSelected
*/
void QCPAbstractItem::setSelectable(bool selectable)
{
  mSelectable = selectable;
}

/*!
  Sets whether this item is selected or not. When selected, it might use a different visual
  appearance (e.g. pen and brush), this depends on the specific item, though.

  The entire selection mechanism for items is handled automatically when \ref
  QCustomPlot::setInteractions contains QCustomPlot::iSelectItems. You only need to call this function when you
  wish to change the selection state manually.
  
  This function can change the selection state even when \ref setSelectable was set to false.
  
  emits the \ref selectionChanged signal when \a selected is different from the previous selection state.
  
  \see selectTest
*/
void QCPAbstractItem::setSelected(bool selected)
{
  if (mSelected != selected)
  {
    mSelected = selected;
    emit selectionChanged(mSelected);
  }
}

/*!
  Returns the QCPItemPosition with the specified \a name. If this item doesn't have a position by
  that name, returns 0.
  
  This function provides an alternative way to access item positions. Normally, you access
  positions direcly by their member pointers (which typically have the same variable name as \a
  name).
  
  \see positions, anchor 
*/
QCPItemPosition *QCPAbstractItem::position(const QString &name) const
{
  for (int i=0; i<mPositions.size(); ++i)
  {
    if (mPositions.at(i)->name() == name)
      return mPositions.at(i);
  }
  qDebug() << Q_FUNC_INFO << "position with name not found:" << name;
  return 0;
}

/*!
  Returns the QCPItemAnchor with the specified \a name. If this item doesn't have an anchor by
  that name, returns 0.
  
  This function provides an alternative way to access item anchors. Normally, you access
  anchors direcly by their member pointers (which typically have the same variable name as \a
  name).
  
  \see anchors, position 
*/
QCPItemAnchor *QCPAbstractItem::anchor(const QString &name) const
{
  for (int i=0; i<mAnchors.size(); ++i)
  {
    if (mAnchors.at(i)->name() == name)
      return mAnchors.at(i);
  }
  qDebug() << Q_FUNC_INFO << "anchor with name not found:" << name;
  return 0;
}

/*!
  Returns whether this item has an anchor with the specified \a name.
  
  Note that you can check for positions with this function, too, because every position is also an
  anchor (QCPItemPosition inherits from QCPItemAnchor).
  
  \see anchor, position 
*/
bool QCPAbstractItem::hasAnchor(const QString &name) const
{
  for (int i=0; i<mAnchors.size(); ++i)
  {
    if (mAnchors.at(i)->name() == name)
      return true;
  }
  return false;
}

/*! \internal
  
  Returns the rect the visual representation of this item is clipped to. This depends on the
  current setting of \ref setClipToAxisRect aswell as the clip axes set with \ref setClipAxes.
  
  If the item is not clipped to an axis rect, the \ref QCustomPlot::viewport rect is returned.
  
  \see draw
*/
QRect QCPAbstractItem::clipRect() const
{
  if (mClipToAxisRect)
  {
    if (mClipKeyAxis && mClipValueAxis)
      return mClipKeyAxis->axisRect() | mClipValueAxis->axisRect();
    else if (mClipKeyAxis)
      return mClipKeyAxis->axisRect();
    else if (mClipValueAxis)
      return mClipValueAxis->axisRect();
  }
  
  return mParentPlot->viewport();
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing item lines.

  This is the antialiasing state the painter passed to the \ref draw method is in by default.
  
  This function takes into account the local setting of the antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased
*/
void QCPAbstractItem::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aeItems);
}

/*! \internal

  Finds the shortest squared distance of \a point to the line segment defined by \a start and \a
  end.
  
  This function may be used to help with the implementation of the \ref selectTest function for
  specific items.
  
  \note This function is identical to QCPAbstractPlottable::distSqrToLine
  
  \see rectSelectTest
*/
double QCPAbstractItem::distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const
{
  QVector2D a(start);
  QVector2D b(end);
  QVector2D p(point);
  QVector2D v(b-a);
  
  double vLengthSqr = v.lengthSquared();
  if (!qFuzzyIsNull(vLengthSqr))
  {
    double mu = QVector2D::dotProduct(p-a, v)/vLengthSqr;
    if (mu < 0)
      return (a-p).lengthSquared();
    else if (mu > 1)
      return (b-p).lengthSquared();
    else
      return ((a + mu*v)-p).lengthSquared();
  } else
    return (a-p).lengthSquared();
}

/*! \internal

  A convenience function which returns the selectTest value for a specified \a rect and a specified
  click position \a pos. \a filledRect defines whether a click inside the rect should also be
  considered a hit or whether only the rect border is sensitive to hits.
  
  This function may be used to help with the implementation of the \ref selectTest function for
  specific items.
  
  For example, if your item consists of four rects, call this function four times, once for each
  rect, in your \ref selectTest reimplementation. Finally, return the minimum of all four returned
  values which were greater or equal to zero. (Because this function may return -1.0 when \a pos
  doesn't hit \a rect at all). If all calls returned -1.0, return -1.0, too, because your item
  wasn't hit.
  
  \see distSqrToLine
*/
double QCPAbstractItem::rectSelectTest(const QRectF &rect, const QPointF &pos, bool filledRect) const
{
  double result = -1;

  // distance to border:
  QList<QLineF> lines;
  lines << QLineF(rect.topLeft(), rect.topRight()) << QLineF(rect.bottomLeft(), rect.bottomRight())
        << QLineF(rect.topLeft(), rect.bottomLeft()) << QLineF(rect.topRight(), rect.bottomRight());
  double minDistSqr = std::numeric_limits<double>::max();
  for (int i=0; i<lines.size(); ++i)
  {
    double distSqr = distSqrToLine(lines.at(i).p1(), lines.at(i).p2(), pos);
    if (distSqr < minDistSqr)
      minDistSqr = distSqr;
  }
  result = qSqrt(minDistSqr);
  
  // filled rect, allow click inside to count as hit:
  if (filledRect && result > mParentPlot->selectionTolerance()*0.99)
  {
    if (rect.contains(pos))
      result = mParentPlot->selectionTolerance()*0.99;
  }
  return result;
}

/*! \internal

  Returns the pixel position of the anchor with Id \a anchorId. This function must be reimplemented in
  item subclasses if they want to provide anchors (QCPItemAnchor).
  
  For example, if the item has two anchors with id 0 and 1, this function takes one of these anchor
  ids and returns the respective pixel points of the specified anchor.
  
  \see createAnchor
*/
QPointF QCPAbstractItem::anchorPixelPoint(int anchorId) const
{
  qDebug() << Q_FUNC_INFO << "called on item which shouldn't have any anchors (anchorPixelPos not reimplemented). anchorId" << anchorId;
  return QPointF();
}

/*! \internal

  Creates a QCPItemPosition, registers it with this item and returns a pointer to it. The specified
  \a name must be a unique string that is usually identical to the variable name of the position
  member (This is needed to provide the name based \ref position access to positions).
  
  Don't delete positions created by this function manually, as the item will take care of it.
  
  Use this function in the constructor (initialization list) of the specific item subclass to
  create each position member. Don't create QCPItemPositions with \b new yourself, because they
  won't be registered with the item properly.
  
  \see createAnchor
*/
QCPItemPosition *QCPAbstractItem::createPosition(const QString &name)
{
  if (hasAnchor(name))
    qDebug() << Q_FUNC_INFO << "anchor/position with name exists already:" << name;
  QCPItemPosition *newPosition = new QCPItemPosition(mParentPlot, this, name);
  mPositions.append(newPosition);
  mAnchors.append(newPosition); // every position is also an anchor
  newPosition->setType(QCPItemPosition::ptPlotCoords);
  newPosition->setAxes(mParentPlot->xAxis, mParentPlot->yAxis);
  newPosition->setCoords(0, 0);
  return newPosition;
}

/*! \internal

  Creates a QCPItemAnchor, registers it with this item and returns a pointer to it. The specified
  \a name must be a unique string that is usually identical to the variable name of the anchor
  member (This is needed to provide the name based \ref anchor access to anchors).
  
  The \a anchorId must be a number identifying the created anchor. It is recommended to create an
  enum (e.g. "AnchorIndex") for this on each item that uses anchors. This id is used by the anchor
  to identify itself when it calls QCPAbstractItem::anchorPixelPoint. That function then returns
  the correct pixel coordinates for the passed anchor id.
  
  Don't delete anchors created by this function manually, as the item will take care of it.
  
  Use this function in the constructor (initialization list) of the specific item subclass to
  create each anchor member. Don't create QCPItemAnchors with \b new yourself, because then they
  won't be registered with the item properly.
  
  \see createPosition
*/
QCPItemAnchor *QCPAbstractItem::createAnchor(const QString &name, int anchorId)
{
  if (hasAnchor(name))
    qDebug() << Q_FUNC_INFO << "anchor/position with name exists already:" << name;
  QCPItemAnchor *newAnchor = new QCPItemAnchor(mParentPlot, this, name, anchorId);
  mAnchors.append(newAnchor);
  return newAnchor;
}

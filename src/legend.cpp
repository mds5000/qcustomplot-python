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

#include "legend.h"

#include "painter.h"
#include "core.h"
#include "plottable.h"

// ================================================================================
// =================== QCPAbstractLegendItem
// ================================================================================

/*! \class QCPAbstractLegendItem
  \brief The abstract base class for all items in a QCPLegend.
  
  It defines a very basic interface to items in a QCPLegend. For representing plottables in the
  legend, the subclass QCPPlottableLegendItem is more suitable.
  
  Only derive directly from this class when you need absolute freedom (i.e. a legend item that's
  not associated with a plottable).

  You must implement the following pure virtual functions:
  \li \ref draw
  \li \ref size
  
  You inherit the following members you may use:
  <table>
    <tr>
      <td>QCPLegend *\b mParentLegend</td>
      <td>A pointer to the parent QCPLegend.</td>
    </tr><tr>
      <td>QFont \b mFont</td>
      <td>The generic font of the item. You should use this font for all or at least the most prominent text of the item.</td>
    </tr>
  </table>
*/

/* start documentation of pure virtual functions */

/*! \fn void QCPAbstractLegendItem::draw(QCPPainter *painter, const QRect &rect) const = 0;
  
  Draws this legend item with \a painter inside the specified \a rect. The \a rect typically has
  the size which was returned from a preceding \ref size call.
*/

/*! \fn QSize QCPAbstractLegendItem::size(const QSize &targetSize) const = 0;

  Returns the size this item occupies in the legend. The legend will adapt its layout with the help
  of this function. If this legend item can have a variable width (e.g. auto-wrapping text), this
  function tries to find a size with a width close to the width of \a targetSize. The height of \a
  targetSize only may have meaning in specific sublasses. Typically, it's ignored.
*/

/* end documentation of pure virtual functions */
/* start of documentation of signals */

/*! \fn void QCPAbstractLegendItem::selectionChanged(bool selected)
  
  This signal is emitted when the selection state of this legend item has changed, either by user interaction
  or by a direct call to \ref setSelected.
*/

/* end of documentation of signals */

/*!
  Constructs a QCPAbstractLegendItem and associates it with the QCPLegend \a parent. This does not
  cause the item to be added to \a parent, so \ref QCPLegend::addItem must be called separately.
*/
QCPAbstractLegendItem::QCPAbstractLegendItem(QCPLegend *parent) : 
  QObject(parent),
  mParentLegend(parent),
  mAntialiased(false),
  mFont(parent->font()),
  mTextColor(parent->textColor()),
  mSelectedFont(parent->selectedFont()),
  mSelectedTextColor(parent->selectedTextColor()),
  mSelectable(true),
  mSelected(false)
{
}

/*!
  Sets whether this legend item is drawn antialiased or not.
  
  Note that this setting may be overridden by \ref QCustomPlot::setAntialiasedElements and \ref
  QCustomPlot::setNotAntialiasedElements.
*/
void QCPAbstractLegendItem::setAntialiased(bool enabled)
{
  mAntialiased = enabled;
}

/*!
  Sets the default font of this specific legend item to \a font.
  
  \see setTextColor, QCPLegend::setFont
*/
void QCPAbstractLegendItem::setFont(const QFont &font)
{
  mFont = font;
}

/*!
  Sets the default text color of this specific legend item to \a color.
  
  \see setFont, QCPLegend::setTextColor
*/
void QCPAbstractLegendItem::setTextColor(const QColor &color)
{
  mTextColor = color;
}

/*!
  When this legend item is selected, \a font is used to draw generic text, instead of the normal
  font set with \ref setFont.
  
  \see setFont, QCPLegend::setSelectedFont
*/
void QCPAbstractLegendItem::setSelectedFont(const QFont &font)
{
  mSelectedFont = font;
}

/*!
  When this legend item is selected, \a color is used to draw generic text, instead of the normal
  color set with \ref setTextColor.
  
  \see setTextColor, QCPLegend::setSelectedTextColor
*/
void QCPAbstractLegendItem::setSelectedTextColor(const QColor &color)
{
  mSelectedTextColor = color;
}

/*!
  Sets whether this specific legend item is selectable.
  
  \see setSelected, QCustomPlot::setInteractions
*/
void QCPAbstractLegendItem::setSelectable(bool selectable)
{
  mSelectable = selectable;
}

/*!
  Sets whether this specific legend item is selected. The selection state of the parent QCPLegend
  is updated correspondingly.
  
  It is possible to set the selection state of this item by calling this function directly, even if
  setSelectable is set to false.
  
  \see setSelectable, QCustomPlot::setInteractions
*/
void QCPAbstractLegendItem::setSelected(bool selected)
{
  if (mSelected != selected)
  {
    mSelected = selected;
    emit selectionChanged(mSelected);
    mParentLegend->updateSelectionState();
  }
}

/*! \internal

  Sets the QPainter::Antialiasing render hint on the provided \a painter, depending on the \ref
  setAntialiased state of this legend item as well as the overrides \ref
  QCustomPlot::setAntialiasedElements and \ref QCustomPlot::setNotAntialiasedElements.
*/
void QCPAbstractLegendItem::applyAntialiasingHint(QCPPainter *painter) const
{
  if (mParentLegend->mParentPlot->notAntialiasedElements().testFlag(QCP::aeLegendItems))
    painter->setAntialiasing(false);
  else if (mParentLegend->mParentPlot->antialiasedElements().testFlag(QCP::aeLegendItems))
    painter->setAntialiasing(true);
  else
    painter->setAntialiasing(mAntialiased);
}


// ================================================================================
// =================== QCPPlottableLegendItem
// ================================================================================

/*! \class QCPPlottableLegendItem
  \brief A legend item representing a plottable with an icon and the plottable name.
  
  This is the standard legend item for plottables. It displays an icon of the plottable next to the
  plottable name. The icon is drawn by the respective plottable itself (\ref
  QCPAbstractPlottable::drawLegendIcon), and tries to give an intuitive symbol for the plottable.
  For example, the QCPGraph draws a centered horizontal line with a single scatter point in the
  middle and filling (if enabled) below.
  
  Legend items of this type are always associated with one plottable (retrievable via the
  plottable() function and settable with the constructor). You may change the font of the plottable
  name with \ref setFont. If \ref setTextWrap is set to true, the plottable name will wrap at the
  right legend boundary (see \ref QCPLegend::setMinimumSize). Icon padding and border pen is taken
  from the parent QCPLegend, see \ref QCPLegend::setIconBorderPen and \ref
  QCPLegend::setIconTextPadding.

  The function \ref QCPAbstractPlottable::addToLegend/\ref QCPAbstractPlottable::removeFromLegend
  creates/removes legend items of this type in the default implementation. However, these functions
  may be reimplemented such that a different kind of legend item (e.g a direct subclass of
  QCPAbstractLegendItem) is used for that plottable.
*/

/*!
  Creates a new legend item associated with \a plottable.
  
  Once it's created, it can be added to the legend via \ref QCPLegend::addItem.
  
  A more convenient way of adding/removing a plottable to/from the legend is via the functions \ref
  QCPAbstractPlottable::addToLegend and \ref QCPAbstractPlottable::removeFromLegend.
*/
QCPPlottableLegendItem::QCPPlottableLegendItem(QCPLegend *parent, QCPAbstractPlottable *plottable) :
  QCPAbstractLegendItem(parent),
  mPlottable(plottable),
  mTextWrap(false)
{
}

/*!
  Sets whether the text of the legend item is wrapped at word boundaries to fit the with of the
  legend.
  
  To prevent the legend autoSize feature (QCPLegend::setAutoSize) from compressing the text too
  strong by wrapping it very often, set an appropriate minimum width with
  QCPLegend::setMinimumSize.
*/
void QCPPlottableLegendItem::setTextWrap(bool wrap)
{
  mTextWrap = wrap;
}

/*! \internal
  
  Returns the pen that shall be used to draw the icon border, taking into account the selection
  state of this item.
*/
QPen QCPPlottableLegendItem::getIconBorderPen() const
{
  return mSelected ? mParentLegend->selectedIconBorderPen() : mParentLegend->iconBorderPen();
}

/*! \internal
  
  Returns the text color that shall be used to draw text, taking into account the selection state
  of this item.
*/
QColor QCPPlottableLegendItem::getTextColor() const
{
  return mSelected ? mSelectedTextColor : mTextColor;
}

/*! \internal
  
  Returns the font that shall be used to draw text, taking into account the selection state of this
  item.
*/
QFont QCPPlottableLegendItem::getFont() const
{
  return mSelected ? mSelectedFont : mFont;
}

/*! \internal
  
  Draws the item with \a painter into \a rect.

  The width of the passed rect is used as text wrapping width, when \ref setTextWrap is enabled.
  The height is ignored. The rect is not used as a clipping rect (overpainting is not prevented
  inside this function), so you should set an appropriate clipping rect on the painter before
  calling this function. Ideally, the width of the rect should be the result of a preceding call to
  \ref size.
*/
void QCPPlottableLegendItem::draw(QCPPainter *painter, const QRect &rect) const
{
  if (!mPlottable) return;
  painter->setFont(getFont());
  painter->setPen(QPen(getTextColor()));
  int iconTextPadding = mParentLegend->iconTextPadding();
  QSize iconSize = mParentLegend->iconSize();
  QRect textRect;
  QRect iconRect(rect.topLeft(), iconSize);
  if (mTextWrap)
  {
    // take width from rect since our text should wrap there (only icon must fit at least):
    textRect = painter->fontMetrics().boundingRect(0, 0, rect.width()-iconTextPadding-iconSize.width(), rect.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
    if (textRect.height() < iconSize.height()) // text smaller than icon, center text vertically in icon height
    {
      painter->drawText(rect.x()+iconSize.width()+iconTextPadding, rect.y(), rect.width()-iconTextPadding-iconSize.width(), iconSize.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
    } else // text bigger than icon, position top of text with top of icon
    {
      painter->drawText(rect.x()+iconSize.width()+iconTextPadding, rect.y(), rect.width()-iconTextPadding-iconSize.width(), textRect.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
    }
  } else
  {
    // text can't wrap (except with explicit newlines), center at current item size (icon size)
    textRect = painter->fontMetrics().boundingRect(0, 0, 0, rect.height(), Qt::TextDontClip, mPlottable->name());
    if (textRect.height() < iconSize.height()) // text smaller than icon, center text vertically in icon height
    {
      painter->drawText(rect.x()+iconSize.width()+iconTextPadding, rect.y(), rect.width(), iconSize.height(), Qt::TextDontClip, mPlottable->name());
    } else // text bigger than icon, position top of text with top of icon
    {
      painter->drawText(rect.x()+iconSize.width()+iconTextPadding, rect.y(), rect.width(), textRect.height(), Qt::TextDontClip, mPlottable->name());
    }
  }
  // draw icon:
  painter->save();
  painter->setClipRect(iconRect, Qt::IntersectClip);
  mPlottable->drawLegendIcon(painter, iconRect);
  painter->restore();
  // draw icon border:
  if (getIconBorderPen().style() != Qt::NoPen)
  {
    painter->setPen(getIconBorderPen());
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(iconRect);
  }
}

/*! \internal
  
  Calculates and returns the size of this item. If \ref setTextWrap is enabled, the width of \a
  targetSize will be used as the text wrapping width. This does not guarantee, that the width of
  the returned QSize is the same as the width of \a targetSize, since wrapping occurs only at word
  boundaries. So a single word that extends beyond the width of \a targetSize, will stretch the
  returned QSize accordingly.
  
  The height of \a targetSize is ignored. The height of the returned QSize is either the height
  of the icon or the height of the text bounding box, whichever is larger.
*/
QSize QCPPlottableLegendItem::size(const QSize &targetSize) const
{
  if (!mPlottable) return QSize();
  QSize result(0, 0);
  QRect textRect;
  QFontMetrics fontMetrics(getFont());
  int iconTextPadding = mParentLegend->iconTextPadding();
  QSize iconSize = mParentLegend->iconSize();
  if (mTextWrap)
  {
    // take width from targetSize since our text can wrap (Only icon must fit at least):
    textRect = fontMetrics.boundingRect(0, 0, targetSize.width()-iconTextPadding-iconSize.width(), iconSize.height(), Qt::TextDontClip | Qt::TextWordWrap, mPlottable->name());
  } else
  {
    // text can't wrap (except with explicit newlines), center at current item size (icon size)
    textRect = fontMetrics.boundingRect(0, 0, 0, iconSize.height(), Qt::TextDontClip, mPlottable->name());
  }
  result.setWidth(iconSize.width() + mParentLegend->iconTextPadding() + textRect.width());
  result.setHeight(qMax(textRect.height(), iconSize.height()));
  return result;
}


// ================================================================================
// =================== QCPLegend
// ================================================================================

/*! \class QCPLegend
  \brief Manages a legend inside a QCustomPlot.

  Doesn't need to be instantiated externally, rather access QCustomPlot::legend
*/

/* start of documentation of signals */

/*! \fn void QCPLegend::selectionChanged(QCPLegend::SelectableParts selection);

  This signal is emitted when the selection state of this legend has changed.
  
  \see setSelected, setSelectable
*/

/* end of documentation of signals */

/*!
  Constructs a new QCPLegend instance with \a parentPlot as the containing plot and default
  values. Under normal usage, QCPLegend needn't be instantiated outside of QCustomPlot.
  Access QCustomPlot::legend to modify the legend (set to invisible by default, see \ref
  setVisible).
*/
QCPLegend::QCPLegend(QCustomPlot *parentPlot) :
  QCPLayerable(parentPlot)
{
  setAntialiased(false);
  setPositionStyle(psTopRight);
  setSize(100, 28);
  setMinimumSize(100, 0);
  setIconSize(32, 18);
  setAutoSize(true);
  
  setMargin(12, 12, 12, 12);
  setPadding(8, 8, 3, 3);
  setItemSpacing(3);
  setIconTextPadding(7);
  
  setSelectable(spLegendBox | spItems);
  setSelected(spNone);
  
  setBorderPen(QPen(Qt::black));
  setSelectedBorderPen(QPen(Qt::blue, 2));
  setIconBorderPen(Qt::NoPen);
  setSelectedIconBorderPen(QPen(Qt::blue, 2));
  setBrush(Qt::white);
  setSelectedBrush(Qt::white);
  setFont(parentPlot->font());
  setSelectedFont(parentPlot->font());
  setTextColor(Qt::black);
  setSelectedTextColor(Qt::blue);
}

QCPLegend::~QCPLegend()
{
  clearItems();
}

/*!
  Sets the pen, the border of the entire legend is drawn with.
*/
void QCPLegend::setBorderPen(const QPen &pen)
{
  mBorderPen = pen;
}

/*!
  Sets the brush of the legend background.
*/
void QCPLegend::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  Sets the default font of legend text. Legend items that draw text (e.g. the name of a graph) will
  use this font by default. However, a different font can be specified on a per-item-basis by
  accessing the specific legend item.
  
  This function will also set \a font on all already existing legend items.
  
  \see QCPAbstractLegendItem::setFont
*/
void QCPLegend::setFont(const QFont &font)
{
  mFont = font;
  for (int i=0; i<mItems.size(); ++i)
    mItems.at(i)->setFont(mFont);
}

/*!
  Sets the default color of legend text. Legend items that draw text (e.g. the name of a graph)
  will use this color by default. However, a different colors can be specified on a per-item-basis
  by accessing the specific legend item.
  
  This function will also set \a color on all already existing legend items.
  
  \see QCPAbstractLegendItem::setTextColor
*/
void QCPLegend::setTextColor(const QColor &color)
{
  mTextColor = color;
  for (int i=0; i<mItems.size(); ++i)
    mItems.at(i)->setTextColor(color);
}

/*!
  Sets the position style of the legend. If the \a legendPositionStyle is not \ref psManual, the
  position is found automatically depending on the specific \a legendPositionStyle and the
  legend margins. If \a legendPositionStyle is \ref psManual, the exact pixel position of the
  legend must be specified via \ref setPosition. Margins have no effect in that case.
  \see setMargin
*/
void QCPLegend::setPositionStyle(PositionStyle legendPositionStyle)
{
  mPositionStyle = legendPositionStyle;
}

/*!
  Sets the exact pixel Position of the legend inside the QCustomPlot widget, if \ref
  setPositionStyle is set to \ref psManual. Margins have no effect in that case.
*/
void QCPLegend::setPosition(const QPoint &pixelPosition)
{
  mPosition = pixelPosition;
}

/*!
  Sets whether the size of the legend should be calculated automatically to fit all the content
  (plus padding), or whether the size must be specified manually with \ref setSize.
  
  If the autoSize mechanism is enabled, the legend will have the smallest possible size to still
  display all its content. For items with text wrapping (QCPPlottableLegendItem::setTextWrap) this
  means, they would become very compressed, i.e. wrapped at every word. To prevent this, set a
  reasonable \ref setMinimumSize width.
*/
void QCPLegend::setAutoSize(bool on)
{
  mAutoSize = on;
}

/*!
  Sets the size of the legend. Setting the size manually with this function only has an effect, if
  \ref setAutoSize is set to false.
  
  If you want to control the minimum size (or the text-wrapping width) while still leaving the
  autoSize mechanism enabled, consider using \ref setMinimumSize.
  
  \see setAutoSize, setMinimumSize
*/
void QCPLegend::setSize(const QSize &size)
{
  mSize = size;
}

/*! \overload
*/
void QCPLegend::setSize(int width, int height)
{
  mSize = QSize(width, height);
}

/*!
  Sets the minimum size of the legend when \ref setAutoSize is enabled.
  
  If text wrapping is enabled in the legend items (e.g. \ref QCPPlottableLegendItem::setTextWrap), this minimum \a size defines the width
  at which the wrapping will occur. Note that the wrapping will happen only at word boundaries, so the actual size might
  still be bigger than the \a size given here, but not smaller.
  
  If \ref setAutoSize is not enabled, the minimum \a size is ignored. Setting a smaller legend size with \ref setSize manually, is not prevented.
  
  \see setAutoSize, setSize, QCPPlottableLegendItem::setTextWrap
*/
void QCPLegend::setMinimumSize(const QSize &size)
{
  mMinimumSize = size;
}

/*! \overload
*/
void QCPLegend::setMinimumSize(int width, int height)
{
  mMinimumSize = QSize(width, height);
}

/*!
  Sets the left padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingLeft(int padding)
{
  mPaddingLeft = padding;
}

/*!
  Sets the right padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingRight(int padding)
{
  mPaddingRight = padding;
}

/*!
  Sets the top padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingTop(int padding)
{
  mPaddingTop = padding;
}

/*!
  Sets the bottom padding of the legend. Padding is the space by what the legend box is made larger
  than minimally needed for the content to fit. I.e. it's the space left blank on each side inside
  the legend.
*/
void QCPLegend::setPaddingBottom(int padding)
{
  mPaddingBottom = padding;
}

/*!
  Sets the padding of the legend. Padding is the space by what the legend box is made larger than
  minimally needed for the content to fit. I.e. it's the space left blank on each side inside the
  legend.
*/
void QCPLegend::setPadding(int left, int right, int top, int bottom)
{
  mPaddingLeft = left;
  mPaddingRight = right;
  mPaddingTop = top;
  mPaddingBottom = bottom;
}

/*!
  Sets the left margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginLeft(int margin)
{
  mMarginLeft = margin;
}

/*!
  Sets the right margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginRight(int margin)
{
  mMarginRight = margin;
}

/*!
  Sets the top margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginTop(int margin)
{
  mMarginTop = margin;
}

/*!
  Sets the bottom margin of the legend. Margins are the distances the legend will keep to the axis
  rect, when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMarginBottom(int margin)
{
  mMarginBottom = margin;
}

/*!
  Sets the margin of the legend. Margins are the distances the legend will keep to the axis rect,
  when \ref setPositionStyle is not \ref psManual.
*/
void QCPLegend::setMargin(int left, int right, int top, int bottom)
{
  mMarginLeft = left;
  mMarginRight = right;
  mMarginTop = top;
  mMarginBottom = bottom;
}

/*!
  Sets the vertical space between two legend items in the legend.
  
  \see setIconTextPadding, setPadding
*/
void QCPLegend::setItemSpacing(int spacing)
{
  mItemSpacing = spacing;
}

/*!
  Sets the size of legend icons. Legend items that draw an icon (e.g. a visual
  representation of the graph) will use this size by default.
*/
void QCPLegend::setIconSize(const QSize &size)
{
  mIconSize = size;
}

/*! \overload
*/
void QCPLegend::setIconSize(int width, int height)
{
  mIconSize.setWidth(width);
  mIconSize.setHeight(height);
}

/*!
  Sets the horizontal space in pixels between the legend icon and the text next to it.
  Legend items that draw an icon (e.g. a visual representation of the graph) and text (e.g. the
  name of the graph) will use this space by default.
  
  \see setItemSpacing
*/
void QCPLegend::setIconTextPadding(int padding)
{
  mIconTextPadding = padding;
}

/*!
  Sets the pen used to draw a border around each legend icon. Legend items that draw an
  icon (e.g. a visual representation of the graph) will use this pen by default.
  
  If no border is wanted, set this to \a Qt::NoPen.
*/
void QCPLegend::setIconBorderPen(const QPen &pen)
{
  mIconBorderPen = pen;
}

/*!
  Sets whether the user can (de-)select the parts in \a selectable by clicking on the QCustomPlot surface.
  (When \ref QCustomPlot::setInteractions contains iSelectLegend.)
  
  However, even when \a selectable is set to a value not allowing the selection of a specific part,
  it is still possible to set the selection of this part manually, by calling \ref setSelected
  directly.
  
  \see SelectablePart, setSelected
*/
void QCPLegend::setSelectable(const SelectableParts &selectable)
{
  mSelectable = selectable;
}

/*!
  Sets the selected state of the respective legend parts described by \ref SelectablePart. When a part
  is selected, it uses a different pen/font and brush. If some legend items are selected and \a selected
  doesn't contain \ref spItems, those items become deselected.
  
  The entire selection mechanism is handled automatically when \ref QCustomPlot::setInteractions
  contains iSelectLegend. You only need to call this function when you wish to change the selection
  state manually.
  
  This function can change the selection state of a part even when \ref setSelectable was set to a
  value that actually excludes the part.
  
  emits the \ref selectionChanged signal when \a selected is different from the previous selection state.
  
  Note that it doesn't make sense to set the selected state \ref spItems here when it wasn't set
  before, because there's no way to specify which exact items to newly select. Do this by calling
  \ref QCPAbstractLegendItem::setSelected directly on the legend item you wish to select.
  
  \see SelectablePart, setSelectable, selectTest, setSelectedBorderPen, setSelectedIconBorderPen, setSelectedBrush,
  setSelectedFont
*/
void QCPLegend::setSelected(const SelectableParts &selected)
{
  if (mSelected != selected)
  {
    if (!selected.testFlag(spItems) && mSelected.testFlag(spItems)) // some items are selected, but new selection state doesn't contain spItems, so deselect them
    {
      for (int i=0; i<mItems.size(); ++i)
        mItems.at(i)->setSelected(false);
      mSelected = selected;
      // not necessary to emit selectionChanged here because this will have happened for the last setSelected(false) on mItems already, via updateSelectionState()
    } else
    {
      mSelected = selected;
      emit selectionChanged(mSelected);
    }
  }
}

/*!
  When the legend box is selected, this pen is used to draw the border instead of the normal pen
  set via \ref setBorderPen.

  \see setSelected, setSelectable, setSelectedBrush
*/
void QCPLegend::setSelectedBorderPen(const QPen &pen)
{
  mSelectedBorderPen = pen;
}

/*!
  Sets the pen legend items will use to draw their icon borders, when they are selected.

  \see setSelected, setSelectable, setSelectedFont
*/
void QCPLegend::setSelectedIconBorderPen(const QPen &pen)
{
  mSelectedIconBorderPen = pen;
}

/*!
  When the legend box is selected, this brush is used to draw the legend background instead of the normal brush
  set via \ref setBrush.

  \see setSelected, setSelectable, setSelectedBorderPen
*/
void QCPLegend::setSelectedBrush(const QBrush &brush)
{
  mSelectedBrush = brush;
}

/*!
  Sets the default font that is used by legend items when they are selected.
  
  This function will also set \a font on all already existing legend items.

  \see setFont, QCPAbstractLegendItem::setSelectedFont
*/
void QCPLegend::setSelectedFont(const QFont &font)
{
  mSelectedFont = font;
  for (int i=0; i<mItems.size(); ++i)
    mItems.at(i)->setSelectedFont(font);
}

/*!
  Sets the default text color that is used by legend items when they are selected.
  
  This function will also set \a color on all already existing legend items.

  \see setTextColor, QCPAbstractLegendItem::setSelectedTextColor
*/
void QCPLegend::setSelectedTextColor(const QColor &color)
{
  mSelectedTextColor = color;
  for (int i=0; i<mItems.size(); ++i)
    mItems.at(i)->setSelectedTextColor(color);
}

/*!
  Returns the item with index \a i.
  
  \see itemCount
*/
QCPAbstractLegendItem *QCPLegend::item(int index) const
{
  if (index >= 0 && index < mItems.size())
    return mItems[index];
  else
    return 0;
}

/*!
  Returns the QCPPlottableLegendItem which is associated with \a plottable (e.g. a \ref QCPGraph*).
  If such an item isn't in the legend, returns 0.
  
  \see hasItemWithPlottable
*/
QCPPlottableLegendItem *QCPLegend::itemWithPlottable(const QCPAbstractPlottable *plottable) const
{
  for (int i=0; i<mItems.size(); ++i)
  {
    if (QCPPlottableLegendItem *pli = qobject_cast<QCPPlottableLegendItem*>(mItems.at(i)))
    {
      if (pli->plottable() == plottable)
        return pli;
    }
  }
  return 0;
}

/*!
  Returns the number of items currently in the legend.
  \see item
*/
int QCPLegend::itemCount() const
{
  return mItems.size();
}

/*!
  Returns whether the legend contains \a item.
*/
bool QCPLegend::hasItem(QCPAbstractLegendItem *item) const
{
  return mItems.contains(item);
}

/*!
  Returns whether the legend contains a QCPPlottableLegendItem which is associated with \a plottable (e.g. a \ref QCPGraph*).
  If such an item isn't in the legend, returns false.
  
  \see itemWithPlottable
*/
bool QCPLegend::hasItemWithPlottable(const QCPAbstractPlottable *plottable) const
{
  return itemWithPlottable(plottable);
}

/*!
  Adds \a item to the legend, if it's not present already.
  
  Returns true on sucess, i.e. if the item wasn't in the list already and has been successfuly added.
  
  The legend takes ownership of the item.
*/
bool QCPLegend::addItem(QCPAbstractLegendItem *item)
{
  if (!mItems.contains(item))
  {
    mItems.append(item);
    return true;
  } else
    return false;
}

/*!
  Removes the item with index \a index from the legend.

  Returns true, if successful.
  
  \see itemCount, clearItems
*/
bool QCPLegend::removeItem(int index)
{
  if (index >= 0 && index < mItems.size())
  {
    mItemBoundingBoxes.remove(mItems.at(index));
    delete mItems.at(index);
    mItems.removeAt(index);
    return true;
  } else
    return false;
}

/*! \overload
  
  Removes \a item from the legend.

  Returns true, if successful.
  
  \see clearItems
*/
bool QCPLegend::removeItem(QCPAbstractLegendItem *item)
{
  return removeItem(mItems.indexOf(item));
}

/*!
  Removes all items from the legend.
*/
void QCPLegend::clearItems()
{
  qDeleteAll(mItems);
  mItems.clear();
  mItemBoundingBoxes.clear();
}


/*!
  Returns the legend items that are currently selected. If no items are selected,
  the list is empty.
  
  \see QCPAbstractLegendItem::setSelected, setSelectable
*/
QList<QCPAbstractLegendItem *> QCPLegend::selectedItems() const
{
  QList<QCPAbstractLegendItem*> result;
  for (int i=0; i<mItems.size(); ++i)
  {
    if (mItems.at(i)->selected())
      result.append(mItems.at(i));
  }
  return result;
}

/*!
  If \ref setAutoSize is true, the size needed to fit all legend contents is calculated and applied.
  Finally, the automatic positioning of the legend is performed, depending on the \ref
  setPositionStyle setting.
*/
void  QCPLegend::reArrange()
{
  if (mAutoSize)
  {
    calculateAutoSize();
  }
  calculateAutoPosition();
}

/*!
  Returns whether the point \a pos in pixels hits the legend rect.
  
  \see selectTestItem
*/
bool QCPLegend::selectTestLegend(const QPointF &pos) const
{
  return QRect(mPosition, mSize).contains(pos.toPoint());
}

/*!
  When the point \a pos in pixels hits a legend item, the item is returned. If no item is hit, 0 is
  returned.
  
  \see selectTestLegend
*/
QCPAbstractLegendItem *QCPLegend::selectTestItem(const QPoint pos) const
{
  QMap<QCPAbstractLegendItem*, QRect>::const_iterator it;
  for (it = mItemBoundingBoxes.constBegin(); it != mItemBoundingBoxes.constEnd(); ++it)
  {
    if (it.value().contains(pos) && mItems.contains(it.key()))
      return it.key();
  }
  return 0;
}

/*! \internal
  
  Updates the spItems part of the selection state of this legend by going through all child items
  and checking their selected state.
  
  If no items are selected and the current selected state contains spItems, it is removed and the
  \ref selectionChanged signal is emitted. If at least one item is selected and the current selection
  state does not contain spItems, it is added and the signal is emitted, too.
  
  This function is called in the QCPAbstractLegendItem::setSelected functions to propagate their
  change to the parent legend.
*/
void QCPLegend::updateSelectionState()
{
  bool hasSelections = false;
  for (int i=0; i<mItems.size(); ++i)
  {
    if (mItems.at(i)->selected())
    {
      hasSelections = true;
      break;
    }
  }
  
  // in the following we don't use setSelected because it would cause unnecessary
  // logic looping through items if spItems isn't set in the new state. (look at setSelected and you'll understand)
  if (hasSelections && !mSelected.testFlag(spItems))
  {
    mSelected |= spItems;
    emit selectionChanged(mSelected);
  } else if (!hasSelections && mSelected.testFlag(spItems))
  {
    mSelected &= ~spItems;
    emit selectionChanged(mSelected);
  }
}

/*! \internal
  
  Handles the selection \a event and returns true when the selection event hit any parts of the
  legend. If the selection state of any parts of the legend was changed, the output parameter \a
  modified is set to true.
  
  When \a additiveSelecton is true, any new selections become selected in addition to the recent
  selections. The recent selections are not cleared. Further, clicking on one object multiple times
  in additive selection mode, toggles the selection of that object on and off.
  
  To indicate that an event deselects the legend (i.e. the parts that are deselectable by the user,
  see \ref setSelectable), pass 0 as \a event.
*/
bool QCPLegend::handleLegendSelection(QMouseEvent *event, bool additiveSelection, bool &modified)
{
  modified = false;
  bool selectionFound = false;
  
  if (event && selectTestLegend(event->pos())) // clicked inside legend somewhere
  {
    QCPAbstractLegendItem *ali = selectTestItem(event->pos());
    if (selectable().testFlag(QCPLegend::spItems) && ali && ali->selectable()) // items shall be selectable and item ali was clicked 
    {
      selectionFound = true;
      // deselect legend box:
      if (!additiveSelection && selected().testFlag(QCPLegend::spLegendBox) && selectable().testFlag(QCPLegend::spLegendBox))
        setSelected(selected() & ~QCPLegend::spLegendBox);
      // first select clicked item:
      if (!ali->selected() || additiveSelection) // if additive selection, we toggle selection on and off per click
      {
        modified = true;
        ali->setSelected(!ali->selected());
      }
      // finally, deselect all other items (if we had deselected all first, the selectionChanged signal of QCPLegend might have been emitted twice):
      if (!additiveSelection)
      {
        for (int i=0; i<itemCount(); ++i)
        {
          if (item(i) != ali && item(i)->selected() && item(i)->selectable())
          {
            modified = true;
            item(i)->setSelected(false);
          }
        }
      }
    } else // no specific item clicked or items not selectable
    {
      // if items actually were selectable, this means none were clicked, deselect them:
      if (selectable().testFlag(QCPLegend::spItems) && selected().testFlag(QCPLegend::spItems) && !additiveSelection)
      {
        for (int i=0; i<itemCount(); ++i)
        {
          if (item(i)->selectable())
            item(i)->setSelected(false);
        }
        modified = true;
      }
      // if legend box is selectable, select it:
      if (selectable().testFlag(QCPLegend::spLegendBox))
      {
        if (!selected().testFlag(QCPLegend::spLegendBox) || additiveSelection)
        {
          selectionFound = true;
          setSelected(selected() ^ QCPLegend::spLegendBox); // xor because we always toggle
          modified = true;
        }
      }
    }
  } else if (selected() != QCPLegend::spNone && selectable() != QCPLegend::spNone && !additiveSelection) // legend not clicked, deselect it if selectable allows that (and all child items)
  {
    // only deselect parts that are allowed to be changed by user according to selectable()
    // deselect child items (and automatically removes spItems from selected state of legend, if last item gets deselected):
    if (selectable().testFlag(spItems)) 
    {
      for (int i=0; i<itemCount(); ++i)
      {
        if (item(i)->selected() && item(i)->selectable())
        {
          item(i)->setSelected(false);
          modified = true;
        }
      }
    }
    // only deselect parts that are allowed to be changed (are selectable). Don't forcibly remove
    // spItems, because some selected items might not be selectable, i.e. allowed to be deselected
    // by user interaction. If that's not the case, spItems will have been removed from selected()
    // state in previous loop by individual setSelected(false) calls on the items anyway.
    QCPLegend::SelectableParts newState = selected() & ~(selectable()&~spItems);
    if (newState != selected())
    {
      setSelected(newState);
      modified = true;
    }
  }
  
  return selectionFound;
}

/*! \internal

  A convenience function to easily set the QPainter::Antialiased hint on the provided \a painter
  before drawing main legend elements.

  This is the antialiasing state the painter passed to the \ref draw method is in by default.
  
  This function takes into account the local setting of the antialiasing flag as well as
  the overrides set e.g. with \ref QCustomPlot::setNotAntialiasedElements.
  
  \see setAntialiased
*/
void QCPLegend::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aeLegend);
}

/*! \internal
  
  Returns the pen used to paint the border of the legend, taking into account the selection state
  of the legend box.
*/
QPen QCPLegend::getBorderPen() const
{
  return mSelected.testFlag(spLegendBox) ? mSelectedBorderPen : mBorderPen;
}

/*! \internal
  
  Returns the brush used to paint the background of the legend, taking into account the selection
  state of the legend box.
*/
QBrush QCPLegend::getBrush() const
{
  return mSelected.testFlag(spLegendBox) ? mSelectedBrush : mBrush;
}

/*! \internal
  
  Draws the legend with the provided \a painter.
*/
void QCPLegend::draw(QCPPainter *painter)
{
  painter->setBrush(getBrush());
  painter->setPen(getBorderPen());
  // draw background rect:
  painter->drawRect(QRect(mPosition, mSize));
  // draw legend items:
  painter->setClipRect(QRect(mPosition, mSize).adjusted(1, 1, 0, 0));
  painter->setPen(QPen());
  painter->setBrush(Qt::NoBrush);
  int currentTop = mPosition.y()+mPaddingTop;
  for (int i=0; i<mItems.size(); ++i)
  {
    QSize itemSize = mItems.at(i)->size(QSize(mSize.width(), 0));
    QRect itemRect = QRect(QPoint(mPosition.x()+mPaddingLeft, currentTop), itemSize);
    mItemBoundingBoxes.insert(mItems.at(i), itemRect);
    painter->save();
    mItems.at(i)->applyAntialiasingHint(painter);
    mItems.at(i)->draw(painter, itemRect);
    painter->restore();
    currentTop += itemSize.height()+mItemSpacing;
  }
}

/*! \internal 
  
  Goes through similar steps as \ref draw and calculates the width and height needed to
  fit all items and padding in the legend. The new calculated size is then applied to the mSize of
  this legend.
*/
void QCPLegend::calculateAutoSize()
{
  int width = mMinimumSize.width()-mPaddingLeft-mPaddingRight; // start with minimum width and only expand from there
  int currentTop;
  bool repeat = true;
  int repeatCount = 0;
  while (repeat && repeatCount < 3) // repeat until we find self-consistent width (usually 2 runs)
  {
    repeat = false;
    currentTop = mPaddingTop;
    for (int i=0; i<mItems.size(); ++i)
    {
      QSize s = mItems.at(i)->size(QSize(width, 0));
      currentTop += s.height();
      if (i < mItems.size()-1) // vertical spacer for all but last item
        currentTop += mItemSpacing;
      if (width < s.width())
      {
        width = s.width();
        repeat = true; // changed width, so need a new run with new width to let other items adapt their height to that new width
      }
    }
    repeatCount++;
  }
  if (repeat)
    qDebug() << Q_FUNC_INFO << "hit repeat limit for iterative width calculation";
  currentTop += mPaddingBottom;
  width += mPaddingLeft+mPaddingRight;
  
  mSize.setWidth(width);
  if (currentTop > mMinimumSize.height())
    mSize.setHeight(currentTop);
  else
    mSize.setHeight(mMinimumSize.height());
}

/*! \internal
  
  Sets the position dependant on the \ref setPositionStyle setting and the margins.
*/
void QCPLegend::calculateAutoPosition()
{
  switch (mPositionStyle)
  {
    case psTopLeft:
      mPosition = mParentPlot->mAxisRect.topLeft() + QPoint(mMarginLeft, mMarginTop); break;
    case psTop:
      mPosition = mParentPlot->mAxisRect.topLeft() + QPoint(mParentPlot->mAxisRect.width()/2.0-mSize.width()/2.0, mMarginTop); break;
    case psTopRight:
      mPosition = mParentPlot->mAxisRect.topRight() + QPoint(-mMarginRight-mSize.width(), mMarginTop); break;
    case psRight:
      mPosition = mParentPlot->mAxisRect.topRight() + QPoint(-mMarginRight-mSize.width(), mParentPlot->mAxisRect.height()/2.0-mSize.height()/2.0); break;
    case psBottomRight:
      mPosition = mParentPlot->mAxisRect.bottomRight() + QPoint(-mMarginRight-mSize.width(), -mMarginBottom-mSize.height()); break;
    case psBottom:
      mPosition = mParentPlot->mAxisRect.bottomLeft() + QPoint(mParentPlot->mAxisRect.width()/2.0-mSize.width()/2.0, -mMarginBottom-mSize.height()); break;
    case psBottomLeft:
      mPosition = mParentPlot->mAxisRect.bottomLeft() + QPoint(mMarginLeft, -mMarginBottom-mSize.height()); break;
    case psLeft:
      mPosition = mParentPlot->mAxisRect.topLeft() + QPoint(mMarginLeft, mParentPlot->mAxisRect.height()/2.0-mSize.height()/2.0); break;
    case psManual: break;
  }
}

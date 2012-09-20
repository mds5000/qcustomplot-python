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

#ifndef QCP_LAYER_H
#define QCP_LAYER_H

#include "global.h"

class QCPPainter;
class QCustomPlot;
class QCPLayerable;

class QCP_LIB_DECL QCPLayer
{
public:
  QCPLayer(QCustomPlot* parentPlot, const QString &layerName);
  ~QCPLayer();
  
  // getters:
  QCustomPlot *parentPlot() const { return mParentPlot; }
  QString name() const { return mName; }
  int index() const;
  QList<QCPLayerable*> children() const { return mChildren; }
  
protected:
  QCustomPlot *mParentPlot;
  QString mName;
  QList<QCPLayerable*> mChildren;
  
  void addChild(QCPLayerable *layerable, bool prepend);
  void removeChild(QCPLayerable *layerable);
  
private:
  Q_DISABLE_COPY(QCPLayer)
  
  friend class QCPLayerable;
};

class QCP_LIB_DECL QCPLayerable : public QObject
{
  Q_OBJECT
public:
  QCPLayerable(QCustomPlot *parentPlot);
  ~QCPLayerable();
  
  // getters:
  bool visible() const { return mVisible; }
  QCustomPlot *parentPlot() const { return mParentPlot; }
  QCPLayer *layer() const { return mLayer; }
  bool antialiased() const { return mAntialiased; }
  
  // setters:
  void setVisible(bool on);
  bool setLayer(QCPLayer *layer);
  bool setLayer(const QString &layerName);
  void setAntialiased(bool enabled);
  
protected:
  bool mVisible;
  QCustomPlot *mParentPlot;
  QCPLayer *mLayer;
  bool mAntialiased;
  
  // non-property methods:
  bool moveToLayer(QCPLayer *layer, bool prepend);
  
  void applyAntialiasingHint(QCPPainter *painter, bool localAntialiased, QCP::AntialiasedElement overrideElement) const;
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const = 0;
  virtual QRect clipRect() const;
  virtual void draw(QCPPainter *painter) = 0;
  
private:
  Q_DISABLE_COPY(QCPLayerable)
  
  friend class QCustomPlot;
};

#endif // QCP_LAYER_H

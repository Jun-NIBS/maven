#include "tinyplot.h"

TinyPlot::TinyPlot(QGraphicsItem* parent, QGraphicsScene *scene, MainWindow *mainwindow):QGraphicsItem(parent)  {
	_width=100;
	_height=100;
	_currentXCoord=0;
	_minXValue = _minYValue = _maxXValue = _maxYValue = 0;
	//QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
	//effect->setOffset(8); 
	//setGraphicsEffect(effect);
    if(scene) scene->addItem(this);
    if (mainwindow) mw = mainwindow;
}


QRectF TinyPlot::boundingRect() const
{
    return(QRectF(0,0,_width,_height));
}

void TinyPlot::addDataColor(QColor c) {
	colors << c;
}

void TinyPlot::addData(QVector<float>&v) { 
	QVector<QPointF>d;
	for(int i=0; i < v.size(); i++ ) d << QPointF(i,v[i]);
	data << d;
}

void TinyPlot::addData(vector<float>&v) { 
	QVector<QPointF>d;
	for(int i=0; i < v.size(); i++ ) d << QPointF(i,v[i]);
	data << d;
}

void TinyPlot::addData(EIC* eic) {
	if(!eic) return;
	QVector<QPointF>d;
	for(int i=0; i<eic->size();i++) {
		d<< QPointF( eic->rt[i], eic->intensity[i]);
	}
	data << d;
}


void TinyPlot::addData(EIC* eic,float rtmin, float rtmax) {
    if(!eic) return;
    QVector<QPointF>d;
    for(int i=0; i<eic->size();i++) {
        if(eic->rt[i] >= rtmin and eic->rt[i] <= rtmax ) {

            d<< QPointF( eic->rt[i], eic->intensity[i]);
        }
    }
    data << d;
}

QPointF TinyPlot::mapToPlot(float x,float y ) {
    float xorigin = 0;
	float yorigin = _height;
	if (_maxXValue == 0 && _minXValue == 0 ) return QPointF(xorigin,yorigin);
	if (_maxYValue == 0 && _minYValue == 0 ) return QPointF(xorigin,yorigin);
	float px = xorigin+((x-_minXValue)/(_maxXValue-_minXValue))*_width;
	float py = yorigin-((y-_minYValue)/(_maxYValue-_minYValue))*_height;
	//qDebug() << x << " " << y << " " << px << " " << py << endl;
	return QPointF(px,py);
}

float TinyPlot::predictYValue(float fx) {
		return 0;
}
 

void TinyPlot::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{ 

    /**
     * Issue 84: Add Rt to plot
     * Note that it was easier to temporarily swap out the value of _height with an adjusted
     * value than re-writing everything to include an explicit axis value.
     * _height prior to x axis adjustment is not the full height, but rather, most of the height,
     * adjusted for a reserved area for the rt axis.
     */

    int oldHeight = _height;
    _height = _height - rt_axis_offset;

	if (_width <= 0 || _height <=0 ) return;
	float nSeries=data.size();
	_minXValue=_minYValue=FLT_MAX;
	_maxXValue=_maxYValue=FLT_MIN;

    if (data.size() == 0 ) return;

	//find bounds
    for(int i=0; i < nSeries; i++ ) {
		for(int j=0; j < data[i].size(); j++ ) {
            if ( data[i][j].y() > _maxYValue ) { _maxYValue=data[i][j].y(); }
            if ( data[i][j].y() < _minYValue ) { _minYValue=data[i][j].y(); }
            if ( data[i][j].x() > _maxXValue ) { _maxXValue=data[i][j].x(); }
            if ( data[i][j].x() < _minXValue ) { _minXValue=data[i][j].x(); }
		}
    }

    //Issue 84: Avoid possible RT assignment bug
    _minYValue = _minYValue * 0.8f;
    _maxYValue = _maxYValue * 1.2f;

    _minXValue = _minXValue * 0.8f;
    _maxXValue = _maxXValue * 1.2f;

    float maxPointIntensity=0;
    for(int i=0; i < points.size(); i++ ) {        
        if(points[i].y() > maxPointIntensity) { 
            maxPointIntensity=points[i].y();
        }
	}


	//qDebug() << QPointF(_minXValue,_maxXValue) << QPointF(_minYValue,_maxYValue);
    QColor blackColor = Qt::black;
    if (mw) {
        blackColor = mw->getBackgroundAdjustedBlack(mw->spectraWidget);
    }

	painter->setPen(Qt::gray);   
	painter->setBrush(Qt::NoBrush);
    painter->drawRect(QRectF(0,0,_width+2,_height+2));

    QPen shadowPen(blackColor);
    shadowPen.setWidth(2);
	painter->setPen(shadowPen);

    painter->drawLine(0,_height+2,_width+2,_height+2);
    painter->drawLine(_width+2, 2, _width+2, _height);


	//title
	if (!_title.isEmpty()) {
		setToolTip(_title);
        painter->setBrush(blackColor);
        painter->setPen(blackColor);
        float _fontH = ((float)_height)/10;

        if (_fontH > 15) { _fontH = 15; }
		if(_fontH > 3) {
			QFont fontSmall("Helvetica",_fontH);
			painter->setFont(fontSmall);
            painter->drawText(5,_fontH+1,_title);
		}
	}

    if (maxPointIntensity > 0) {
		setToolTip(_title);
        painter->setBrush(blackColor);
        painter->setPen(blackColor);
        float _fontH = ((float)_height)/10;

        if (_fontH > 15) { _fontH = 15; }
		if(_fontH > 3) {

            int prec;
            if (maxPointIntensity < 100) prec=1;
            else if (maxPointIntensity < 10) prec=2;
            else prec=0;

            QString rightText = QString::number(maxPointIntensity,'f',prec);
			QFont font("Helvetica",_fontH);
            QFontMetrics fm( font );
            int lagendShift = fm.size(0,rightText,0,NULL).width();
			painter->setFont(font);
            painter->drawText(_width-lagendShift-2,_fontH+1,rightText);
		}
	}


	int nColors = colors.size();
	if (nColors==0) colors << Qt::blue << Qt::red << Qt::yellow << Qt::green;

	for(int i=0; i < nSeries; i++ ) {
        painter->setBrush(blackColor);
		painter->setPen(colors[ i % nColors ] );

		int nPoints = data[i].size();
		QPolygonF path;
        if (nPoints >= 1 ) path << mapToPlot(data[i][0].x(),_minYValue);
		for(int j=0; j < nPoints; j++ ) {
            path << mapToPlot(data[i][j].x(),data[i][j].y());
		//	painter->drawEllipse(mapToPlot(data[i][j].x(), data[i][j].y()), 5,5);
		}
		//close path
		if (nPoints >= 1 ) path << mapToPlot(data[i][nPoints-1].x(),_minYValue);
		path << mapToPlot(_minXValue,_minYValue);

		painter->setBrush( colors[ i % nColors ] );
        if (_width > 20) painter->setPen(blackColor);
		painter->drawPolygon(path);
	}

    int pointSize = _height/30; if (pointSize<2) {pointSize=2;} if(pointSize>10) {pointSize=10;}
	for(int i=0; i < points.size(); i++ ) {        
		painter->setBrush(Qt::gray);
        painter->drawEllipse(mapToPlot(points[i].x(),points[i].y()),pointSize,pointSize);
	}

	if ( _currentXCoord != 0 ) {
		float x = _currentXCoord * (_maxXValue-_minXValue);
		painter->setBrush(Qt::red);
		painter->setPen(Qt::red);
		painter->drawLine(mapToPlot(x,_minYValue),mapToPlot(x,_maxYValue));
	}

    //rt axes
    //much of this is lifted from plot_axes.cpp

    _height = oldHeight;

    QPen pen(Qt::darkGray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);

    QFont font("Helvetica",12);
    font.setBold(true);
    painter->setFont(font);

    for (unsigned int i = 1; i <= 10; i++) {
        float x = ((i+1.0f)/12.0f) * _maxXValue;
        float px = ((x-_minXValue)/(_maxXValue-_minXValue))*_width;

        //tick mark
        painter->drawLine(px, _height-rt_axis_offset-5, px, _height-rt_axis_offset+5);

        //label
        painter->drawText(px, _height-rt_axis_offset+14, QString::number(x,'f',2));
    }}

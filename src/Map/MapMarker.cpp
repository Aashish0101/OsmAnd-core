#include "MapMarker.h"
#include "MapMarker_P.h"

OsmAnd::MapMarker::MapMarker(
    const int baseOrder_,
    const std::shared_ptr<const SkBitmap>& pinIcon_,
    const QHash< OnSurfaceIconKey, std::shared_ptr<const SkBitmap> >& onMapSurfaceIcons_,
    const FColorRGB precisionCircleBaseColor_)
    : _p(new MapMarker_P(this))
    , baseOrder(baseOrder_)
    , pinIcon(pinIcon_)
    , onMapSurfaceIcons(onMapSurfaceIcons_)
    , precisionCircleBaseColor(precisionCircleBaseColor_)
{
}

OsmAnd::MapMarker::~MapMarker()
{
}

bool OsmAnd::MapMarker::isHidden() const
{
    return _p->isHidden();
}

void OsmAnd::MapMarker::setIsHidden(const bool hidden)
{
    _p->setIsHidden(hidden);
}

bool OsmAnd::MapMarker::isPrecisionCircleEnabled() const
{
    return _p->isPrecisionCircleEnabled();
}

void OsmAnd::MapMarker::setIsPrecisionCircleEnabled(const bool enabled)
{
    _p->setIsPrecisionCircleEnabled(enabled);
}

double OsmAnd::MapMarker::getPrecisionCircleRadius() const
{
    return _p->getPrecisionCircleRadius();
}

void OsmAnd::MapMarker::setPrecisionCircleRadius(const double radius)
{
    _p->setPrecisionCircleRadius(radius);
}

OsmAnd::PointI OsmAnd::MapMarker::getPosition() const
{
    return _p->getPosition();
}

void OsmAnd::MapMarker::setPosition(const PointI position)
{
    _p->setPosition(position);
}

float OsmAnd::MapMarker::getOnMapSurfaceIconDirection(const OnSurfaceIconKey key) const
{
    return _p->getOnMapSurfaceIconDirection(key);
}

void OsmAnd::MapMarker::setOnMapSurfaceIconDirection(const OnSurfaceIconKey key, const float direction)
{
    _p->setOnMapSurfaceIconDirection(key, direction);
}

bool OsmAnd::MapMarker::hasUnappliedChanges() const
{
    return _p->hasUnappliedChanges();
}

bool OsmAnd::MapMarker::applyChanges()
{
    return _p->applyChanges();
}

std::shared_ptr<OsmAnd::MapSymbolsGroup> OsmAnd::MapMarker::createSymbolsGroup() const
{
    return _p->createSymbolsGroup();
}

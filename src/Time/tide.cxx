// tide.cxx -- interface for tidal movement
//
// Written by Erik Hofman, Octover 2020
//
// Copyright (C) 2020  Erik Hofman <erik@ehofman.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#include <simgear/constants.h>
#include <simgear/timing/sg_time.hxx>
#include <simgear/structure/SGExpression.hxx>
#include <Main/globals.hxx>

#include "tide.hxx"
#include "light.hxx"
#include "bodysolver.hxx"

void FGTide::reinit() {
    _prev_moon_lon = -9999.0;
}

void FGTide::bind()
{
    SGPropertyNode *props = globals->get_props();

    viewLon = props->getNode("sim/current-view/viewer-lon-deg", true);
    _tideAnimation = props->getNode("/environment/sea/surface/delta-T-tide", true);

    _tideLevelNorm = props->getNode("/sim/time/tide-level-norm", true);
    _tideLevelNorm->setDoubleValue(_tide_level);
}

void FGTide::unbind()
{
    viewLon.reset();
    _tideLevelNorm.reset();
    _tideAnimation.reset();
}

void FGTide::update(double dt)
{
    FGLight *l = static_cast<FGLight*>(globals->get_subsystem("lighting"));
    double moon_lon = l->get_moon_lon();
    if (fabs(_prev_moon_lon - moon_lon) > (SGD_PI/180.0))
    {
        _prev_moon_lon = moon_lon;

        double sun_lon = l->get_sun_lon();
        double viewer_lon = viewLon->getDoubleValue();

        _tide_level = cos(2.0*(moon_lon - viewer_lon));
        _tide_level += 0.1*cos(2.0*(sun_lon - viewer_lon));
        if (_tide_level < -1.0) _tide_level = -1.0;
        else if (_tide_level > 1.0) _tide_level = 1.0;

        _tideLevelNorm->setDoubleValue(_tide_level);
        _tideAnimation->setDoubleValue(0.5 - 0.5*_tide_level);
    }
}

// Register the subsystem.
SGSubsystemMgr::Registrant<FGTide> registrantFGTide;

// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "defines.h" // IWYU pragma: keep
#include "PostMsg.h"
#include "GamePlayerInfo.h"
#include <boost/format.hpp>

PostMsg::PostMsg(unsigned sendFrame, const std::string& text, PostMessageCategory cat, const MapPoint& pt /*= MapPoint::Invalid()*/):
    sendFrame_(sendFrame), text_(text), cat_(cat), pt_(pt)
{}

PostMsg::PostMsg(unsigned sendFrame, PactType pt, const GamePlayerInfo& otherPlayer, bool acceptedOrCanceled):
    sendFrame_(sendFrame), cat_(PMC_DIPLOMACY), pt_(MapPoint::Invalid())
{
    if(acceptedOrCanceled)
    {
        text_ = boost::str(
            boost::format(_("The %s between player '%s' and you has been concluded.")) % PACT_NAMES[pt] % otherPlayer.nation
        );
    } else
    {
        text_ = boost::str(
            boost::format(_("The %s between player '%s' and you has been cancelled.")) % PACT_NAMES[pt] % otherPlayer.nation
        );
    }
}

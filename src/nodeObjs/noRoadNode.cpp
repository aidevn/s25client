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
#include "noRoadNode.h"

#include "RoadSegment.h"
#include "GameClientPlayer.h"
#include "SerializedGameData.h"
#include "world/GameWorldGame.h"


noRoadNode::noRoadNode(const NodalObjectType nop, const MapPoint pos, const unsigned char player)
    : noCoordBase(nop, pos),
      player(player)
{
    for(unsigned i = 0; i < 6; ++i)
        routes[i] = 0;
    last_visit = 0;
}

noRoadNode::~noRoadNode()
{
}

void noRoadNode::Destroy_noRoadNode()
{
    DestroyAllRoads();

    Destroy_noCoordBase();
}

void noRoadNode::Serialize_noRoadNode(SerializedGameData& sgd) const
{
    Serialize_noCoordBase(sgd);

    sgd.PushUnsignedChar(player);

    // the trick only seems to work for flags
    if (this->GetGOT() == GOT_FLAG)
    {
        // this is a trick:
        // -> initialize routes for flag with NULL
        // -> RoadSegment will set these later
        for (unsigned i = 0; i < 6; ++i)
        {
            sgd.PushObject(static_cast<GameObject*>(NULL), true);
        }
    }
    else
    {
        for (unsigned i = 0; i < 6; ++i)
        {
            sgd.PushObject(routes[i], true);
        }
    }
}

noRoadNode::noRoadNode(SerializedGameData& sgd, const unsigned obj_id) : noCoordBase(sgd, obj_id),
    player(sgd.PopUnsignedChar())
{
    for (unsigned i = 0; i < 6; ++i)
    {
        routes[i] = sgd.PopObject<RoadSegment>(GOT_ROADSEGMENT);
    }

    last_visit = 0;
}

void noRoadNode::UpgradeRoad(const unsigned char dir)
{
    if(routes[dir])
        routes[dir]->UpgradeDonkeyRoad();
}

void noRoadNode::DestroyRoad(const unsigned char dir)
{
    if(routes[dir])
    {
        MapPoint t = routes[dir]->GetF1()->GetPos();
        for(unsigned z = 0; z < routes[dir]->GetLength(); ++z)
        {
            gwg->SetPointRoad(t, routes[dir]->GetRoute(z), 0);
            gwg->RecalcBQForRoad(t);
            t = gwg->GetNeighbour(t, routes[dir]->GetRoute(z));
        }

        noRoadNode* oflag;

        if(routes[dir]->GetF1() == this)
            oflag = routes[dir]->GetF2();
        else
            oflag = routes[dir]->GetF1();

        for(unsigned z = 0; z < 6; ++z)
        {
            if(oflag->routes[z] == routes[dir])
            {
                oflag->routes[z] = NULL;
                break;
            }else
                RTTR_Assert(z < 5); // Need to find it before last iteration
        }

        RoadSegment* tmp = routes[dir];
        routes[dir] = NULL;

        tmp->Destroy();
        delete tmp;

        // Spieler Bescheid sagen
        gwg->GetPlayer(player).RoadDestroyed();
    }
}

/// Vernichtet Alle Straße um diesen Knoten
void noRoadNode::DestroyAllRoads()
{
    // Alle Straßen um mich herum zerstören
    for(unsigned char i = 0; i < 6; ++i)
        DestroyRoad(i);
}

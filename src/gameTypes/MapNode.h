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

#ifndef MapNode_h__
#define MapNode_h__

#include "gameTypes/MapTypes.h"
#include "gameTypes/BuildingQuality.h"
#include "gameData/MaxPlayers.h"
#include <boost/array.hpp>
#include <list>

class FOWObject;
class noBase;
class SerializedGameData;

/// Eigenschaften von einem Punkt auf der Map
struct MapNode
{
    /// Roads
    boost::array<unsigned char, 3> roads;
    boost::array<bool, 3> roads_real;
    /// Height
    unsigned char altitude;
    /// Schattierung
    unsigned char shadow;
    /// Terrain (t1 is the triangle with the edge at the top exactly below this pt, t2 with the edge at the bottom on the right lower side of the pt)
    TerrainType t1, t2;
    /// Ressourcen
    unsigned char resources;
    /// Reservierungen
    bool reserved;
    /// Owner (playerIdx - 1)
    unsigned char owner;
    /// Grenzsteine (der Punkt, und dann jeweils nach rechts, unten-links und unten-rechts die Zwischensteine)
    typedef boost::array<unsigned char, 4> BoundaryStones;
    BoundaryStones boundary_stones;
    BuildingQuality bq, bqVisual;
    /// How a player sees the point in FoW
    struct FoWData
    {
        /// Zeit (GF-Zeitpunkt), zu der, der Punkt zuletzt aktualisiert wurde
        unsigned last_update_time;
        /// Sichtbarkeit des Punktes
        Visibility visibility;
        /// FOW-Objekt
        FOWObject* object;
        boost::array<unsigned char, 3> roads;
        /// Grenzsteine (der Punkt, und dann jeweils nach rechts, unten-links und unten-rechts die Zwischensteine)
        unsigned char owner;
        /// Grenzsteine (der Punkt, und dann jeweils nach rechts, unten-links und unten-rechts die Zwischensteine)
        boost::array<unsigned char, 4> boundary_stones;
        FoWData(): object(NULL){}
    };
    boost::array<FoWData, MAX_PLAYERS> fow;

    /// To which sea this belongs to (0=None)
    unsigned short sea_id;
    /// Hafenpunkt-ID (0 = kein Hafenpunkt)
    unsigned harbor_id;

    /// Objekt, welches sich dort befindet
    noBase* obj;
    /// Figures or fights on this node
    std::list<noBase*> figures;

    MapNode(): obj(NULL){}
    void Serialize(SerializedGameData& sgd, const unsigned numPlayers) const;
    void Deserialize(SerializedGameData& sgd, const unsigned numPlayers);
};

#endif // MapNode_h__


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

#ifndef GameWorldBase_h__
#define GameWorldBase_h__

#include "world/World.h"
#include "buildings/nobBaseMilitary.h"
#include "postSystem/PostManager.h"
#include "notifications/NotificationManager.h"
#include "helpers/Deleter.h"
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>

class RoadPathFinder;
class FreePathFinder;
class GameInterface;
class noBuildingSite;
class GameClientPlayerList;
class GlobalGameSettings;
class GameClientPlayer;
class nofPassiveSoldier;
class nobHarborBuilding;
class noFlag;
class noBase;
class LuaInterfaceGame;
class EventManager;

/// Grundlegende Klasse, die die Gamewelt darstellt, enth�lt nur deren Daten
class GameWorldBase: public World
{
    boost::interprocess::unique_ptr<RoadPathFinder, Deleter<RoadPathFinder> > roadPathFinder;
    boost::interprocess::unique_ptr<FreePathFinder, Deleter<FreePathFinder> > freePathFinder;
    PostManager postManager;
    NotificationManager notifications;

    GameClientPlayerList& players;
    const GlobalGameSettings& gameSettings;
    EventManager& em;
protected:
    boost::interprocess::unique_ptr<LuaInterfaceGame, Deleter<LuaInterfaceGame> > lua;
    /// Interface zum GUI
    GameInterface* gi;
    /// harbor building sites created by ships
    std::list<noBuildingSite*> harbor_building_sites_from_sea;

public:
    GameWorldBase(GameClientPlayerList& players, const GlobalGameSettings& gameSettings, EventManager& em);
    ~GameWorldBase() override;

    // Grundlegende Initialisierungen
    void Init(const unsigned short width, const unsigned short height, LandscapeType lt) override;

    /// Setzt GameInterface
    void SetGameInterface(GameInterface* const gi) { this->gi = gi; }

    /// Gibt Dynamische Objekte, die von einem bestimmten Punkt aus laufen oder dort stehen sowie andere Objekte,
    /// die sich dort befinden, zur�ck
    std::vector<noBase*> GetDynamicObjectsFrom(const MapPoint pt) const;

    /// Kann a node be used for a road (no flag/bld, no other road, no danger...)
    /// Should only be used for the points between the 2 flags of a road
    bool RoadAvailable(const bool boat_road, const MapPoint pt, const bool visual = true) const;
    /// Pr�ft ob exakt die gleiche Stra�e schon gebaut wurde
    bool RoadAlreadyBuilt(const bool boat_road, const MapPoint start, const std::vector<unsigned char>& route);

    /// Berechnet BQ bei einer gebauten Stra�e
    void RecalcBQForRoad(const MapPoint pt);
    /// Pr�ft, ob sich in unmittelbarer N�he (im Radius von 4) Milit�rgeb�ude befinden
    bool IsMilitaryBuildingNearNode(const MapPoint nPt, const unsigned char player) const;

    /// Test, ob auf dem besagten Punkt ein Milit�rgeb�ude steht
    bool IsMilitaryBuilding(const MapPoint pt) const;

    /// Erstellt eine Liste mit allen Milit�rgeb�uden in der Umgebung, radius bestimmt wie viele K�stchen nach einer Richtung im Umkreis
    sortedMilitaryBlds LookForMilitaryBuildings(const MapPoint pt, unsigned short radius) const;

    /// Pr�ft, ob von einem bestimmten Punkt aus der Untergrund f�r Figuren zug�nglich ist (kein Wasser,Lava,Sumpf)
    bool IsNodeToNodeForFigure(const MapPoint pt, const unsigned dir) const;

    /// Finds a path for figures. Returns 0xFF if none found
    unsigned char FindHumanPath(const MapPoint start, const MapPoint dest, const unsigned max_route = 0xFFFFFFFF, const bool random_route = false, unsigned* length = NULL) const;
    /// Find path for ships. Return true on success
    bool FindShipPath(const MapPoint start, const MapPoint dest, std::vector<unsigned char>* route, unsigned* length);
    RoadPathFinder& GetRoadPathFinder() const { return *roadPathFinder; }
    FreePathFinder& GetFreePathFinder() const { return *freePathFinder; }

    /// Baut eine (bisher noch visuell gebaute) Stra�e wieder zur�ck
    void RemoveVisualRoad(const MapPoint start, const std::vector<unsigned char>& route);

    /// x,y ist ein Punkt auf irgendeinem Wegstck, gibt die Flagge zur�ck
    noFlag* GetRoadFlag(MapPoint pt, unsigned char& dir, unsigned last_i = 255);

    /// Erzeugt eine GUI-ID f�r die Fenster von Map-Objekten
    unsigned CreateGUIID(const MapPoint pt) const { return 1000 + GetIdx(pt); }

    /// Gets the (height adjusted) global coordinates of the node (e.g. for drawing)
    Point<int> GetNodePos(const MapPoint pt) const;

    /// Ver�ndert die H�he eines Punktes und die damit verbundenen Schatten
    void AltitudeChanged(const MapPoint pt) override;

    /// Berechnet Bauqualitäten an Punkt x;y und den ersten Kreis darum neu
    void RecalcBQAroundPoint(const MapPoint pt);
    /// Berechnet Bauqualitäten wie bei letzterer Funktion, bloß noch den 2. Kreis um x;y herum
    void RecalcBQAroundPointBig(const MapPoint pt);

    /// Ermittelt Sichtbarkeit eines Punktes auch unter Einbeziehung der Verb�ndeten des jeweiligen Spielers
    Visibility CalcWithAllyVisiblity(const MapPoint pt, const unsigned char player) const;

    /// Ist es an dieser Stelle f�r einen Spieler m�glich einen Hafen zu bauen
    bool IsHarborPointFree(const unsigned harbor_id, const unsigned char player,
        const unsigned short sea_id) const;
    /// Ermittelt, ob ein Punkt K�stenpunkt ist, d.h. Zugang zu einem schiffbaren Meer, an dem auch mindestens 1 Hafenplatz liegt, hat
    /// und gibt ggf. die Meeres-ID zur�ck, ansonsten 0
    unsigned short IsCoastalPointToSeaWithHarbor(const MapPoint pt) const;
    /// Sucht freie Hafenpunkte, also wo noch ein Hafen gebaut werden kann
    unsigned GetNextFreeHarborPoint(const MapPoint pt, const unsigned origin_harbor_id, const unsigned char dir, const unsigned char player) const;
    /// Berechnet die Entfernung zwischen 2 Hafenpunkten
    unsigned CalcHarborDistance(const unsigned habor_id1, const unsigned harbor_id2) const;
    /// Bestimmt f�r einen beliebigen Punkt auf der Karte die Entfernung zum n�chsten Hafenpunkt
    unsigned CalcDistanceToNearestHarbor(const MapPoint pos) const;
    /// returns true when a harborpoint is in SEAATTACK_DISTANCE for figures!
    bool IsAHarborInSeaAttackDistance(const MapPoint pos) const;

    /// Return the player with the given index
    GameClientPlayer& GetPlayer(const unsigned id) const;
    unsigned GetPlayerCount() const;
    /// Return the game settings
    const GlobalGameSettings& GetGGS() const { return gameSettings; }
    EventManager& GetEvMgr(){ return em; }
    const EventManager& GetEvMgr() const { return em; }
    PostManager& GetPostMgr(){ return postManager; }
    const PostManager& GetPostMgr() const { return postManager; }
    NotificationManager& GetNotifications() const { return const_cast<GameWorldBase*>(this)->notifications; } // We want to be abled to add notifications even on a const world

    struct PotentialSeaAttacker
    {
        /// Comparator that compares only the soldier pointer
        struct CmpSoldier
        {
            nofPassiveSoldier* const search;
            CmpSoldier(nofPassiveSoldier* const search): search(search){}
            bool operator()(const PotentialSeaAttacker& other)
            {
                return other.soldier == search;
            }
        };
        /// Soldat, der als Angreifer in Frage kommt
        nofPassiveSoldier* soldier;
        /// Hafen, den der Soldat zuerst ansteuern soll
        nobHarborBuilding* harbor;
        /// Entfernung Hafen-Hafen (entscheidende)
        unsigned distance;

        PotentialSeaAttacker(nofPassiveSoldier* soldier, nobHarborBuilding* harbor, unsigned distance): soldier(soldier), harbor(harbor), distance(distance){}

        /// Komperator zum Sortieren
        bool operator<(const PotentialSeaAttacker& pa) const;
    };


    /// Liefert Hafenpunkte im Umkreis von einem bestimmten Milit�rgeb�ude
    std::vector<unsigned> GetHarborPointsAroundMilitaryBuilding(const MapPoint pt) const;
    /// returns all sea_ids from which a given building can be attacked by sea
    std::vector<unsigned> GetValidSeaIDsAroundMilitaryBuildingForAttack(const MapPoint pt, std::vector<bool>& use_seas, const unsigned char player_attacker)const;
    /// returns all sea_ids found in the given vector from which a given building can be attacked by sea
    void GetValidSeaIDsAroundMilitaryBuildingForAttackCompare(const MapPoint pt, std::vector<unsigned short>& use_seas, const unsigned char player_attacker)const;
    /// Sucht verf�gbare Soldaten, um dieses Milit�rgeb�ude mit einem Seeangriff anzugreifen
    std::vector<PotentialSeaAttacker> GetAvailableSoldiersForSeaAttack(const unsigned char player_attacker, const MapPoint pt) const;
    /// Gibt Anzahl oder gesch�tzte St�rke(rang summe + anzahl) der verf�gbaren Soldaten die zu einem Schiffsangriff starten k�nnen von einer bestimmten sea id aus
    unsigned int GetAvailableSoldiersForSeaAttackAtSea(const unsigned char player_attacker, unsigned short seaid, bool count = true) const;

    bool HasLua() const { return lua.get() != NULL; }
    LuaInterfaceGame& GetLua() const { return *lua.get(); }
protected:

    /// F�r abgeleitete Klasse, die dann das Terrain entsprechend neu generieren kann
    virtual void VisibilityChanged(const MapPoint pt) = 0;

    /// Gibt n�chsten Hafenpunkt in einer bestimmten Richtung zur�ck, bzw. 0, wenn es keinen gibt
    unsigned GetNextHarborPoint(const MapPoint pt, const unsigned origin_harbor_id, const unsigned char dir,
        const unsigned char player, bool (GameWorldBase::*IsPointOK)(const unsigned, const unsigned char, const unsigned short) const) const;

};

#endif // GameWorldBase_h__

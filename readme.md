![Logo-artwork](https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/arduwars-logo.png "Arduwars Logo")
### Story
There has been peace for many years in our lands until the evil forces of *Nextgenia* have invaded our country. Equipped with never before seen weapons their Army fearlessly takes over our lands.

*Cpt. Eidbit*, it is your mission to recover our lands and stop the great evil once for all. You are the commander of our army which you have lead to victory 🏁.

![Mission-Report-1](https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/Reports/mission-report-01-1337.png "Mission Report 1")

---

### Gameplay
Arduwars is a port of [Advance Wars](https://en.wikipedia.org/wiki/Advance_Wars) for the [Arduboy](https://arduboy.com/).
It is a turn based strategy game where the Players goal is to overtake the enemies Headquarters or destroy all his units.

Every turn, the Player commands his *Units* to capture *Buildings* or fight enemy Units. They key for winning is to use the enviroment strategically for your benefit. Certain terrain grant bonus for defense or movement. Buildings need to be captured to build more Units and get more income.

The Goal is to either capture the *Nextgenians* Headquarters or destroy all their units.
![controls](https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/readme-controls.png "Controls")


![Mission-Report-2](https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/Reports/mission-report-02-16.12.1991.png "Mission Report 2")

#### Battle system
Ever unit can be moved a certain distance. After a unit has been moved, it may attack or capture a building. Only infantry units can capture Buildings and not every unit can attack after it has moved.

Units that can attack, do so either directly (can attack adjacent to another unit where standing or moved, but be counter-attacked by the enemy unit), or in-directly (must remain still to attack, and have an enemy in firing range).

All unit traits are listed in the Units-section.

![Mission-Report-3](https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/Reports/mission-report-03-05.10.2015.png "Mission Report 3")
#### Terrain
Sprite  |  Name   |  Description
--|--|--
![terrain][terrain-plains]    |  Plains     |   Basic Terrain with no special effect.
![terrain][terrain-street]    |  Streets    |   Since movement is easier on paved Roads, Streets grant a small mobility bonus.
![terrain][terrain-forrest]   |  Forrests   |   Forrest grant a small defensive bonus but it is harder to move through them.
![terrain][terrain-hill]      |  Hills      |   Only Infantry Units can pass hills and they do that slowly. In return they provide a significant defense bonus.
![terrain][terrain-mountain]  |  Mountains  |   Only Infantry Units can pass Mountains and they do that very slowly. In return they provide a hughe defense bonus.
![building][building-city]    |  Buildings  |   Buildings are strategic locations in war. They grant different options. See the buildings section for more
![terrain][terrain-water]     |  Water      |   Water can only be accessed by Ships and Planes.
![terrain][terrain-reef]      |  Reefs      |   Reefs are a dangerous area for ships and must be avoided. information.

![Mission-Report-4](https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/Reports/mission-report-04-21.10.2015.png "Mission Report 4")

#### Buildings

Sprite  |  Name   |  Description
--|--|--
![building][building-city]      |  Cities                   |   Cities are the foundation of a nation. When occupied, they share a regular income of 1000 coins at the start of every round.
![building][building-factory]   |  Factories                |   Factories are needed to build new Land Units. Every occupied Factory can produce one Unit if there is no unit stationed above.
![building][building-airport]   |  Airports                 |   Airports are needed to build new Air Units. Every occupied Airport can produce one Unit if there is no unit stationed above.
![building][building-shipyard]  |  Shipyards                |   Shipyards are needed to build new Naval Units. Every occupied Shipyard can produce one Unit if there is no unit stationed above.
![building][building-lab]       |  Laboratories             |   There are numerous scientific Facilities located in our lands. When a Unit is Stationed they provide live satelite imagery which cancels the fog of war.
![building][building-hq1]       |  Players HQ               |   This is our Headquarters. Defend it by any means.
![building][building-hq2]       |  Nextgenias Headquarter   |   This are the Headquarters of the Nextgenians. If you can occupy this heavily fortified buildings you immediately win the battle.

![Mission-Report-5](https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/Reports/mission-report-05-04.07.1996.png "Mission Report 5")

#### Units
Sprite  |  Name   |  Description
--|--|--
![unit][unit-soldier]         |  Soldier        | Basic Infantry Unit. Weakest of all units but key for a strategic win. Only Infantry can occupy Buildings and move on Hills and Mountains.
![unit][unit-mech]            |  Mech           | Potent Infantry Unit which deal good damage to armored Units.
![unit][others-questionsmark] |  ???            | Every Army has secret and well trained Special-Operations Unit.
![unit][unit-scout]           |  Scout          | A light vehicle with high mobility.
![unit][unit-support]         |  Support        | This vehicle can transport Infantry Units and restores 1 Healthpoint of any adjacent Unit at the start of the turn. This Unit can not attack.
![unit][unit-tank]            |  Tank           | Powerful armored vehicle with good mobility.
![unit][others-questionsmark] |  ???            | Our engineers have created a powerful Unit for desperate times.
![unit][unit-artillery]       |  Artillery      | Long range unit that can deal small damage. May not move and attack in the same round.
![unit][unit-rocket]          |  Rocket         | Long range unit that can deal medium damage to Air and Marine Units. May not move and attack in the same round.
![unit][others-questionsmark] |  ???            | What may this weapon be?
![unit][unit-heli]            |  Helicopter     | Basic Air Unit. Can attack all other classes but only deals a small amount of damage.
![unit][unit-fighter]         |  Fighter        | Powerful Air Unit which can only attack other Air Units.
![unit][others-questionsmark] |  ???            | They call it **Fear of the Skies**.
![unit][unit-transportship]   |  Transportship  | A Ship that can load Infantry Units and move them over long distances in water. This Unit can not attack.
![unit][unit-cruiser]         |  Cruiser        | This well armed Ship can destroy all other ships easily but cannot attack anything else.
![unit][others-questionsmark] |  ???            | One of our most powerful weapons far outside on the sea.

![Mission-Report-6](https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/Reports/mission-report-06-05.04.2063.png "Mission Report 6")

[//]: # (image references here)
[terrain-forrest]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/terrain-forrest.png "terrain-forrest"
[terrain-hill]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/terrain-hill.png "terrain-hill"
[terrain-mountain]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/terrain-mountain.png "terrain-mountain"
[terrain-plains]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/terrain-plains.png "terrain-plains"
[terrain-reef]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/terrain-reef.png "terrain-reef"
[terrain-street]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/terrain-street.png "terrain-street"
[terrain-water]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/terrain-water.gif "terrain-water"

[building-airport]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/building-airport.png "building-airport"
[building-city]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/building-city.png "building-city"
[building-factory]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/building-factory.png "building-factory"
[building-hq1]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/building-headquarter1.png "building-hq1"
[building-hq2]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/building-headquarter2.png "building-hq2"
[building-lab]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/building-lab.png "building-lab"
[building-shipyard]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/terrainAndBuildings/building-shipyard.png "building-shipyard"


[//]: # (Unit image references here)
[unit-artillery]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-artillery.gif "unit-artillery"
[unit-battleship]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-battleship.gif "unit-battleship"
[unit-bigtank]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-bigtank.gif "unit-bigtank"
[unit-bomber]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-bomber.gif "unit-bomber"
[unit-cruiser]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-cruiser.gif "unit-cruiser"
[unit-fighter]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-fighter.gif "unit-fighter"
[unit-heli]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-heli.gif "unit-heli"
[unit-mech]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-mech.gif "unit-mech"
[unit-missiles]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-missiles.gif "unit-missiles"
[unit-rocket]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-rocket.gif "unit-rocket"
[unit-scout]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-scout.gif "unit-scout"
[unit-soldier]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-soldier.gif "unit-soldier"
[unit-specops]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-specops.gif "unit-specops"
[unit-support]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-support.gif "unit-support"
[unit-tank]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-tank.gif "unit-tank"
[unit-transportship]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/unitgifs/unit-transportship.gif "unit-transportship"
[others-questionsmark]: https://github.com/yinkou/Arduwars/blob/master/Assets/ReadmeAssets/questionsmarks.gif "others-questionsmark"

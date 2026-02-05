#pragma once

static constexpr auto plan = R"PLAN(
{
    "fileType": "Plan",
    "geoFence": {
        "circles": [
        ],
        "polygons": [
        ],
        "version": 2
    },
    "groundStation": "QGroundControl",
    "mission": {
        "cruiseSpeed": 15,
        "firmwareType": 12,
        "globalPlanAltitudeMode": 1,
        "hoverSpeed": 5,
        "items": [
            {
                "AMSLAltAboveTerrain": null,
                "Altitude": 20,
                "AltitudeMode": 1,
                "autoContinue": true,
                "command": 22,
                "doJumpId": 1,
                "frame": 3,
                "params": [
                    0,
                    0,
                    0,
                    null,
                    47.3977418,
                    8.545593799999999,
                    20
                ],
                "type": "SimpleItem"
            },
            {
                "AMSLAltAboveTerrain": null,
                "Altitude": 20,
                "AltitudeMode": 1,
                "autoContinue": true,
                "command": 16,
                "doJumpId": 2,
                "frame": 3,
                "params": [
                    0,
                    0,
                    0,
                    null,
                    47.398548,
                    8.5458834,
                    20
                ],
                "type": "SimpleItem"
            },
            {
                "AMSLAltAboveTerrain": null,
                "Altitude": 20,
                "AltitudeMode": 1,
                "autoContinue": true,
                "command": 16,
                "doJumpId": 3,
                "frame": 3,
                "params": [
                    0,
                    0,
                    0,
                    null,
                    47.3988022,
                    8.5447891,
                    20
                ],
                "type": "SimpleItem"
            },
            {
                "AMSLAltAboveTerrain": null,
                "Altitude": 20,
                "AltitudeMode": 1,
                "autoContinue": true,
                "command": 16,
                "doJumpId": 4,
                "frame": 3,
                "params": [
                    0,
                    0,
                    0,
                    null,
                    47.398243,
                    8.5445209,
                    20
                ],
                "type": "SimpleItem"
            },
            {
                "AMSLAltAboveTerrain": null,
                "Altitude": 20,
                "AltitudeMode": 1,
                "autoContinue": true,
                "command": 16,
                "doJumpId": 5,
                "frame": 3,
                "params": [
                    0,
                    0,
                    0,
                    null,
                    47.398068699999996,
                    8.545593799999999,
                    20
                ],
                "type": "SimpleItem"
            },
            {
                "autoContinue": true,
                "command": 20,
                "doJumpId": 6,
                "frame": 2,
                "params": [
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0
                ],
                "type": "SimpleItem"
            }
        ],
        "plannedHomePosition": [
            47.3977418,
            8.5455938,
            488.78478462718084
        ],
        "vehicleType": 2,
        "version": 2
    },
    "rallyPoints": {
        "points": [
        ],
        "version": 2
    },
    "version": 1
}
)PLAN";

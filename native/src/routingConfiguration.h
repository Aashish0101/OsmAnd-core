#ifndef _OSMAND_ROUTING_CONFIGURATION_H
#define _OSMAND_ROUTING_CONFIGURATION_H
#include "CommonCollections.h"
#include "commonOsmAndCore.h"
#include "generalRouter.h"
#include <algorithm>

struct RoutingRule {
    string tagName;
    string t;
    string v;
    string param;
    string value1;
    string value2;
    string type;
};

struct RoutingConfiguration {

    const static int DEFAULT_MEMORY_LIMIT = 30;
    const static int DEVIATION_RADIUS = 3000;
    MAP_STR_STR attributes;

    SHARED_PTR<GeneralRouter> router;

	int memoryLimitation;
	float initialDirection;

	int zoomToLoad;
	float heurCoefficient;
	int planRoadDirection;
	string routerName;
	
    // 1.5 Recalculate distance help
    float recalculateDistance;

    RoutingConfiguration(float initDirection = -360, int memLimit = 64) : router(new GeneralRouter()), memoryLimitation(memLimit), initialDirection(initDirection), zoomToLoad(16), heurCoefficient(1), planRoadDirection(0), routerName(""), recalculateDistance(20000.0f) {
    }

    string getAttribute(SHARED_PTR<GeneralRouter> router, string propertyName) {
        if (router->containsAttribute(propertyName)) {
            return router->getAttribute(propertyName);
        }
        return attributes[propertyName];
    }

    void initParams() {
		planRoadDirection = (int) parseFloat(getAttribute(router, "planRoadDirection"), 0);
		heurCoefficient = parseFloat(getAttribute(router, "heuristicCoefficient"), 1);
        recalculateDistance = parseFloat(getAttribute(router, "recalculateDistanceHelp"), 20000);
		// don't use file limitations?
		memoryLimitation = (int)parseFloat(getAttribute(router, "nativeMemoryLimitInMB"), memoryLimitation);
		zoomToLoad = (int)parseFloat(getAttribute(router, "zoomToLoadTiles"), 16);
		//routerName = parseString(getAttribute(router, "name"), "default");
	}
};

class RoutingConfigurationBuilder {
private:
    UNORDERED(map)<string, SHARED_PTR<GeneralRouter> > routers;
    MAP_STR_STR attributes;
    UNORDERED(map)<int64_t, int_pair> impassableRoadLocations;
    std::vector<SHARED_PTR<RouteDataObject> > impassableRoads;

public:
    string defaultRouter;

    RoutingConfigurationBuilder() : defaultRouter("") {
    }
    
    SHARED_PTR<RoutingConfiguration> build(string router, int memoryLimitMB, const MAP_STR_STR& params = MAP_STR_STR()) {
        return build(router, -360, memoryLimitMB, params);
    }
    
    SHARED_PTR<RoutingConfiguration> build(string router, float direction, int memoryLimitMB, const MAP_STR_STR& params = MAP_STR_STR()) {
        if (routers.find(router) == routers.end()) {
            router = defaultRouter;
        }
        SHARED_PTR<RoutingConfiguration> i = SHARED_PTR<RoutingConfiguration>(new RoutingConfiguration());
        if (routers.find(router) != routers.end()) {
            i->router = routers[router];
            i->router = i->router->build(params);
            i->routerName = router;
        }
        attributes["routerName"] = router;
        i->attributes.insert(attributes.begin(), attributes.end());
        i->initialDirection = direction;
        i->memoryLimitation = memoryLimitMB;
        i->initParams();
        
        auto it = impassableRoadLocations.begin();
        for(;it != impassableRoadLocations.end(); it++) {
            i->router->impassableRoadIds.insert(it->first);
        }

        return i;
    }
    
    std::vector<SHARED_PTR<RouteDataObject> >& getImpassableRoads() {
        return impassableRoads;
    }
    
    UNORDERED(map)<int64_t, int_pair>& getImpassableRoadLocations() {
        return impassableRoadLocations;
    }
    
    bool addImpassableRoad(SHARED_PTR<RouteDataObject> route, int x31, int y31) {
        if (impassableRoadLocations.find(route->id) == impassableRoadLocations.end()) {
            impassableRoadLocations[route->id] = int_pair(x31, y31);
            impassableRoads.push_back(route);
            return true;
        }
        return false;
    }
    
    SHARED_PTR<GeneralRouter> getRouter(string applicationMode) {
        return routers[applicationMode];
    }
    
    void addRouter(string name, SHARED_PTR<GeneralRouter> router) {
        routers[name] = router;
    }

    void addAttribute(string name, string value) {
        attributes[name] = value;
    }

    void removeImpassableRoad(SHARED_PTR<RouteDataObject> obj) {
        impassableRoadLocations.erase(obj->id);
        impassableRoads.erase(std::remove(impassableRoads.begin(), impassableRoads.end(), obj), impassableRoads.end());
    }
};

SHARED_PTR<RoutingConfigurationBuilder> parseRoutingConfigurationFromXml(const char* filename);

#endif /*_OSMAND_ROUTING_CONFIGURATION_H*/
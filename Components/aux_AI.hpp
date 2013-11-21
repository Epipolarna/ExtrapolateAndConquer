#ifndef AUX_AI_HPP
#define AUX_AI_HPP

#include <cmath>
#include <string>
#include <list>
#include <map>

/* AI auxilirary data containers
 * ---------------------------------------------------------------
 */

/*!
 *  \brief      The ObjectObservation struct
 *  \details    Data container of an object observation.
 */
struct ObjectObservation {
    QPoint position;    // Currently global position of object
    std::string type;   // Tree, water, br0nk,...
    int descriptor;     // Currently object ID (entity)
};

/*!
 *  \brief      The ObjectsInSight class
 *  \details    Provides access to various selections of seen objects sorted on distance, with the closest object first.
 */
struct ObjectsInSight {
public:
    std::list<ObjectObservation> & getSeenObjects() {
        return seenObjects;
    }
    std::list<ObjectObservation> getSeenObjects(std::string type) {
        return seenObjectsByType[type];
    }
    std::list<ObjectObservation> getSeenObjects(int descriptor, float allowedError = 0) {
        std::list<ObjectObservation> acceptedObjects;
        for(ObjectObservation & object : seenObjects)
            if(std::fabs(object.descriptor - descriptor) <= allowedError)
                acceptedObjects.push_back(object);
        return acceptedObjects;
    }

private:
    std::list<ObjectObservation> seenObjects;
    std::map<std::string, std::list<ObjectObservation>> seenObjectsByType;
};

#endif

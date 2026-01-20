#pragma once
#include "Instance.h"
#include "../datatypes/Vector3.h"

/**
 * @class PVInstance
 * @brief Adds position and rotation capabilities without physics
 *
 * @description
 * PVInstance introduces 3D transform properties (Position, Rotation, Scale)
 * to any Instance, without adding physics or collision logic.
 *
 * @inherits Instance
 */
struct PVInstance : public Instance {
    //-- Properties --//

    /**
     * @property Position
     * @type Vector3
     * @default Vector3{0,0,0}
     * @description Local position relative to parent
     */
    Vector3 Position = Vector3{0, 0, 0};

    /**
     * @property Rotation
     * @type Vector3
     * @default Vector3{0,0,0}
     * @description Local rotation in degrees
     */
    Vector3 Rotation = Vector3{0, 0, 0};

    //-- Methods --//

    PVInstance(const std::string &className = "PVInstance")
        : Instance(className) {}

    virtual ~PVInstance() = default;

    /**
     * @method GetWorldPosition
     * @returns Vector3
     * @description Computes the world position based on parent hierarchy
     */
    Vector3 GetWorldPosition() const;

    /**
     * @method GetWorldRotation
     * @returns Vector3
     * @description Computes the world rotation based on parent hierarchy
     */
    Vector3 GetWorldRotation() const;

    virtual bool IsA(const std::string &className) const override {
        return className == "PVInstance" || Instance::IsA(className);
    }
};

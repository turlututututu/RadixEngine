#include "PortalSystem.hpp"
#include <vector>
#include <engine/component/Player.hpp>
#include <engine/component/Transform.hpp>
#include <Portal.hpp>
#include <PlayerMotion.hpp>
#include "../physics/PhysicsHelper.hpp"

namespace glPortal {

void PortalSystem::setScene(Scene *scene) {
  this->scene = scene;
}

PortalSystem::PortalSystem() :
  scene(nullptr) {
}

PortalSystem::~PortalSystem() {
}

void PortalSystem::update(float dtime) {
  (void) dtime;
  for (Entity &e : scene->entities) {
    if (e.hasComponent<Player>()) {
      Transform &t = e.getComponent<Transform>();
      const Vector3f &pos = t.getPosition();
      Player &aplr = e.getComponent<Player>();
      PlayerMotion &pMotion = e.getComponent<PlayerMotion>();
      for (const EntityPair &pp : scene->portalPairs) {
        btVector3 min, max;
        Portal &pA = pp.first->getComponent<Portal>(),
               &pB = pp.second->getComponent<Portal>();

        // No uncollider? No portal link.
        if (!pA.uncollider || !pB.uncollider) {
          continue;
        }

        pA.uncolliderShape->getAabb(
          pA.uncollider->getWorldTransform(),
          min, max);
        // Is player within portal A uncollider (AABB then real check) ?
        if (PhysicsHelper::pointInAABB(pos, min, max) &&
            PhysicsHelper::pointInVolume(pos, *pA.uncollider)) {
          // Behind portal A
          const Transform
            &pAT = pp.first->getComponent<Transform>(),
            &pBT = pp.second->getComponent<Transform>();
          Vector3f normalA = pA.getDirection();
          Vector3f AP = pos - pAT.getPosition();
          if (dot(AP, normalA) < 0) {
            double APl = AP.length();
            Quaternion ad = pAT.getOrientation()*conjugate(pBT.getOrientation());
            t.setPosition(pBT.getPosition() + (ad*AP)*APl);
            pMotion.headAngle = (ad*pMotion.getBaseHeadOrientation()).toAero();
          }
        }
        pB.uncolliderShape->getAabb(
          pB.uncollider->getWorldTransform(),
          min, max);
        // Is player within portal B uncollider ?
        if (PhysicsHelper::pointInAABB(pos, min, max) &&
            PhysicsHelper::pointInVolume(pos, *pB.uncollider)) {
          // Behind portal B
          const Transform
            &pAT = pp.first->getComponent<Transform>(),
            &pBT = pp.second->getComponent<Transform>();
          Vector3f normalB = pB.getDirection();
          Vector3f BP = pos - pBT.getPosition();
          if (dot(BP, normalB) < 0) {
            double BPl = BP.length();
            Quaternion ad = pBT.getOrientation()*conjugate(pAT.getOrientation());
            t.setPosition(pAT.getPosition() + (ad*BP)*BPl);
            pMotion.headAngle = (ad*pMotion.getBaseHeadOrientation()).toAero();
          }
        }
      }
    }
  }
}

} /* namespace glPortal */

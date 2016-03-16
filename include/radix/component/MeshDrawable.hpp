#ifndef RADIX_COMPONENT_MESHDRAWABLE_HPP
#define RADIX_COMPONENT_MESHDRAWABLE_HPP

#include <radix/component/Component.hpp>
#include <radix/core/math/Vector3f.hpp>
#include <radix/model/Mesh.hpp>
#include <radix/material/Material.hpp>

namespace radix {

class MeshDrawable : public Component {
public:
  Mesh mesh;
  // TODO: MaterialDrawable?
  Material material;

  MeshDrawable(Entity &ent) :
    Component(ent) {}

  const char* getName() const {
    return "MeshDrawable";
  }

  TypeId getTypeId() const {
    return Component::getTypeId<std::remove_reference<decltype(*this)>::type>();
  }

  void serialize(serine::Archiver &ar) {
    /// @todo MeshDrawable serialization
  }
};

} /* namespace radix */

#endif /* RADIX_COMPONENT_MESHDRAWABLE_HPP */

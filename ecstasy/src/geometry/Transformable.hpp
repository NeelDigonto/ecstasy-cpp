#pragma once
#include <common/common.hpp>
#include <utils/Entity.h>
#include <utils/EntityManager.h>
#include <Eigen/Dense>

namespace ecstasy {
class Transformable {
  private:
    utils::EntityManager& entity_manager_;
    filament::TransformManager& transform_manager_;
    utils::Entity entity_;
    std::optional<utils::Entity> parent_;
    filament::TransformManager::Instance transform_instance_;
    Eigen::Vector3f translation_ = Eigen::Vector3f{0., 0., 0.};
    Eigen::Vector3f rotation_ = Eigen::Vector3f{0., 0., 0.};
    Eigen::Vector3f scale_ = Eigen::Vector3f{1., 1., 1.};

  public:
    Transformable(filament::TransformManager& _transform_manager)
        : entity_manager_{utils::EntityManager::get()}, transform_manager_{_transform_manager} {
        entity_ = entity_manager_.create();
        transform_manager_.create(entity_);
        transform_instance_ = transform_manager_.getInstance(entity_);
    };
    utils::Entity getEntity() { return entity_; }
    filament::TransformManager::Instance getTransformInstace() { return transform_instance_; }

    void setParent(filament::TransformManager::Instance _parent) {
        transform_manager_.setParent(transform_instance_, _parent);
    }
    void setTranslation(Eigen::Vector3f _translation) { translation_ = _translation; }
    void setRotation(Eigen::Vector3f _rotation) { rotation_ = _rotation; }
    void setScale(Eigen::Vector3f _scale) { scale_ = _scale; }
    void computeTransform() {
        Eigen::Transform<float, 3, Eigen::Affine> t;
        t = Eigen::Translation<float, 3>(translation_);
        t.rotate(Eigen::AngleAxisf(rotation_.x(), Eigen::Vector3f::UnitX()));
        t.rotate(Eigen::AngleAxisf(rotation_.y(), Eigen::Vector3f::UnitY()));
        t.rotate(Eigen::AngleAxisf(rotation_.z(), Eigen::Vector3f::UnitZ()));
        t.scale(scale_);

        auto raw_transform = t.matrix();
        filament::math::mat4f local_transform(*reinterpret_cast<filament::math::mat4f*>(&raw_transform));
        transform_manager_.setTransform(transform_instance_, local_transform);
    }
};
} // namespace ecstasy
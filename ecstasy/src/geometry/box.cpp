#include <geometry/box.hpp>
#include <common/utils.hpp>

ecstasy::cube::cube(filament::Engine* _engine, Eigen::Vector3d _dimention, Eigen::Vector3d _segments) {
    filament_engine_ = _engine;
    vertices_ = {
        Eigen::Vector3f{-1, -1, 1},  // 0. left bottom far
        Eigen::Vector3f{1, -1, 1},   // 1. right bottom far
        Eigen::Vector3f{-1, 1, 1},   // 2. left top far
        Eigen::Vector3f{1, 1, 1},    // 3. right top far
        Eigen::Vector3f{-1, -1, -1}, // 4. left bottom near
        Eigen::Vector3f{1, -1, -1},  // 5. right bottom near
        Eigen::Vector3f{-1, 1, -1},  // 6. left top near
        Eigen::Vector3f{1, 1, -1}    // 7. right top near
    };

    normals_ = {
        getFloat4FromEuler({0.0f, 0.0f, -1.0f}), getFloat4FromEuler({0.0f, 0.0f, -1.0f}),
        getFloat4FromEuler({0.0f, 0.0f, -1.0f}), getFloat4FromEuler({0.0f, 0.0f, -1.0f}),
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}),  getFloat4FromEuler({0.0f, 0.0f, 1.0f}),
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}),  getFloat4FromEuler({0.0f, 0.0f, 1.0f}),
    };

    uvs_ = {Eigen::Vector2f{}};

    indices_ = {
        2, 0, 1, 2, 1, 3, // far
        6, 4, 5, 6, 5, 7, // near
        2, 0, 4, 2, 4, 6, // left
        3, 1, 5, 3, 5, 7, // right
        0, 4, 5, 0, 5, 1, // bottom
        2, 6, 7, 2, 7, 3, // top
    };

    /*   vertices_.reserve(0U);
      normals_.reserve(0U);
      uvs_.reserve(0U);
      indices_.reserve(0U);

      for (size_t i = 0; i != _segments.x(); ++i) {
          for (size_t j = 0; j != _segments.y(); ++j) {
              for (size_t k = 0; k != _segments.z(); ++k) {

              }
          }
      } */
}

void ecstasy::cube::build_plane() {

    // Build Front Plane
}

std::pair<Eigen::Vector3d, Eigen::Vector3d> ecstasy::cube::getBoundingBox() { return {}; }
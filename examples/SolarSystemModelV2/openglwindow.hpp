#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string>
#include <string_view>

#include "abcg.hpp"
#include "camera.hpp"
#include "model.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  struct Astro {
    Model m_model;
    int m_trianglesToDraw{};
    glm::mat4 m_modelMatrix{1.0f};
  };

  // Properties of the astros
  struct Property {
    glm::vec3 position;     // Radius of the orbit
    glm::vec3 orientation;  // Orientation adjustment of the object
    float revolution;       // Velocity of the rotation around the sun
    float scale;            // Scale of the model
  };

  Astro astros[11];
  std::string names[11][2] = {{"sun.obj", "sun_texture.jpg"},
                              {"mercury.obj", "mercury_texture.jpg"},
                              {"venus.obj", "venus_texture.jpg"},
                              {"earth.obj", "earth_texture.png"},
                              {"moon.obj", "moon_texture.png"},
                              {"mars.obj", "mars_texture.jpg"},
                              {"jupiter.obj", "jupiter_texture.jpg"},
                              {"saturn.obj", "saturn_texture.jpg"},
                              {"uranus.obj", "uranus_texture.jpg"},
                              {"neptune.obj", "neptune_texture.jpg"},
                              {"pluto.obj", "pluto_texture.jpg"}};

  Property properties[11] = {
      {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
       0.2f},  // sun
      {glm::vec3(0.18f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0479f,
       0.02f},  // mercury
      {glm::vec3(0.25f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.035f,
       0.035},  // venus
      {glm::vec3(0.36f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0298f,
       0.04f},  // earth
      {glm::vec3(0.040f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0596f,
       0.013f},  // moon
      {glm::vec3(0.475f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0241f,
       0.035f},  // mars
      {glm::vec3(0.60f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0131f,
       0.1f},  // jupiter
      {glm::vec3(0.78f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0097f,
       0.11f},  // saturn
      {glm::vec3(0.95f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0068f,
       0.07f},  // uranus
      {glm::vec3(1.05f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0054f,
       0.045f},  // neptune
      {glm::vec3(1.15f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0047f,
       0.015f}  // pluto
  };

  // Camera variables
  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_pedestalSpeed{0.0f};
  float m_panSpeed{0.0f};
  float m_tiltSpeed{0.0f};

  // Control variables
  int m_viewportWidth{};
  int m_viewportHeight{};
  bool pause = false;
  int count = 1;
  float simulation_velocity = 1;

  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh
  int m_mappingMode{};

  // Light and material properties
  glm::vec4 m_lightDir{0.5f, 0.0f, 0.0f, 0.0f};
  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f, 0.861f, 0.591f, 1.0f};
  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  float m_shininess{};

  GLuint m_program;

  // Skybox
  GLuint m_skyVAO{};
  GLuint m_skyVBO{};
  GLuint m_skyProgram{};
  GLuint m_skyTexture;

  // clang-format off
  const std::array<glm::vec3, 36>  m_skyPositions{
    // Front
    glm::vec3{-3, -3, +3}, glm::vec3{+3, -3, +3}, glm::vec3{+3, +3, +3},
    glm::vec3{-3, -3, +3}, glm::vec3{+3, +3, +3}, glm::vec3{-3, +3, +3},
    // Back
    glm::vec3{+3, -3, -3}, glm::vec3{-3, -3, -3}, glm::vec3{-3, +3, -3},
    glm::vec3{+3, -3, -3}, glm::vec3{-3, +3, -3}, glm::vec3{+3, +3, -3},
    // Right
    glm::vec3{+3, -3, -3}, glm::vec3{+3, +3, -3}, glm::vec3{+3, +3, +3},
    glm::vec3{+3, -3, -3}, glm::vec3{+3, +3, +3}, glm::vec3{+3, -3, +3},
    // Left
    glm::vec3{-3, -3, +3}, glm::vec3{-3, +3, +3}, glm::vec3{-3, +3, -3},
    glm::vec3{-3, -3, +3}, glm::vec3{-3, +3, -3}, glm::vec3{-3, -3, -3},
    // Top
    glm::vec3{-3, +3, +3}, glm::vec3{+3, +3, +3}, glm::vec3{+3, +3, -3},
    glm::vec3{-3, +3, +3}, glm::vec3{+3, +3, -3}, glm::vec3{-3, +3, -3},
    // Bottom
    glm::vec3{-3, -3, -3}, glm::vec3{+3, -3, -3}, glm::vec3{+3, -3, +3},
    glm::vec3{-3, -3, -3}, glm::vec3{+3, -3, +3}, glm::vec3{-3, -3, +3}
  };


  void initializeSkybox();
  void renderSkybox();
  void terminateSkybox();
  void loadModel();
  void update();
};

#endif
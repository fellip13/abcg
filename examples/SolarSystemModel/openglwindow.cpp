#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    const std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};
}  // namespace std

void OpenGLWindow::handleEvent(SDL_Event& ev) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_KP_8 || ev.key.keysym.sym == SDLK_8)
      m_dollySpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_KP_2 || ev.key.keysym.sym == SDLK_2)
      m_dollySpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_KP_4 || ev.key.keysym.sym == SDLK_4)
      m_truckSpeed = -0.3f;
    if (ev.key.keysym.sym == SDLK_KP_6 || ev.key.keysym.sym == SDLK_6)
      m_truckSpeed = 0.3f;
    if (ev.key.keysym.sym == SDLK_DOWN) m_pedestalSpeed = -0.3f;
    if (ev.key.keysym.sym == SDLK_UP) m_pedestalSpeed = 0.3f;
    if (ev.key.keysym.sym == SDLK_a) m_panSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_d) m_panSpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_s) m_tiltSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_w) m_tiltSpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_p) pause = !pause;
    if (ev.key.keysym.sym == SDLK_r) count = 1;
  }
  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_KP_8 || ev.key.keysym.sym == SDLK_8) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_KP_2 || ev.key.keysym.sym == SDLK_2) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_KP_4 || ev.key.keysym.sym == SDLK_4) &&
        m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_KP_6 || ev.key.keysym.sym == SDLK_6) &&
        m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_DOWN) && m_pedestalSpeed < 0)
      m_pedestalSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_UP) && m_pedestalSpeed > 0)
      m_pedestalSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_a) && m_panSpeed < 0) m_panSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_d) && m_panSpeed > 0) m_panSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_s) && m_tiltSpeed < 0) m_tiltSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_w) && m_tiltSpeed > 0) m_tiltSpeed = 0.0f;
  }
}

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "SSM.vert",
                                    getAssetsPath() + "SSM.frag");

  // Load model
  m_sun_model.loadObj(getAssetsPath() + "sun.obj", false);
  m_earth_model.loadObj(getAssetsPath() + "earth.obj", false);
  m_mercurio_model.loadObj(getAssetsPath() + "mercurio.obj", false);
  m_venus_model.loadObj(getAssetsPath() + "venus.obj", false);
  m_mars_model.loadObj(getAssetsPath() + "mars.obj", false);
  m_jupiter_model.loadObj(getAssetsPath() + "jupiter.obj", false);
  m_neptuno_model.loadObj(getAssetsPath() + "neptuno.obj", false);
  m_saturn_model.loadObj(getAssetsPath() + "saturn.obj", false);
  m_uranus_model.loadObj(getAssetsPath() + "uranus.obj", false);
  m_pluto_model.loadObj(getAssetsPath() + "pluto.obj", false);
  m_moon_model.loadObj(getAssetsPath() + "moon.obj", false);

  m_sun_model.setupVAO(m_program);
  m_earth_model.setupVAO(m_program);
  m_mercurio_model.setupVAO(m_program);
  m_venus_model.setupVAO(m_program);
  m_mars_model.setupVAO(m_program);
  m_jupiter_model.setupVAO(m_program);
  m_neptuno_model.setupVAO(m_program);
  m_saturn_model.setupVAO(m_program);
  m_uranus_model.setupVAO(m_program);
  m_pluto_model.setupVAO(m_program);
  m_moon_model.setupVAO(m_program);

  resizeGL(getWindowSettings().width, getWindowSettings().height);
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);

  abcg::glBindVertexArray(m_VAO);

  // Sun
  glm::mat4 sun_model{1.0f};
  sun_model = glm::translate(sun_model, glm::vec3(0.0f, 0.0f, 0.0f));
  sun_model =
      glm::rotate(sun_model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  sun_model = glm::scale(sun_model, glm::vec3(0.005f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &sun_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.976f, 0.733f, 0.184f, 1.0f);
  m_sun_model.render(-1);

  // Mercury
  glm::mat4 mercurio_model{1.0f};
  mercurio_model = glm::rotate(mercurio_model, glm::radians(0.0479f * count),
                               glm::vec3(0, 1, 0));
  mercurio_model =
      glm::translate(mercurio_model, glm::vec3(150.0f * 0.001f, 0.0f, 0.0f));
  mercurio_model = glm::rotate(mercurio_model, glm::radians(0.005f * count),
                               glm::vec3(0, 1, 0));
  mercurio_model = glm::scale(mercurio_model, glm::vec3(0.004f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &mercurio_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.588f, 0.588f, 0.588f, 1.0f);
  m_mercurio_model.render(-1);

  // Venus
  glm::mat4 venus_model{1.0f};
  venus_model = glm::rotate(venus_model, glm::radians(0.035f * count),
                            glm::vec3(0, 1, 0));
  venus_model =
      glm::translate(venus_model, glm::vec3(250.0f * 0.001f, 0.0f, 0.0f));
  venus_model = glm::rotate(venus_model, glm::radians(0.005f * count),
                            glm::vec3(0, 1, 0));
  venus_model = glm::scale(venus_model, glm::vec3(0.009f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &venus_model[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 0.517f, 0.278f, 1.0f);
  m_venus_model.render(-1);

  // Earth
  glm::mat4 earth_model{1.0f};
  earth_model = glm::rotate(earth_model, glm::radians(0.0298f * count),
                            glm::vec3(0, 1, 0));
  earth_model =
      glm::translate(earth_model, glm::vec3(350.0f * 0.001f, 0.0f, 0.0f));
  earth_model = glm::rotate(earth_model, glm::radians(0.005f * count),
                            glm::vec3(0, 1, 0));
  earth_model = glm::scale(earth_model, glm::vec3(18.0f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &earth_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.101f, 0.345f, 0.576f, 1.0f);
  m_earth_model.render(-1);

  // Moon
  glm::mat4 moon_model{1.0f};
  moon_model = glm::rotate(moon_model, glm::radians(0.0298f * count),
                           glm::vec3(0, 1, 0));
  moon_model =
      glm::translate(moon_model, glm::vec3(350.0f * 0.001f, 0.0f, 0.0f));
  moon_model = glm::rotate(moon_model, glm::radians(0.0298f * 2.0f * count),
                           glm::vec3(0, 1, 0));
  moon_model =
      glm::translate(moon_model, glm::vec3(30.0f * 0.001f, 0.0f, 0.0f));
  moon_model =
      glm::rotate(moon_model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  moon_model = glm::scale(moon_model, glm::vec3(3.0f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &moon_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.400f, 0.400f, 0.400f, 1.0f);
  m_moon_model.render(-1);

  // Mars
  glm::mat4 mars_model{1.0f};
  mars_model = glm::rotate(mars_model, glm::radians(0.0241f * count),
                           glm::vec3(0, 1, 0));
  mars_model =
      glm::translate(mars_model, glm::vec3(450.0f * 0.001f, 0.0f, 0.0f));
  mars_model =
      glm::rotate(mars_model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  mars_model = glm::scale(mars_model, glm::vec3(2.5f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &mars_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.976f, 0.262f, 0.262f, 1.0f);
  m_mars_model.render(-1);

  // Jupiter
  glm::mat4 jupiter_model{1.0f};
  jupiter_model = glm::rotate(jupiter_model, glm::radians(0.0131f * count),
                              glm::vec3(0, 1, 0));
  jupiter_model =
      glm::translate(jupiter_model, glm::vec3(560.0f * 0.001f, 0.0f, 0.0f));
  jupiter_model = glm::rotate(jupiter_model, glm::radians(0.005f * count),
                              glm::vec3(0, 1, 0));
  jupiter_model = glm::scale(jupiter_model, glm::vec3(0.08f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &jupiter_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.725f, 0.541f, 0.372f, 1.0f);
  m_jupiter_model.render(-1);

  // Saturn
  glm::mat4 saturn_model{1.0f};
  saturn_model = glm::rotate(saturn_model, glm::radians(0.0097f * count),
                             glm::vec3(0, 1, 0));
  saturn_model =
      glm::translate(saturn_model, glm::vec3(700.0f * 0.001f, 0.0f, 0.0f));
  saturn_model = glm::rotate(saturn_model, glm::radians(0.005f * count),
                             glm::vec3(0, 1, 0));
  saturn_model = glm::scale(saturn_model, glm::vec3(0.1f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &saturn_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.784f, 0.666f, 0.254f, 1.0f);
  m_saturn_model.render(-1);

  // Uranus
  glm::mat4 uranus_model{1.0f};
  uranus_model = glm::rotate(uranus_model, glm::radians(0.0068f * count),
                             glm::vec3(0, 1, 0));
  uranus_model =
      glm::translate(uranus_model, glm::vec3(830.0f * 0.001f, 0.0f, 0.0f));
  uranus_model = glm::rotate(uranus_model, glm::radians(0.005f * count),
                             glm::vec3(0, 1, 0));
  uranus_model = glm::scale(uranus_model, glm::vec3(0.07f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &uranus_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.313f, 0.780f, 0.984f, 1.0f);
  m_uranus_model.render(-1);

  // Neptuno
  glm::mat4 neptuno_model{1.0f};
  neptuno_model = glm::rotate(
      neptuno_model, glm::radians(0.0054f * count * 0.1f), glm::vec3(0, 1, 0));
  neptuno_model =
      glm::translate(neptuno_model, glm::vec3(930.0f * 0.001f, 0.0f, 0.0f));
  neptuno_model = glm::rotate(neptuno_model, glm::radians(0.005f * count),
                              glm::vec3(0, 1, 0));
  neptuno_model = glm::scale(neptuno_model, glm::vec3(0.004f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &neptuno_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.231f, 0.415f, 0.768f, 1.0f);
  m_neptuno_model.render(-1);

  // Pluto
  glm::mat4 pluto_model{1.0f};
  pluto_model = glm::rotate(pluto_model, glm::radians(0.0047f * count * 0.1f),
                            glm::vec3(0, 1, 0));
  pluto_model =
      glm::translate(pluto_model, glm::vec3(980.0f * 0.001f, 0.0f, 0.0f));
  pluto_model = glm::rotate(pluto_model, glm::radians(0.005f * count),
                            glm::vec3(0, 1, 0));
  pluto_model = glm::scale(pluto_model, glm::vec3(0.005f * 0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &pluto_model[0][0]);
  abcg::glUniform4f(colorLoc, 0.231f, 0.415f, 0.768f, 1.0f);
  m_pluto_model.render(-1);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
  if (!pause) count++;
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  const auto size{ImVec2(500, 300)};
  const auto position{ImVec2(0.8f, 0.8f)};
  const auto size2{ImVec2(105, 30)};
  const auto position2{ImVec2((m_viewportWidth - size2.x - 0.8f), 0.8f)};
  const auto size3{ImVec2(400, 80)};
  const auto position3{ImVec2((m_viewportWidth - size3.x - 0.8f),
                              (m_viewportHeight - size3.y - 0.8f))};
  ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoInputs};

  ImGui::SetNextWindowPos(position);
  ImGui::SetNextWindowSize(size);
  ImGui::Begin("Controls", nullptr, flags);
  ImGui::Text("Controls:");
  ImGui::Text("Dolly:  8 / 2 (numeric keyboard preferred)");
  ImGui::Text("Truck : 4 / 6 (numeric keyboard preferred)");
  ImGui::Text("Pedestal :  Up / Down (arrow keys)");
  ImGui::Text("Pan : A / D");
  ImGui::Text("Tilt : W / S");
  ImGui::Text("Reset: R");
  ImGui::Text("Pause : P");
  ImGui::End();

  ImGui::SetNextWindowPos(position2);
  ImGui::SetNextWindowSize(size2);
  ImGui::Begin(" ", nullptr, flags);
  ImGui::Text("Not to Scale");
  ImGui::End();

  ImGui::SetNextWindowPos(position3);
  ImGui::SetNextWindowSize(size3);
  ImGui::Begin("Coordinates", nullptr, flags);
  ImGui::Text("Coordinates:");
  ImGui::Text("Camera - X: %f ,Y: %f ,Z: %f ", m_camera.m_eye.x,
              m_camera.m_eye.y, m_camera.m_eye.z);
  ImGui::Text("Looking at - X: %f ,Y: %f ,Z: %f ", m_camera.m_at.x,
              m_camera.m_at.y, m_camera.m_at.z);
  ImGui::End();
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  m_sun_model.terminateGL();
  m_earth_model.terminateGL();
  m_mercurio_model.terminateGL();
  m_venus_model.terminateGL();
  m_mars_model.terminateGL();
  m_jupiter_model.terminateGL();
  m_neptuno_model.terminateGL();
  m_saturn_model.terminateGL();
  m_uranus_model.terminateGL();
  m_pluto_model.terminateGL();
  abcg::glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pedestal(m_pedestalSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.tilt(m_tiltSpeed * deltaTime);
}
#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

int count = 1;

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
    if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a)
      m_panSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d)
      m_panSpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_q) m_truckSpeed = -0.3f;
    if (ev.key.keysym.sym == SDLK_e) m_truckSpeed = 0.3f;
    if (ev.key.keysym.sym == SDLK_r) m_liftSpeed = -0.3f;
    if (ev.key.keysym.sym == SDLK_f) m_liftSpeed = 0.3f;

    // Tentativa de fazer a camera do eixo X
    // if (ev.key.keysym.sym == SDLK_t) m_panXSpeed = -1.0f;
    // if (ev.key.keysym.sym == SDLK_g) m_panXSpeed = 1.0f;
  }
  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_q && m_truckSpeed < 0) m_truckSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_e && m_truckSpeed > 0) m_truckSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_r && m_liftSpeed < 0) m_liftSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_f && m_liftSpeed > 0) m_liftSpeed = 0.0f;

    // Tentativa de fazer a camera do eixo X
    // if (ev.key.keysym.sym == SDLK_t && m_panXSpeed < 0) m_panXSpeed = 0.0f;
    // if (ev.key.keysym.sym == SDLK_g && m_panXSpeed > 0) m_panXSpeed = 0.0f;
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
  loadModelFromFile(getAssetsPath() + "earth.obj");

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  const GLint positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  resizeGL(getWindowSettings().width, getWindowSettings().height);
}

void OpenGLWindow::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      const tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // Vertex position
      const int startIndex{3 * index.vertex_index};
      const float vx{attrib.vertices.at(startIndex + 0)};
      const float vy{attrib.vertices.at(startIndex + 1)};
      const float vz{attrib.vertices.at(startIndex + 2)};

      Vertex vertex{};
      vertex.position = {vx, vy, vz};

      // If hash doesn't contain this vertex
      if (hash.count(vertex) == 0) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
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
  glm::mat4 model{1.0f};
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.5f * 0.1f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.976f, 0.733f, 0.184f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Mercury
  model = glm::mat4(1.0);
  model = glm::rotate(model, glm::radians(0.0479f * count), glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-1.8f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.008 * 0.383f * 0.1f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.588f, 0.588f, 0.588f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Venus
  model = glm::mat4(1.0);
  model = glm::rotate(model, glm::radians(0.035f * count), glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-2.0f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.008 * 0.949f * 0.1f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 0.517f, 0.278f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Earth
  model = glm::mat4(1.0);
  model = glm::rotate(model, glm::radians(0.0298f * count), glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-2.2f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.008f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.101f, 0.345f, 0.576f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Moon
  model = glm::mat4(1.0);
  model = glm::rotate(model, glm::radians(0.0298f * count), glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-2.2f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.0298f * 2.0f * count),
                      glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-0.5f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.008f * 0.273f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.200f, 0.200f, 0.200f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Mars
  model = glm::mat4(1.0);
  model = glm::rotate(model, glm::radians(0.0241f * count), glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-2.4f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.008f * 0.532f * 0.1f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.976f, 0.262f, 0.262f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Jupiter
  model = glm::mat4(1.0);
  model = glm::rotate(model, glm::radians(0.0131f * count), glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-2.8f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.008f * 11.21f * 0.1f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.725f, 0.541f, 0.372f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Saturn
  model = glm::mat4(1.0);
  model = glm::rotate(model, glm::radians(0.0097f * count), glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-3.6f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.008f * 9.45f * 0.1f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.784f, 0.666f, 0.254f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Uranus
  model = glm::mat4(1.0);
  model = glm::rotate(model, glm::radians(0.0068f * count), glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-4.1f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.008f * 4.01f * 0.1f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.313f, 0.780f, 0.984f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Neptune
  model = glm::mat4(1.0);
  model = glm::rotate(model, glm::radians(0.0054f * count * 0.1f),
                      glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(-4.6f * 0.1f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.005f * count), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.008f * 3.88f * 0.1f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.231f, 0.415f, 0.768f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 0.0f, 0.8f, 1.0f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);
  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
  count++;
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  const auto size{ImVec2(100, 20)};
  const auto position{
      ImVec2(((m_viewportHeight - size.x) / 2.0f) * 0.6f, 0.8f)};
  ImGui::SetNextWindowPos(position);
  ImGui::SetNextWindowSize(size);
  ImGui::Text("Eye - Px: %f ,Py: %f ,Pz: %f ", m_camera.m_eye.x,
              m_camera.m_eye.y, m_camera.m_eye.z);
  ImGui::Text("At - Px: %f ,Py: %f ,Pz: %f ", m_camera.m_at.x, m_camera.m_at.y,
              m_camera.m_at.z);
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.lift(m_liftSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  // Tentativa de fazer a camera do eixo X
  // m_camera.panX(m_panXSpeed * deltaTime);
}
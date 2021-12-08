#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <string>

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
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);

  // Create program
  auto path{getAssetsPath() + "shaders/texture"};
  auto program{createProgramFromFile(path + ".vert", path + ".frag")};
  m_program = program;

  // Load default model
  loadModel();

  // Load cube map
  m_modelBox.loadCubeTexture(getAssetsPath() + "maps/cube/");
  initializeSkybox();
}

void OpenGLWindow::initializeSkybox() {
  // Create skybox program
  const auto path{getAssetsPath() + "shaders/skybox"};
  m_skyProgram = createProgramFromFile(path + ".vert", path + ".frag");

  // Generate VBO
  abcg::glGenBuffers(1, &m_skyVBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_skyPositions),
                     m_skyPositions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const GLint positionAttribute{
      abcg::glGetAttribLocation(m_skyProgram, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_skyVAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_skyVAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void OpenGLWindow::loadModel() {
  // Load every object in the list astro
  for (int i = 0; i < 11; i++) {
    astros[i].m_model.loadFromFile(getAssetsPath() + names[i][0]);
    astros[i].m_model.loadDiffuseTexture(getAssetsPath() + "maps/" +
                                         names[i][1]);
    astros[i].m_model.setupVAO(m_program);
    astros[i].m_trianglesToDraw = astros[i].m_model.getNumTriangles();
  }

  // Use material properties from the loaded model
  m_Ka = astros[1].m_model.getKa();
  m_Kd = astros[1].m_model.getKd();
  m_Ks = astros[1].m_model.getKs();
  m_shininess = 50.0f;
}

void OpenGLWindow::paintGL() {
  update();

  glEnable(GL_CULL_FACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  glUseProgram(m_program);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(m_program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(m_program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(m_program, "shininess")};
  GLint IaLoc{glGetUniformLocation(m_program, "Ia")};
  GLint IdLoc{glGetUniformLocation(m_program, "Id")};
  GLint IsLoc{glGetUniformLocation(m_program, "Is")};
  GLint KaLoc{glGetUniformLocation(m_program, "Ka")};
  GLint KdLoc{glGetUniformLocation(m_program, "Kd")};
  GLint KsLoc{glGetUniformLocation(m_program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(m_program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(m_program, "normalTex")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_camera.m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_camera.m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform4fv(lightDirLoc, 1, &m_lightDir.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);

  // Sun material properties
  float mat[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  glUniform1f(shininessLoc, 5000.0f);
  glUniform4fv(KaLoc, 1, mat);
  glUniform4fv(KdLoc, 1, mat);
  glUniform4fv(KsLoc, 1, mat);

  // Rendering the astronomical objects
  for (int i = 0; i < 11; i++) {
    astros[i].m_modelMatrix = glm::mat4(1.0);

    // Making the moon [astro 4] orbit the earth [astro 3]
    if (i == 4) {
      astros[i].m_modelMatrix =
          glm::rotate(astros[i].m_modelMatrix,
                      glm::radians(properties[i - 1].revolution * count),
                      glm::vec3(0, 1, 0));
      astros[i].m_modelMatrix =
          glm::translate(astros[i].m_modelMatrix, properties[i - 1].position);
    }

    // If the astronomical object is not the sun [astro 0], make then
    // orbit the sun
    if (i != 0) {
      glUniform1f(shininessLoc, m_shininess);
      glUniform4fv(KaLoc, 1, &m_Ka.x);
      glUniform4fv(KdLoc, 1, &m_Kd.x);
      glUniform4fv(KsLoc, 1, &m_Ks.x);

      astros[i].m_modelMatrix = glm::rotate(
          astros[i].m_modelMatrix,
          glm::radians(properties[i].revolution * count), glm::vec3(0, 1, 0));
    }
    astros[i].m_modelMatrix =
        glm::translate(astros[i].m_modelMatrix, properties[i].position);
    astros[i].m_modelMatrix =
        glm::rotate(astros[i].m_modelMatrix, glm::radians(-0.005f * count),
                    glm::vec3(0.0f, 1.0f, 0.0f));

    // Chaging the scale e orietation of the astro
    astros[i].m_modelMatrix =
        glm::rotate(astros[i].m_modelMatrix, glm::radians(90.0f),
                    properties[i].orientation);
    astros[i].m_modelMatrix =
        glm::scale(astros[i].m_modelMatrix, glm::vec3(properties[i].scale));

    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE,
                       &astros[i].m_modelMatrix[0][0]);

    auto modelViewMatrix{
        glm::mat3(m_camera.m_viewMatrix * astros[i].m_modelMatrix)};
    glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

    astros[i].m_model.render(astros[i].m_trianglesToDraw);
  }
  renderSkybox();
  abcg::glUseProgram(0);
  if (!pause) count += 20 * simulation_velocity;
}

void OpenGLWindow::renderSkybox() {
  abcg::glUseProgram(m_skyProgram);

  // Get location of uniform variables
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_skyProgram, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_skyProgram, "projMatrix")};
  const GLint skyTexLoc{abcg::glGetUniformLocation(m_skyProgram, "skyTex")};

  // Set uniform variables
  const auto viewMatrix{m_camera.m_viewMatrix};
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(skyTexLoc, 0);

  abcg::glBindVertexArray(m_skyVAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_modelBox.getCubeTexture());

  abcg::glEnable(GL_CULL_FACE);
  abcg::glFrontFace(GL_CW);
  abcg::glDepthFunc(GL_LEQUAL);
  abcg::glDrawArrays(GL_TRIANGLES, 0, m_skyPositions.size());
  abcg::glDepthFunc(GL_LESS);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  const auto size{ImVec2(340, 240)};
  const auto position{ImVec2(5, 5)};
  const auto size2{ImVec2(105, 30)};
  const auto position2{ImVec2((m_viewportWidth - size2.x - 5), 5)};
  const auto size3{ImVec2(400, 80)};
  const auto position3{ImVec2((m_viewportWidth - size3.x - 5),
                              (m_viewportHeight - size3.y - 5))};
  const auto position4{ImVec2(350, 5)};
  ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoInputs};

  // Menu widget
  {
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Menu");
    ImGui::Text("              Camera controls");
    ImGui::Text("Dolly:  8 / 2 (numeric keyboard preferred)");
    ImGui::Text("Truck : 4 / 6 (numeric keyboard preferred)");
    ImGui::Text("Pedestal :  Up / Down (arrow keys)");
    ImGui::Text("Pan : A / D");
    ImGui::Text("Tilt : W / S");
    ImGui::Text("Reset: R");
    ImGui::Text("Pause : P");
    ImGui::Text("        Simulation Velocity control");
    ImGui::PushItemWidth(size.x - 16);
    ImGui::SliderFloat("", &simulation_velocity, 0.1f, 10.0f, "%.2fx velocity");
    ImGui::PopItemWidth();
    ImGui::End();
  }

  // Not to scale message
  {
    ImGui::SetNextWindowPos(position2);
    ImGui::SetNextWindowSize(size2);
    ImGui::Begin("Not to scale", nullptr, flags);
    ImGui::Text("Not to Scale");
    ImGui::End();
  }

  // Coordinates widget
  {
    // changes layout depending on the size of the window
    if (m_viewportWidth <= 860) {
      ImGui::SetNextWindowPos(position3);
    } else {
      ImGui::SetNextWindowPos(position4);
    }
    ImGui::SetNextWindowSize(size3);
    ImGui::Begin("Coordinates", nullptr);
    ImGui::Text("Camera - X: %f ,Y: %f ,Z: %f ", m_camera.m_eye.x,
                m_camera.m_eye.y, m_camera.m_eye.z);
    ImGui::Text("Looking at - X: %f ,Y: %f ,Z: %f ", m_camera.m_at.x,
                m_camera.m_at.y, m_camera.m_at.z);
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  for (int i = 0; i < 11; i++) {
    astros[i].m_model.terminateGL();
  }
  abcg::glDeleteProgram(m_program);
  terminateSkybox();
}

void OpenGLWindow::terminateSkybox() {
  abcg::glDeleteProgram(m_skyProgram);
  abcg::glDeleteBuffers(1, &m_skyVBO);
  abcg::glDeleteVertexArrays(1, &m_skyVAO);
  abcg::glDeleteTextures(1, &m_skyTexture);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pedestal(m_pedestalSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.tilt(m_tiltSpeed * deltaTime);
}
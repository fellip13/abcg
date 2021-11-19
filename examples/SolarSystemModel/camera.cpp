#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Camera::computeProjectionMatrix(int width, int height) {
  m_projMatrix = glm::mat4(1.0f);
  const auto aspect{static_cast<float>(width) / static_cast<float>(height)};
  m_projMatrix = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 5.0f);
}

void Camera::computeViewMatrix() {
  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}

void Camera::dolly(float speed) {
  // Compute forward vector (view direction)
  const glm::vec3 forward{glm::normalize(m_at - m_eye)};

  // Move eye and center forward (speed > 0) or backward (speed < 0)
  m_eye += forward * speed;
  m_at += forward * speed;

  computeViewMatrix();
}

void Camera::lift(float speed) {
  // Compute vector to the up
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  // Move eye and center to the up (speed < 0) or to the down (speed > 0)
  m_at -= up * speed;
  m_eye -= up * speed;

  computeViewMatrix();
}

void Camera::truck(float speed) {
  // Compute forward vector (view direction)
  const glm::vec3 forward{glm::normalize(m_at - m_eye)};
  // Compute vector to the left
  const glm::vec3 left{glm::cross(m_up, forward)};

  // Move eye and center to the left (speed < 0) or to the right (speed > 0)
  m_at -= left * speed;
  m_eye -= left * speed;

  computeViewMatrix();
}

// Tentativa de fazer a camera do eixo X
// void Camera::panX(float speed) {
//   glm::mat4 transform{glm::mat4(1.0f)};
//   glm::vec3 x = {glm::vec3(1.0f, 0.0f, 0.0f)};

//   // Rotate camera around its local x axis
//   transform = glm::translate(transform, m_eye);
//   transform = glm::rotate(transform, -speed, x);
//   transform = glm::translate(transform, -m_eye);

//   m_at = transform * glm::vec4(m_at, 1.0f);

//   computeViewMatrix();
// }

void Camera::pan(float speed) {
  glm::mat4 transform{glm::mat4(1.0f)};

  // Rotate camera around its local y axis
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, -speed, m_up);
  transform = glm::translate(transform, -m_eye);

  m_at = transform * glm::vec4(m_at, 1.0f);

  computeViewMatrix();
}
//
// Created by henry on 3/25/23.
//

#ifndef JAMEGAM_CAMERACONTROLLER_H
#define JAMEGAM_CAMERACONTROLLER_H

#include <hagame/graphics/camera.h>
#include <hagame/graphics/resolution.h>

class CameraController {
public:

    CameraController() {
        m_camera.centered = true;
    }

    void setResolution(hg::graphics::Resolution resolution) {
        m_camera.size = resolution;
    }

    hg::graphics::OrthographicCamera* camera() {
        return &m_camera;
    }

    void move(hg::Vec3 pos, float seconds = 1.0) {
        if (pos == m_camera.transform.position || m_lerping) {
            return;
        }

        m_lerping = true;
        m_start = m_camera.transform.position;
        m_end = pos;
        m_speed = seconds;
        m_t = 0;
    }

    void update(double dt) {
        if (m_lerping) {
            m_t += dt / m_speed;
            if (m_t >= 1.0) {
                m_t = 1.0;
                m_lerping = false;
            }

            m_camera.transform.position = m_start + (m_end - m_start) * m_t;

        }
    }

private:

    hg::Vec3 m_start, m_end;
    bool m_lerping = false;
    float m_speed;
    float m_t;

    hg::graphics::OrthographicCamera m_camera;

};

#endif //JAMEGAM_CAMERACONTROLLER_H

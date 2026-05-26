#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <array>

#include <sim/sim_leg.h>
#include <sim/sim_gait.h>

std::array<std::string, 12> actuator_names = {
    "tl_coxa_motor","tl_femur_motor","tl_tibia_motor",
    "tr_coxa_motor","tr_femur_motor","tr_tibia_motor",
    "bl_coxa_motor","bl_femur_motor","bl_tibia_motor",
    "br_coxa_motor","br_femur_motor","br_tibia_motor"
};

pluto::sim::SimGaitController gait;
pluto::sim::SimLegJointType current_joint   = pluto::sim::SimLegJointType::COXA;
pluto::LegSide current_side              = pluto::LegSide::TOP_LEFT;

void handle_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    switch(key) {
        case GLFW_KEY_F:
            gait.set_motion(pluto::sim::SimMotionCommand::FORWARD);
            std::cout << "Motion: forward" << std::endl;
            break;
        case GLFW_KEY_B:
            gait.set_motion(pluto::sim::SimMotionCommand::BACKWARD);
            std::cout << "Motion: backward" << std::endl;
            break;
        case GLFW_KEY_S:
            gait.set_motion(pluto::sim::SimMotionCommand::IDLE);
            std::cout << "Motion: stop" << std::endl;
            break;
        case GLFW_KEY_1:
            gait.set_gait(pluto::sim::SimGaitKind::WALK);
            std::cout << "Gait: walk" << std::endl;
            break;
        case GLFW_KEY_2:
            gait.set_gait(pluto::sim::SimGaitKind::TROT);
            std::cout << "Gait: trot" << std::endl;
            break;
        case GLFW_KEY_3:
            gait.set_gait(pluto::sim::SimGaitKind::GALLOP);
            std::cout << "Gait: gallop" << std::endl;
            break;
    }
}

int main() {
    const char* model_path = PLUTO_MODEL_PATH;
    char error[1000] = "Could not load model";
    mjModel* m = mj_loadXML(model_path, nullptr, error, 1000);
    if (!m) {
        std::cerr << "Error loading model: " << error << std::endl;
        return 1;
    }
    std::cout << "Model loaded successfully" << std::endl;
    std::cout << "Number of actuators: " << m->nu << std::endl;
    for (int i = 0; i < m->nu; ++i) {
    std::cout << "Actuator " << i << ": "
              << mj_id2name(m, mjOBJ_ACTUATOR, i)
              << std::endl;
    }

    mjData* d = mj_makeData(m);

    if (!d) {
        std::cerr << "Failed to create MuJoCo data" << std::endl;
        return 1;
    }

    // --------------------------------------
    // GLFW initialization
    // --------------------------------------
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Pluto Simulation", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    std::cout << "OpenGL version: "
            << glGetString(GL_VERSION)
            << std::endl;

    std::cout << "Renderer: "
            << glGetString(GL_RENDERER)
            << std::endl;

    glfwSwapInterval(1);
    glfwSetKeyCallback(window, handle_key);

    mjvCamera cam;
    mjvOption opt;
    mjvScene scn;
    mjrContext con;

    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjv_makeScene(m, &scn, 1000);

    mjr_defaultContext(&con);
    
    std::cout << "Creating MuJoCo context..." << std::endl;
    mjr_makeContext(m, &con, mjFONTSCALE_150);
    std::cout << "Context created" << std::endl;
    // --------------------------------------
    // Initialize legs and gait
    // --------------------------------------
    std::array<pluto::sim::SimLeg, 4> legs = {
        pluto::sim::SimLeg{pluto::LegSide::TOP_LEFT},
        pluto::sim::SimLeg{pluto::LegSide::TOP_RIGHT},
        pluto::sim::SimLeg{pluto::LegSide::BOTTOM_LEFT},
        pluto::sim::SimLeg{pluto::LegSide::BOTTOM_RIGHT}
    };
    gait.stand(legs);

    auto start_time = std::chrono::steady_clock::now();

    std::array<int, 12> actuator_indices;
    for (int i=0;i<12;i++) {
        actuator_indices[i] = mj_name2id(m, mjOBJ_ACTUATOR, actuator_names[i].c_str());
        if (actuator_indices[i] == -1) {
            std::cerr << "Actuator not found: " << actuator_names[i] << std::endl;
            return 1;
        }
    }

    // --------------------------------------
    // Simulation loop
    // --------------------------------------
    constexpr double PI = 3.14159265358979323846;
    std::cout << "Starting simulation loop..." << std::endl;
    while (!glfwWindowShouldClose(window)) {
        auto now = std::chrono::steady_clock::now();
        double t = std::chrono::duration_cast<std::chrono::duration<double>>(now - start_time).count();

        gait.update(legs, static_cast<uint32_t>(t*1000));

        int idx = 0;
        for (auto side : {pluto::LegSide::TOP_LEFT, pluto::LegSide::TOP_RIGHT,
                          pluto::LegSide::BOTTOM_LEFT, pluto::LegSide::BOTTOM_RIGHT}) {
            for (auto joint : {pluto::sim::SimLegJointType::COXA, pluto::sim::SimLegJointType::FEMUR, pluto::sim::SimLegJointType::TIBIA}) {
                int32_t md = legs[(uint8_t)side][joint].current_angle();
                double rad = md * (PI / 180000.0);
                d->ctrl[actuator_indices[idx]] = rad;
                idx++;
            }
        }

        double simstart = d->time;

        while (d->time - simstart < 1.0 / 60.0)
        {
            if (std::isnan(d->qpos[0])) {
                std::cerr << "Simulation exploded (NaN detected)" << std::endl;
                break;
            }
            mj_step(m, d);
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        mjrRect viewport = {0, 0, width, height};
        mjv_updateScene(m, d, &opt, nullptr, &cam, mjCAT_ALL, &scn);
        mjr_render(viewport, &scn, &con);

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    mjv_freeScene(&scn);
    mjr_freeContext(&con);
    mj_deleteData(d);
    mj_deleteModel(m);
    glfwDestroyWindow(window);
    glfwTerminate();
}
#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>
#include <Adafruit_PWMServoDriver.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <array>

#include <legs/leg.h>
#include <motion/gait.h>

std::array<std::string, 12> actuator_names = {
    "tl_coxa_motor","tl_femur_motor","tl_tibia_motor",
    "tr_coxa_motor","tr_femur_motor","tr_tibia_motor",
    "bl_coxa_motor","bl_femur_motor","bl_tibia_motor",
    "br_coxa_motor","br_femur_motor","br_tibia_motor"
};

pluto::motion::GaitController gait;
pluto::LegJointType current_joint   = pluto::LegJointType::COXA;
pluto::LegSide current_side         = pluto::LegSide::TOP_LEFT;

void handle_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    switch(key) {
        case GLFW_KEY_F:
            gait.set_motion(pluto::motion::MotionCommand::FORWARD);
            std::cout << "Motion: forward" << std::endl;
            break;
        case GLFW_KEY_B:
            gait.set_motion(pluto::motion::MotionCommand::BACKWARD);
            std::cout << "Motion: backward" << std::endl;
            break;
        case GLFW_KEY_S:
            gait.set_motion(pluto::motion::MotionCommand::IDLE);
            std::cout << "Motion: stop" << std::endl;
            break;
        case GLFW_KEY_1:
            gait.set_gait(pluto::motion::GaitKind::WALK);
            std::cout << "Gait: walk" << std::endl;
            break;
        case GLFW_KEY_2:
            gait.set_gait(pluto::motion::GaitKind::TROT);
            std::cout << "Gait: trot" << std::endl;
            break;
        case GLFW_KEY_3:
            gait.set_gait(pluto::motion::GaitKind::GALLOP);
            std::cout << "Gait: gallop" << std::endl;
            break;
        case GLFW_KEY_R:
            std::cout << "Reset current leg joint" << std::endl;
            break;
        case GLFW_KEY_L:
            current_side = pluto::next_leg_side(current_side);
            break;
        case GLFW_KEY_N:
            current_joint = pluto::next_leg_joint_type(current_joint);
            break;
        case GLFW_KEY_EQUAL:
            break;
        case GLFW_KEY_MINUS:
            break;
    }
}

int main() {
    const char* model_path = "../../mesh/pluto.xml";
    char error[1000] = "Could not load model";
    mjModel* m = mj_loadXML(model_path, nullptr, error, 1000);
    if (!m) {
        std::cerr << "Error loading model: " << error << std::endl;
        return 1;
    }
    mjData* d = mj_makeData(m);

    // --------------------------------------
    // GLFW initialization
    // --------------------------------------
    if (!glfwInit()) return 1;
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Pluto Simulation", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, handle_key);

    mjvCamera cam;
    mjvOption opt;
    mjvScene scn;
    mjrContext con;

    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjv_makeScene(m, &scn, 1000);
    mjr_makeContext(m, &con, mjFONTSCALE_150);

    // --------------------------------------
    // Initialize legs and gait
    // --------------------------------------
    Adafruit_PWMServoDriver PWM = Adafruit_PWMServoDriver();
    std::array<pluto::Leg, 4> legs = {
        pluto::Leg{PWM, pluto::LegSide::TOP_LEFT},
        pluto::Leg{PWM, pluto::LegSide::TOP_RIGHT},
        pluto::Leg{PWM, pluto::LegSide::BOTTOM_LEFT},
        pluto::Leg{PWM, pluto::LegSide::BOTTOM_RIGHT}
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
    while (!glfwWindowShouldClose(window)) {
        auto now = std::chrono::steady_clock::now();
        double t = std::chrono::duration_cast<std::chrono::duration<double>>(now - start_time).count();

        gait.update(legs, static_cast<uint32_t>(t*1000));

        int idx = 0;
        for (auto side : {pluto::LegSide::TOP_LEFT, pluto::LegSide::TOP_RIGHT,
                          pluto::LegSide::BOTTOM_LEFT, pluto::LegSide::BOTTOM_RIGHT}) {
            for (auto joint : {pluto::LegJointType::COXA, pluto::LegJointType::FEMUR, pluto::LegJointType::TIBIA}) {
                int32_t md = legs[(uint8_t)side][(uint8_t)joint].current_angle();
                double rad = md * (M_PI / 180000.0);
                d->ctrl[actuator_indices[idx]] = rad;
                idx++;
            }
        }

        mj_step(m, d);

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
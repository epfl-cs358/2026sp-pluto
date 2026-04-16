from __future__ import annotations

import logging
import math
import multiprocessing as mp
import queue
import time

JOINT_MAP = {
    # Front Left
    "fl_hip": 1,
    "fl_upper": 2,

    # Front Right
    "fr_hip": 5,
    "fr_upper": 6,

    # Back Left
    "bl_hip": 9,
    "bl_upper": 10,

    # Back Right
    "br_hip": 13,
    "br_upper": 14,
}

logger = logging.getLogger(__name__)

def _sim_process_main(cmd_q: mp.Queue) -> None:
    import pybullet as p

    client = p.connect(p.GUI)
    p.setGravity(0, 0, -9.81, physicsClientId=client)
    p.setRealTimeSimulation(0, physicsClientId=client)

    plane = p.createCollisionShape(p.GEOM_PLANE, physicsClientId=client)
    p.createMultiBody(0, plane, physicsClientId=client)

    x = 0.0
    y = 0.0
    yaw = 0.0
    z = 0.5

    # The robot body/link structure and part of the initial joint setup was adapted from:
    # RobotDog implementation by Richard Bloemenkamp
    # https://github.com/richardbloemenkamp/Robotdog/blob/master/robotdog.py

    sh_body = p.createCollisionShape(
        p.GEOM_BOX, halfExtents=[0.45, 0.08, 0.02], physicsClientId=client
    )
    sh_extraweight = p.createCollisionShape(
        p.GEOM_BOX, halfExtents=[0.45, 0.08, 0.025], physicsClientId=client
    )
    sh_roll = p.createCollisionShape(
        p.GEOM_BOX, halfExtents=[0.02, 0.02, 0.02], physicsClientId=client
    )
    sh_hip = p.createCollisionShape(
        p.GEOM_BOX, halfExtents=[0.02, 0.02, 0.02], physicsClientId=client
    )
    sh_knee = p.createCollisionShape(
        p.GEOM_BOX, halfExtents=[0.02, 0.02, 0.02], physicsClientId=client
    )
    sh_foot = p.createCollisionShape(
        p.GEOM_SPHERE, radius=0.04, physicsClientId=client
    )

    link_masses = [
        0.1, 0.1, 0.1, 0.1,
        0.1, 0.1, 0.1, 0.1,
        0.1, 0.1, 0.1, 0.1,
        0.1, 0.1, 0.1, 0.1,
        20,
    ]
    link_collision_shape_indices = [
        sh_roll, sh_hip, sh_knee, sh_foot,
        sh_roll, sh_hip, sh_knee, sh_foot,
        sh_roll, sh_hip, sh_knee, sh_foot,
        sh_roll, sh_hip, sh_knee, sh_foot,
        sh_extraweight,
    ]
    nlnk = len(link_masses)
    link_visual_shape_indices = [-1] * nlnk

    xhipf = 0.4
    xhipb = -0.4
    yhipl = 0.1
    xoffh = 0.05
    yoffh = 0.05
    hu = 0.3
    hl = 0.3

    link_positions = [
        [xhipf, yhipl, 0],
        [xoffh, yoffh, 0],
        [0, 0, -hu],
        [0, 0, -hl],
        [xhipf, -yhipl, 0],
        [xoffh, -yoffh, 0],
        [0, 0, -hu],
        [0, 0, -hl],
        [xhipb, yhipl, 0],
        [xoffh, yoffh, 0],
        [0, 0, -hu],
        [0, 0, -hl],
        [xhipb, -yhipl, 0],
        [xoffh, -yoffh, 0],
        [0, 0, -hu],
        [0, 0, -hl],
        [0, 0, +0.029],
    ]
    link_orientations = [[0, 0, 0, 1]] * nlnk
    link_inertial_frame_positions = [[0, 0, 0]] * nlnk
    link_inertial_frame_orientations = [[0, 0, 0, 1]] * nlnk
    indices = [
        0, 1, 2, 3,
        0, 5, 6, 7,
        0, 9, 10, 11,
        0, 13, 14, 15,
        0,
    ]
    joint_types = [
        p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_PRISMATIC,
        p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_PRISMATIC,
        p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_PRISMATIC,
        p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_PRISMATIC,
        p.JOINT_PRISMATIC,
    ]
    axis = [
        [1, 0, 0], [0, 1, 0], [0, 1, 0], [0, 0, 1],
        [1, 0, 0], [0, 1, 0], [0, 1, 0], [0, 0, 1],
        [1, 0, 0], [0, 1, 0], [0, 1, 0], [0, 0, 1],
        [1, 0, 0], [0, 1, 0], [0, 1, 0], [0, 0, 1],
        [0, 0, 1],
    ]

    robot_id = p.createMultiBody(
        1,
        sh_body,
        -1,
        [x, y, z],
        [0, 0, 0, 1],
        linkMasses=link_masses,
        linkCollisionShapeIndices=link_collision_shape_indices,
        linkVisualShapeIndices=link_visual_shape_indices,
        linkPositions=link_positions,
        linkOrientations=link_orientations,
        linkInertialFramePositions=link_inertial_frame_positions,
        linkInertialFrameOrientations=link_inertial_frame_orientations,
        linkParentIndices=indices,
        linkJointTypes=joint_types,
        linkJointAxis=axis,
        physicsClientId=client,
    )

    from servo import Servo
    sim_client = {"robot_id": robot_id}

    servos = {
        "fl_hip": Servo("fl_hip", JOINT_MAP["fl_hip"], sim_client),
        "fl_upper": Servo("fl_upper", JOINT_MAP["fl_upper"], sim_client),

        "fr_hip": Servo("fr_hip", JOINT_MAP["fr_hip"], sim_client),
        "fr_upper": Servo("fr_upper", JOINT_MAP["fr_upper"], sim_client),

        "bl_hip": Servo("bl_hip", JOINT_MAP["bl_hip"], sim_client),
        "bl_upper": Servo("bl_upper", JOINT_MAP["bl_upper"], sim_client),

        "br_hip": Servo("br_hip", JOINT_MAP["br_hip"], sim_client),
        "br_upper": Servo("br_upper", JOINT_MAP["br_upper"], sim_client),
    }

    servos["fl_hip"].set_angle(90)
    servos["fl_upper"].set_angle(90)
    servos["fr_hip"].set_angle(90)
    servos["fr_upper"].set_angle(90)
    servos["bl_hip"].set_angle(90)
    servos["bl_upper"].set_angle(90)
    servos["br_hip"].set_angle(90)
    servos["br_upper"].set_angle(90)

    # #test to move one joint
    # p.setJointMotorControl2(
    # bodyUniqueId=robot_id,
    # jointIndex=0,
    # controlMode=p.POSITION_CONTROL,
    # targetPosition=0.5,
    # force=500,
    # )   

    for i in range(p.getNumJoints(robot_id)):
        info = p.getJointInfo(robot_id, i)
        print(f"Joint {i}: {info[1].decode('utf-8')}")

    p.setJointMotorControl2(
        robot_id,
        16,
        p.POSITION_CONTROL,
        targetPosition=0.01,
        force=1000,
        maxVelocity=3,
        physicsClientId=client,
    )

    for joint in (3, 7, 11, 15):
        p.setJointMotorControl2(
            robot_id,
            joint,
            p.POSITION_CONTROL,
            targetPosition=0.0,
            force=1000,
            maxVelocity=3,
            physicsClientId=client,
        )
        p.changeDynamics(robot_id, joint, lateralFriction=2, physicsClientId=client)

    current_motion = "idle"
    current_speed = 1.0

    max_linear_speed = 3.0
    max_turn_rate = math.radians(120)

    while p.isConnected(client):
        try:
            while True:
                cmd = cmd_q.get_nowait()
                kind = cmd[0]

                if kind == "shutdown":
                    if p.isConnected(client):
                        p.disconnect(physicsClientId=client)
                    return

                if kind == "set_motion":
                    current_motion = str(cmd[1])
                    current_speed = max(0.0, min(float(cmd[2]), 1.0))

                elif kind == "stop_motion":
                    current_motion = "idle"
                    current_speed = 0.0

        except queue.Empty:
            pass

        dt = 1.0 / 240.0

        if current_motion == "forward":
            x += math.cos(yaw) * max_linear_speed * current_speed * dt
            y += math.sin(yaw) * max_linear_speed * current_speed * dt

        elif current_motion == "backward":
            x -= math.cos(yaw) * max_linear_speed * current_speed * dt
            y -= math.sin(yaw) * max_linear_speed * current_speed * dt

        elif current_motion == "turn_left":
            yaw += max_turn_rate * current_speed * dt

        elif current_motion == "turn_right":
            yaw -= max_turn_rate * current_speed * dt

        quat = p.getQuaternionFromEuler([0, 0, yaw])
        p.resetBasePositionAndOrientation(
            robot_id, [x, y, z], quat, physicsClientId=client
        )

        p.stepSimulation(physicsClientId=client)

        p.resetDebugVisualizerCamera(
            cameraDistance=1.6,
            cameraYaw=math.degrees(yaw) - 90,
            cameraPitch=-35,
            cameraTargetPosition=[x, y, 0.25],
            physicsClientId=client,
        )

        time.sleep(dt)

    if p.isConnected(client):
        p.disconnect(physicsClientId=client)


class PyBulletMotionController:
    """Simulates directional robot motion in a PyBullet window."""

    def __init__(self) -> None:
        self._process: mp.Process | None = None
        self._cmd_q: mp.Queue | None = None

    def _is_running(self) -> bool:
        return self._process is not None and self._process.is_alive()

    def start(self) -> None:
        if self._is_running():
            logger.info("Simulation already running")
            return

        self._cmd_q = mp.Queue()
        self._process = mp.Process(
            target=_sim_process_main,
            args=(self._cmd_q,),
            daemon=True,
        )
        self._process.start()
        logger.info("PyBullet simulation started")

    def set_motion(self, motion: str, speed: float = 1.0) -> None:
        logger.info("Set motion: %s at speed %.2f", motion, speed)
        self._send(("set_motion", motion, float(speed)))

    def stop(self) -> None:
        logger.info("Stop requested")
        self._send(("stop_motion",))

    def shutdown(self) -> None:
        proc = self._process
        cmd_q = self._cmd_q
        if proc is not None and proc.is_alive() and cmd_q is not None:
            cmd_q.put(("shutdown",))
            proc.join(timeout=1.0)
            if proc.is_alive():
                proc.terminate()
                proc.join(timeout=1.0)

        self._process = None
        self._cmd_q = None
        logger.info("PyBullet simulation closed")

    def _send(self, cmd: tuple) -> None:
        if not self._is_running() or self._cmd_q is None:
            raise RuntimeError("Simulation is not running. Click Open PyBullet first.")
        self._cmd_q.put(cmd)

if __name__ == "__main__":
    from servo import Servo
    from leg import Leg
    from robot import Robot

    # crete the servos for each leg

    # front left leg
    fl_hip = Servo("fl_hip")
    fl_upper = Servo("fl_upper")
    fl_lower = Servo("fl_lower")

    # front right leg
    fr_hip = Servo("fr_hip")
    fr_upper = Servo("fr_upper")
    fr_lower = Servo("fr_lower")

    # back left leg
    bl_hip = Servo("bl_hip")
    bl_upper = Servo("bl_upper")
    bl_lower = Servo("bl_lower")

    # back rigtn leg
    br_hip = Servo("br_hip")
    br_upper = Servo("br_upper")
    br_lower = Servo("br_lower")

    #create the the legs with 3 servos each

    front_left = Leg(fl_hip, fl_upper, fl_lower)
    front_right = Leg(fr_hip, fr_upper, fr_lower)
    back_left = Leg(bl_hip, bl_upper, bl_lower)
    back_right = Leg(br_hip, br_upper, br_lower)

    # createion of the robot 

    robot = Robot(front_left, front_right, back_left, back_right)

    # Put robot in standing position
    robot.stand()

    # Print all angles
    robot.print_state()
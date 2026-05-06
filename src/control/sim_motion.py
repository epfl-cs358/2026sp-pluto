import logging
import math
import multiprocessing as mp
import os
import queue
import time

from gait import Walk, Trot, Gallop

logger = logging.getLogger(__name__)

LEG_JOINTS = {
    "fl": {"coxa": 0, "femur": 1, "tibia": 2},
    "fr": {"coxa": 4, "femur": 5, "tibia": 6},
    "bl": {"coxa": 8, "femur": 9, "tibia": 10},
    "br": {"coxa": 12, "femur": 13, "tibia": 14},
}

def _sim_process_main(cmd_q: mp.Queue) -> None:
    import pybullet as p

    client = p.connect(p.GUI)
    p.setGravity(0, 0, -9.81, physicsClientId=client)
    p.setRealTimeSimulation(0, physicsClientId=client)
    p.configureDebugVisualizer(p.COV_ENABLE_WIREFRAME, 0, physicsClientId=client)

    plane = p.createCollisionShape(p.GEOM_PLANE, physicsClientId=client)
    p.createMultiBody(0, plane, physicsClientId=client)

    x = 0.0
    y = 0.0
    yaw = 0.0
    z = 2.5

    # The robot body/link structure and part of the initial joint setup was adapted from:
    # RobotDog implementation by Richard Bloemenkamp
    # https://github.com/richardbloemenkamp/Robotdog/blob/master/robotdog.py

    _mesh_body = os.path.join(os.path.dirname(__file__), "..", "mesh", "body.stl")
    _mesh_coxa = os.path.join(os.path.dirname(__file__), "..", "mesh", "coxa.stl")
    _mesh_femur = os.path.join(os.path.dirname(__file__), "..", "mesh", "femur.stl")
    _mesh_tibia = os.path.join(os.path.dirname(__file__), "..", "mesh", "tibia.stl")

    sh_body = p.createCollisionShape(
        p.GEOM_BOX, halfExtents=[0.88, 0.70, 0.17], physicsClientId=client
    )
    vs_body = p.createVisualShape(
        p.GEOM_MESH,
        fileName=_mesh_body,
        meshScale=[0.01, 0.01, 0.01],
        visualFramePosition=[-0.83, -1.77, -0.17],
        rgbaColor=[0.5, 1.0, 0.1, 1.0], # gray
        physicsClientId=client,
    )
    vs_coxa = p.createVisualShape(
        p.GEOM_MESH,
        fileName=_mesh_coxa,
        meshScale=[0.01, 0.01, 0.01],
        visualFramePosition=[-0.26, 0.000, -0.34],
        rgbaColor=[0.9, 0.15, 0.15, 1.0], # blue
        physicsClientId=client,
    )
    vs_femur = p.createVisualShape(
        p.GEOM_MESH,
        fileName=_mesh_femur,
        meshScale=[0.01, 0.01, 0.01],
        visualFramePosition=[-0.532, 0.487, -0.10],
        visualFrameOrientation=[0.7071, 0, 0, 0.7071],
        rgbaColor=[0.9, 0.15, 0.15, 1.0],
        physicsClientId=client,
    )
    vs_tibia = p.createVisualShape(
        p.GEOM_MESH,
        fileName=_mesh_tibia,
        meshScale=[0.01, 0.01, 0.01],
        visualFramePosition=[-0.505, 1.000, -0.721],
        rgbaColor=[0.9, 0.15, 0.15, 1.0], # orange
        physicsClientId=client,
    )
    link_masses = [
        0.1, 0.1, 0.1, 0.1,
        0.1, 0.1, 0.1, 0.1,
        0.1, 0.1, 0.1, 0.1,
        0.1, 0.1, 0.1, 0.1,
    ]
    link_collision_shape_indices = [-1] * 16
    nlnk = len(link_masses)
    link_visual_shape_indices = [
        vs_coxa, vs_femur, vs_tibia, -1,
        vs_coxa, vs_femur, vs_tibia, -1,
        vs_coxa, vs_femur, vs_tibia, -1,
        vs_coxa, vs_femur, vs_tibia, -1,
    ]

    xhipf = 0.75
    xhipb = -0.75
    yhipl = 0.65
    xoffh = 0.04
    yoffh = 0.39
    hu = 1.20
    hl = 1.36

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
    ]
    link_orientations = [[0, 0, 0, 1]] * nlnk
    link_inertial_frame_positions = [[0, 0, 0]] * nlnk
    link_inertial_frame_orientations = [[0, 0, 0, 1]] * nlnk
    indices = [
        0, 1, 2, 3,
        0, 5, 6, 7,
        0, 9, 10, 11,
        0, 13, 14, 15,
    ]
    joint_types = [
        p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_PRISMATIC,
        p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_PRISMATIC,
        p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_PRISMATIC,
        p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_REVOLUTE, p.JOINT_PRISMATIC,
    ]
    axis = [
        [1, 0, 0], [0, 1, 0], [0, 1, 0], [0, 0, 1],
        [1, 0, 0], [0, 1, 0], [0, 1, 0], [0, 0, 1],
        [1, 0, 0], [0, 1, 0], [0, 1, 0], [0, 0, 1],
        [1, 0, 0], [0, 1, 0], [0, 1, 0], [0, 0, 1],
    ]

    robot_id = p.createMultiBody(
        1,
        sh_body,
        vs_body,
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

    gait = Gallop(period=0.3)

    sim_time = 0.0

    # Place joints in the standing pose immediately
    for leg, joints in LEG_JOINTS.items():
        coxa_a, femur_a, tibia_a = gait.stand_angles(right_side=leg in ("fr", "br"))
        p.resetJointState(robot_id, joints["coxa"],  coxa_a,  physicsClientId=client)
        p.resetJointState(robot_id, joints["femur"], femur_a, physicsClientId=client)
        p.resetJointState(robot_id, joints["tibia"], tibia_a, physicsClientId=client)

    current_motion = "idle"
    current_speed = 1.0

    max_linear_speed = 3.0
    max_turn_rate = math.radians(60)

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

                elif kind == "set_gait":
                    name = str(cmd[1])
                    if name == "walk":
                        gait = Walk(period=0.5)
                    elif name == "trot":
                        gait = Trot(period=0.35)
                    elif name == "gallop":
                        gait = Gallop(period=0.3)

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

        # Animate leg joints
        if current_motion in ("forward", "backward", "turn_left", "turn_right"):
            forward_scale = -1.0 if current_motion == "backward" else 1.0
            for leg, joints in LEG_JOINTS.items():
                coxa_a, femur_a, tibia_a = gait.get_leg_angles(sim_time, leg, forward_scale)
                p.setJointMotorControl2(
                    robot_id, joints["coxa"], p.POSITION_CONTROL,
                    targetPosition=coxa_a, force=50, maxVelocity=15,
                    physicsClientId=client,
                )
                p.setJointMotorControl2(
                    robot_id, joints["femur"], p.POSITION_CONTROL,
                    targetPosition=femur_a, force=50, maxVelocity=15,
                    physicsClientId=client,
                )
                p.setJointMotorControl2(
                    robot_id, joints["tibia"], p.POSITION_CONTROL,
                    targetPosition=tibia_a, force=50, maxVelocity=15,
                    physicsClientId=client,
                )
        else:
            for leg, joints in LEG_JOINTS.items():
                coxa_a, femur_a, tibia_a = gait.stand_angles(right_side=leg in ("fr", "br"))
                p.setJointMotorControl2(
                    robot_id, joints["coxa"], p.POSITION_CONTROL,
                    targetPosition=coxa_a, force=50, maxVelocity=3,
                    physicsClientId=client,
                )
                p.setJointMotorControl2(
                    robot_id, joints["femur"], p.POSITION_CONTROL,
                    targetPosition=femur_a, force=50, maxVelocity=3,
                    physicsClientId=client,
                )
                p.setJointMotorControl2(
                    robot_id, joints["tibia"], p.POSITION_CONTROL,
                    targetPosition=tibia_a, force=50, maxVelocity=3,
                    physicsClientId=client,
                )

        sim_time += dt

        p.stepSimulation(physicsClientId=client)

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

    def set_gait(self, gait_name: str) -> None:
        logger.info("Set gait: %s", gait_name)
        self._send(("set_gait", gait_name))

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
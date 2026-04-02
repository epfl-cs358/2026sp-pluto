from __future__ import annotations

import logging
import math
import multiprocessing as mp
import queue
import time


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

    # Reference: adapted from RobotDog implementation (@Richard Bloemenkamp)
    # https://github.com/richardbloemenkamp/Robotdog/blob/master/robotdog.py
    # Includes robot structure definition, kinematics, multi-body construction, and initial joint control/friction settings.

    # Dog robot body and links.
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
    sh_foot = p.createCollisionShape(p.GEOM_SPHERE, radius=0.04, physicsClientId=client)

    link_masses = [
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        20,
    ]
    link_collision_shape_indices = [
        sh_roll,
        sh_hip,
        sh_knee,
        sh_foot,
        sh_roll,
        sh_hip,
        sh_knee,
        sh_foot,
        sh_roll,
        sh_hip,
        sh_knee,
        sh_foot,
        sh_roll,
        sh_hip,
        sh_knee,
        sh_foot,
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
        0,
        1,
        2,
        3,
        0,
        5,
        6,
        7,
        0,
        9,
        10,
        11,
        0,
        13,
        14,
        15,
        0,
    ]
    joint_types = [
        p.JOINT_REVOLUTE,
        p.JOINT_REVOLUTE,
        p.JOINT_REVOLUTE,
        p.JOINT_PRISMATIC,
        p.JOINT_REVOLUTE,
        p.JOINT_REVOLUTE,
        p.JOINT_REVOLUTE,
        p.JOINT_PRISMATIC,
        p.JOINT_REVOLUTE,
        p.JOINT_REVOLUTE,
        p.JOINT_REVOLUTE,
        p.JOINT_PRISMATIC,
        p.JOINT_REVOLUTE,
        p.JOINT_REVOLUTE,
        p.JOINT_REVOLUTE,
        p.JOINT_PRISMATIC,
        p.JOINT_PRISMATIC,
    ]
    axis = [
        [1, 0, 0],
        [0, 1, 0],
        [0, 1, 0],
        [0, 0, 1],
        [1, 0, 0],
        [0, 1, 0],
        [0, 1, 0],
        [0, 0, 1],
        [1, 0, 0],
        [0, 1, 0],
        [0, 1, 0],
        [0, 0, 1],
        [1, 0, 0],
        [0, 1, 0],
        [0, 1, 0],
        [0, 0, 1],
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

    # Keep prismatic links and foot contact parameters close to the original setup.
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

    max_linear_speed = 1.0
    max_turn_rate = math.radians(180)

    while p.isConnected(client):
        try:
            cmd = cmd_q.get_nowait()
            kind = cmd[0]
            if kind == "shutdown":
                break

            if kind in {"forward", "backward"}:
                duration, speed = cmd[1], cmd[2]
                direction = 1.0 if kind == "forward" else -1.0
                speed = max(0.0, min(float(speed), 1.0))
                vx = max_linear_speed * speed * direction
                end_t = time.time() + max(0.0, float(duration))
                last_t = time.time()
                while time.time() < end_t and p.isConnected(client):
                    now = time.time()
                    dt = now - last_t
                    last_t = now
                    x += math.cos(yaw) * vx * dt
                    y += math.sin(yaw) * vx * dt
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
                    time.sleep(1 / 240)

            if kind == "turn":
                angle, speed = float(cmd[1]), float(cmd[2])
                if angle > 180:
                    angle -= 360
                elif angle < -180:
                    angle += 360

                speed = max(0.0, min(speed, 1.0))
                if speed > 0 and angle != 0:
                    target = math.radians(angle)
                    rate = max_turn_rate * speed
                    duration = abs(target) / rate
                    end_t = time.time() + duration
                    sign = 1.0 if target > 0 else -1.0
                    last_t = time.time()
                    while time.time() < end_t and p.isConnected(client):
                        now = time.time()
                        dt = now - last_t
                        last_t = now
                        yaw += sign * rate * dt
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
                        time.sleep(1 / 240)

        except queue.Empty:
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
            time.sleep(1 / 240)

    if p.isConnected(client):
        p.disconnect(physicsClientId=client)


class PyBulletMotionController:
    """Imitates forward/turn motor commands in a PyBullet window."""

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
        self._process = mp.Process(target=_sim_process_main, args=(self._cmd_q,), daemon=True)
        self._process.start()
        logger.info("PyBullet simulation started")

    def forward(self, duration: float = 1.0, speed: float = 1.0) -> None:
        logger.info("Walking forward for %.2f second(s) at speed %.2f", duration, speed)
        self._send(("forward", float(duration), float(speed)))

    def backward(self, duration: float = 1.0, speed: float = 1.0) -> None:
        logger.info("Walking backward for %.2f second(s) at speed %.2f", duration, speed)
        self._send(("backward", float(duration), float(speed)))

    def turn(self, angle: float, speed: float = 1.0) -> None:
        logger.info("Turning by %.1f degrees at speed %.2f", angle, speed)
        self._send(("turn", float(angle), float(speed)))

    def stop(self) -> None:
        logger.info("Stop requested")

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

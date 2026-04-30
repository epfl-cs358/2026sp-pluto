import logging
import math
import multiprocessing as mp
import queue
import time
import pybullet as p
import ik_solver as IK

from gait import Walk, Trot, Gallop

logger = logging.getLogger(__name__)

LEG_JOINTS = {
    "fl": {"hip": 1, "knee": 2},
    "fr": {"hip": 5, "knee": 6},
    "bl": {"hip": 9, "knee": 10},
    "br": {"hip": 13, "knee": 14},
}
# TODO: roll moves as well

def _sim_process_main(cmd_q: mp.Queue) -> None:
    import pybullet as p
    import numpy as np

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

    gait = Gallop(period=0.6)

    sim_time = 0.0

    # Place joints in the standing pose immediately
    for joints in LEG_JOINTS.values():
        p.resetJointState(robot_id, joints["hip"], gait.stand_hip, physicsClientId=client)
        p.resetJointState(robot_id, joints["knee"], gait.stand_knee, physicsClientId=client)

    current_motion = "idle"
    current_speed = 1.0

    max_linear_speed = 5.0
    max_turn_rate = math.radians(120)

    # Set legs to desired positions
    def set_legs(xvals, yvals, zvals, vel=[1,1,1,1]):
        angles = IK.robot_IK(xvals, yvals, zvals, yoffh, hu, hl, [xhipf, yhipl])

        for leg in range(4):
            for joint in range(3):
                p.setJointMotorControl2(robot_id, joint + (4*leg), p.POSITION_CONTROL, targetPosition=angles[leg][joint], force=1000, maxVelocity=vel[leg])

    # Rotation matrix for yaw between robot-frame and world-frame
    def RotYaw(yaw):
        rho = np.array([[np.cos(yaw), -np.sin(yaw), 0], 
                        [np.sin(yaw), np.cos(yaw), 0],
                        [0, 0, 1]])
        return rho
    
    # Initial robot parameters
    yawri=1.3
    xrOi=np.array([1,1,0.5])
    legsRi=np.array([[xhipf,xhipf,xhipb,xhipb],
                [yhipl+0.1,-yhipl-0.1,yhipl+0.1,-yhipl-0.1],
                [-0.5,-0.5,-0.5,-0.5]])
    #Set body to the robot pos
    xbOi=xrOi
    #Init body position and orientation
    quat=p.getQuaternionFromEuler([0,0,yawri])
    p.resetBasePositionAndOrientation(robot_id,xbOi,quat)
    #Init leg abs pos
    Ryawri=RotYaw(yawri)
    legsO=(np.dot(Ryawri,legsRi).T + xbOi).T   #Apply rotation plus translation

    #Set the non-initial variables and matrix
    yawr=yawri
    xrO=xrOi
    xbO=xrO
    Ryawr=RotYaw(yawri)

    #Recalculate leg rel pos in robot frame and set the legs
    dlegsO=(legsO.T-xbO).T
    dlegsR=np.dot(Ryawr.T,dlegsO)
    set_legs(dlegsR[0],dlegsR[1],dlegsR[2],[1,1,1,1])

    #Calculate a new robot center position from the average of the feet positions
    #Calculate a new robot yaw direction also from the feet positions
    xfO=(legsO[:,0]+legsO[:,1])/2.0
    xbO=(legsO[:,2]+legsO[:,3])/2.0
    xrOn=(xfO+xbO)/2.0 + np.array([0,0,0.5])
    xfmbO=xfO-xbO
    yawrn=np.arctan2(xfmbO[1],xfmbO[0])

    #Walking speed (cannot be lower than 400)
    walkLoopSpd=400

    #Current leg to change position
    l=0
    #Init the center for the robot rotation to the current robot pos
    xrcO=xrO
    #Set the body position to the robot position
    xoff=0
    yoff=0
    #Init to walking fwd
    dr=0
    drp=0
    #Leg sequence
    lseq=[2,0,3,1]
    lseqp=[2,0,3,1]

    anim_timer = 0.0

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
                        gait = Walk(period=0.6)
                    elif name == "trot":
                        gait = Trot(period=0.6)
                    elif name == "gallop":
                        gait = Gallop(period=0.6)

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
                hip_angle, knee_angle = gait.get_leg_angles(sim_time, leg, forward_scale)
                p.setJointMotorControl2(
                    robot_id, joints["hip"], p.POSITION_CONTROL,
                    targetPosition=hip_angle, force=50, maxVelocity=6,
                    physicsClientId=client,
                )
                p.setJointMotorControl2(
                    robot_id, joints["knee"], p.POSITION_CONTROL,
                    targetPosition=knee_angle, force=50, maxVelocity=6,
                    physicsClientId=client,
                )
        else:
            for joints in LEG_JOINTS.values():
                p.setJointMotorControl2(
                    robot_id, joints["hip"], p.POSITION_CONTROL,
                    targetPosition=gait.stand_hip, force=50, maxVelocity=3,
                    physicsClientId=client,
                )
                p.setJointMotorControl2(
                    robot_id, joints["knee"], p.POSITION_CONTROL,
                    targetPosition=gait.stand_knee, force=50, maxVelocity=3,
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
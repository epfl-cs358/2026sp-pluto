class Servo:
    """
    Servo abstraction class.

    This class simulates a servo motor and optionally connects
    to a PyBullet joint for simulation.

    Attributes:
        name (str): Identifier for the servo
        joint_id (int): PyBullet joint index (optional)
        client (dict): Contains robot_id for PyBullet control
        min_angle (int): Minimum allowed angle
        max_angle (int): Maximum allowed angle
    """

    def __init__(self, name, joint_id=None, client=None, min_angle=0, max_angle=180):
        self.name = name
        self.joint_id = joint_id
        self.client = client
        self.min_angle = min_angle
        self.max_angle = max_angle
        self.angle = 90  # default position

    def set_angle(self, angle: float) -> None:
        """
        Set servo angle.

        Args:
            angle (float): Desired angle in degrees

        Raises:
            ValueError: If angle is outside allowed range
        """

        if angle < self.min_angle or angle > self.max_angle:
            raise ValueError(f"{self.name}: angle out of bounds")

        self.angle = angle
        print(f"[SIM] {self.name} set to {angle}°")

        # Send command to PyBullet if connected
        if self.joint_id is not None and self.client is not None:
            import pybullet as p

            # Convert degrees to radians (for pybullet)
            rad = angle * 3.14159 / 180.0

            p.setJointMotorControl2(
                bodyUniqueId=self.client["robot_id"],
                jointIndex=self.joint_id,
                controlMode=p.POSITION_CONTROL,
                targetPosition=rad,
                force=500,
            )

    def get_angle(self) -> float:
        """Return current servo angle."""
        return self.angle
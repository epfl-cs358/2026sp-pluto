from dataclasses import dataclass
from servo import Servo

@dataclass
class Leg:
    """
    Represents one robot leg with 3 servos:
    - hip
    - upper leg
    - lower leg
    """
    hip: Servo
    upper: Servo
    lower: Servo

    def set_angles(self, hip_angle: float, upper_angle: float, lower_angle: float) -> None:
        """Set all 3 joint angles"""
        self.hip.set_angle(hip_angle)
        self.upper.set_angle(upper_angle)
        self.lower.set_angle(lower_angle)

    def get_angles(self) -> tuple[float, float, float]:
        """Return current angles"""
        return (
            self.hip.get_angle(),
            self.upper.get_angle(),
            self.lower.get_angle()
        )